/**
 * @file logicdisplay.c
 * @author Denise Ratasich
 * @date 2017-11-07
 *
 * @brief Implementation of the logic displays.
 *
 * The front logic display consists of two LED matrices with 4x7 LEDs. The rows
 * are powered by an ULN2804 chip. The columns are controlled by uC pins.
 *
 * The rear logic display has 4 rows and 22 columns, so a 16bit-demux (4 uC
 * pins + 1 enable) + 6 uC pins are used to switch through the columns. The
 * rows are also directly connected to the uC.
 *
 * Settings: >60 frames per second necessary for the eye. 60Hz * 8 rows of
 * front display gives row switch rate of <2ms. Switching all 22 columns of the
 * rear display with >60Hz we have to update at <0.7ms.
 *
 * Low on row r and high on column c means the LED_{r,c} is on for front logic
 * display, rear is vice versa.
 */

#include "logicdisplay.h"
#include "io.h"	// port, pins definition
#include "gpt.h"

#include <avr/io.h>	// e.g., uint8_t
#include <stdlib.h>     // rand, srand

#define LD_FRONT_ROWS           (8)
#define LD_FRONT_COLS           (7)
#define LD_REAR_ROWS            (4)
#define LD_REAR_COLS            (22)

#define LD_REAR_COLS_UC         (0xFC) // column pins to LEDs directly
#define LD_REAR_DISABLE_DM      (0x02) // enable pin for demux (active low)
#define LD_REAR_COLS_DM         (0x0F) // column pins to demux
#define LD_REAR_ROWS_UC         (0xF0) // row pins of PORT2 (active high)

/** Status of the LEDs (0/1 .. on/off; one byte -- columns -- for each row) */
static volatile uint8_t frame_front[LD_FRONT_ROWS];
static volatile uint8_t frame_rear[LD_REAR_COLS];

/** Display mode. */
static logicdisplay_mode_t mode = LOGICDISPLAY_RANDOM;

// bits are switched w.r.t. wiring/numbering of columns
// (col0->bit7, 1->6, ..., 7->0)
static const uint8_t ld_digit_front[10][4] = {
    { 0b111, 0b101, 0b101, 0b111 }, // 0
    { 0b100, 0b100, 0b100, 0b100 },
    { 0b111, 0b100, 0b001, 0b111 },
    { 0b111, 0b100, 0b100, 0b111 },
    { 0b101, 0b111, 0b100, 0b100 },
    { 0b111, 0b001, 0b100, 0b111 },
    { 0b111, 0b001, 0b111, 0b111 },
    { 0b111, 0b100, 0b100, 0b100 },
    { 0b111, 0b111, 0b111, 0b111 },
    { 0b111, 0b111, 0b100, 0b111 }  // 9
};

static const uint8_t ld_digit_rear[10][3] = {
    { 0b1111, 0b1001, 0b1111 }, // 0
    { 0b0000, 0b0000, 0b1111 },
    { 0b1101, 0b1001, 0b1011 },
    { 0b1001, 0b1001, 0b1111 },
    { 0b0011, 0b0010, 0b1111 },
    { 0b1011, 0b1001, 0b1101 },
    { 0b1111, 0b1101, 0b1101 },
    { 0b0001, 0b0001, 0b1111 },
    { 0b1111, 0b1111, 0b1111 },
    { 0b1011, 0b1011, 0b1111 }  // 9
};
static const uint8_t ld_char_rear['Z'-'A'+1][3] = {
    { 0b1111, 0b0011, 0b1111 }, // 'A'
    { 0b1111, 0b1010, 0b1110 },
    { 0b1111, 0b1001, 0b1001 },
    { 0b1111, 0b1001, 0b0110 },
    { 0b1111, 0b1101, 0b1001 },
    { 0b1111, 0b0011, 0b0001 },
    { 0b1111, 0b1001, 0b1101 }, // 'G'
    { 0b1111, 0b0010, 0b1111 },
    { 0b0000, 0b1111, 0b0000 },
    { 0b1100, 0b1000, 0b1111 },
    { 0b1111, 0b0100, 0b1010 },
    { 0b1111, 0b1000, 0b1000 },
    { 0b1101, 0b0010, 0b1101 }, // 'M'
    { 0b1100, 0b0010, 0b1100 },
    { 0b1110, 0b1010, 0b1110 },
    { 0b1111, 0b0011, 0b0011 },
    { 0b1111, 0b1101, 0b1111 },
    { 0b1111, 0b0101, 0b1011 }, // 'R'
    { 0b1011, 0b1001, 0b1101 },
    { 0b0001, 0b1111, 0b0001 },
    { 0b1111, 0b1000, 0b1111 },
    { 0b0111, 0b1000, 0b0111 }, // 'V'
    { 0b1011, 0b0100, 0b1011 },
    { 0b1001, 0b0110, 0b1001 },
    { 0b0011, 0b1100, 0b0011 },
    { 0b1101, 0b1001, 0b1011 }  // 'Z'
};


// modes of operation

/** Changes the frame in mode 'RANDOM'. */
static void logicdisplay_frame_random(void)
{
    // map rand values to LEDs
    // front
    for (uint8_t r = 0; r < LD_FRONT_ROWS; r++) {
        frame_front[r] = (uint8_t) rand();
    }
    // rear
    for (uint8_t c = 0; c < LD_REAR_COLS; c++) {
        frame_rear[c] = (uint8_t) rand();
    }
}

/** Changes the frame in mode 'CHAR'. */
void logicdisplay_print(const char *front_up, const char *front_lo,
                        const char *rear)
{
    uint8_t fu_max = (LD_FRONT_COLS+1)/4;
    uint8_t fl_max = (LD_FRONT_COLS+1)/4;
    uint8_t re_max = (LD_REAR_COLS+1)/4;

    // reset frames
    for (uint8_t r = 0; r < LD_FRONT_ROWS; r++)
        frame_front[r] = 0x00;
    for (uint8_t c = 0; c < LD_REAR_COLS; c++)
        frame_rear[c] = 0x00;

    // map chars to LEDs
    for (uint8_t i = 0; front_up[i] != '\0' && i < fu_max; i++) {
        if (front_up[i] >= '0' && front_up[i] <= '9') {
            uint8_t off = (fu_max-1 - i) * 4;
            const uint8_t *c = ld_digit_front[front_up[i] - '0'];
            frame_front[0] |= c[0] << off;
            frame_front[1] |= c[1] << off;
            frame_front[2] |= c[2] << off;
            frame_front[3] |= c[3] << off;
        }
    }
    for (uint8_t i = 0; front_lo[i] != '\0' && i < fl_max; i++) {
        if (front_lo[i] >= '0' && front_lo[i] <= '9') {
            uint8_t off = (fl_max-1 - i) * 4;
            const uint8_t *c = ld_digit_front[front_lo[i] - '0'];
            frame_front[4] |= c[0] << off;
            frame_front[5] |= c[1] << off;
            frame_front[6] |= c[2] << off;
            frame_front[7] |= c[3] << off;
        }
    }
    for (uint8_t i = 0; rear[i] != '\0' && i < re_max; i++) {
        uint8_t off = i * 4;
        if (rear[i] >= '0' && rear[i] <= '9') {
            const uint8_t *c = ld_digit_rear[rear[i] - '0'];
            frame_rear[off] |= c[0];
            frame_rear[off+1] |= c[1];
            frame_rear[off+2] |= c[2];
        }
        if (rear[i] >= 'A' && rear[i] <= 'Z') {
            const uint8_t *c = ld_char_rear[rear[i] - 'A'];
            frame_rear[off] |= c[0];
            frame_rear[off+1] |= c[1];
            frame_rear[off+2] |= c[2];
        }
    }
}

/** Changes the frame in mode 'CHASER'. */
static void logicdisplay_frame_chaser(void)
{
    static int8_t posf = 0, posr = 0;
    static int8_t dirf = 1, dirr = 1;
    static uint8_t slow = 0;

    // light up next column
    for (uint8_t r = 0; r < LD_FRONT_ROWS; r++) {
        frame_front[r] = 1 << posf;
    }
    // rear
    for (uint8_t c = 0; c < LD_REAR_COLS; c++) {
        if (c == posr)
            frame_rear[c] = 0xFF;
        else
            frame_rear[c] = 0x00;
    }

    // update front light chaser position
    if (slow >= 1) {
        if (posf >= LD_FRONT_COLS - 1)
            dirf = -1;
        if (posf <= 0)
            dirf = +1;
        posf = (posf + dirf) % LD_FRONT_COLS;
    }
    slow = (slow + 1) % 2;

    // update rear light chaser position
    if (posr >= LD_REAR_COLS - 1)
        dirr = -1;
    if (posr <= 0)
        dirr = +1;
    posr = (posr + dirr) % LD_REAR_COLS;
}


// low-level control

/** Turn off all columns. */
static void logicdisplay_rear_reset_columns(void) {
    // 1 .. off
    LOGICDISPLAY_REAR_PORT1 |= LD_REAR_COLS_UC;
    // disable demux (all outputs high)
    LOGICDISPLAY_REAR_PORT1 |= LD_REAR_DISABLE_DM;
}

/** Switch to the specified column (0 .. LD_REAR_COLS-1). */
static void logicdisplay_rear_set_column(uint8_t column) {
    // 0 .. on
    if (column < 16) {
        // first 16 LED columns controlled by demux
        // set column and enable
        LOGICDISPLAY_REAR_PORT2 &= ~(LD_REAR_COLS_DM);
        LOGICDISPLAY_REAR_PORT2 |= column;
        LOGICDISPLAY_REAR_PORT1 &= ~(LD_REAR_DISABLE_DM);
    } else {
        // top 6 LED columns controlled by uc directly
        LOGICDISPLAY_REAR_PORT1 &= ~(1 << (column-16+2));
    }
}

/** Applys a row frame to the LEDs. */
static void logicdisplay_rear_apply(uint8_t frame) {
    // 1 .. on
    LOGICDISPLAY_REAR_PORT2 &= ~(LD_REAR_ROWS_UC);
    LOGICDISPLAY_REAR_PORT2 |= ((frame<<4) & LD_REAR_ROWS_UC);
}

/** Applys columns of the frame to the LEDs subsequently. */
static void logicdisplay_step(void)
{
    static uint8_t row = 0;
    static uint8_t col = 0;

    // front
    // turn off current row
    LOGICDISPLAY_FRONT_ROWS_PORT = 0;
    // switch to next row
    row = (row + 1) % LD_FRONT_ROWS;
    // apply current column
    LOGICDISPLAY_FRONT_COLS_PORT &= ~LOGICDISPLAY_FRONT_COLS_MASK;
    LOGICDISPLAY_FRONT_COLS_PORT |= frame_front[row] << 1;
    // turn on a row
    LOGICDISPLAY_FRONT_ROWS_PORT = (1 << row);

    // rear
    logicdisplay_rear_reset_columns();
    col = (col + 1) % LD_REAR_COLS;
    logicdisplay_rear_apply(frame_rear[col]);
    logicdisplay_rear_set_column(col);
}


// user interface

void logicdisplay_init(void)
{
    // init pinout
    // front
    LOGICDISPLAY_FRONT_ROWS_DDR = 0xFF;
    LOGICDISPLAY_FRONT_COLS_DDR = LOGICDISPLAY_FRONT_COLS_MASK;
    // rear
    LOGICDISPLAY_REAR_DDR1 = LOGICDISPLAY_REAR_MASK1;
    LOGICDISPLAY_REAR_DDR2 = LOGICDISPLAY_REAR_MASK2;

    // init frame change
    gpt_init(US100);
    gpt_requestTimer(5, logicdisplay_step);
    logicdisplay_mode(LOGICDISPLAY_RANDOM);

    // init random number generator
    srand(1);
}

void logicdisplay_mode(logicdisplay_mode_t new_mode)
{
    static int8_t gptid = -1;

    // quit current mode
    switch(mode) {
    case LOGICDISPLAY_RANDOM:
    case LOGICDISPLAY_CHASER:
        gpt_releaseTimer(gptid);
        break;
    case LOGICDISPLAY_CHAR:
        // nothing to do
        break;
    default:
        // should be impossible
        break;
    }

    // apply new mode
    switch(new_mode) {
    case LOGICDISPLAY_RANDOM:
        gptid = gpt_requestTimer(2500, logicdisplay_frame_random);
        break;
    case LOGICDISPLAY_CHAR:
        // nothing to do here
        // set character to update frame
        break;
    case LOGICDISPLAY_CHASER:
        gptid = gpt_requestTimer(500, logicdisplay_frame_chaser);
        break;
    default:
        // shall not be used -- abort
        return;
    }

    // save new mode
    mode = new_mode;
}
