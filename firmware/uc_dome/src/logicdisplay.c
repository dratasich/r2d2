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
 * pins) + 6 uC pins are used to switch through the columns. The rows are also
 * directly connected to the uC.
 *
 * Settings: >60 frames per second necessary for the eye. 60Hz * 8 rows of
 * front display gives row switch rate of <2ms. Switching all columns of the
 * rear display with >60Hz we have to update at <0.7ms.
 *
 * Low on row r and high on column c means the LED_{r,c} is on.
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
    LOGICDISPLAY_REAR_PORT2 |= (frame & LD_REAR_ROWS_UC);
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

/** Changes the frame, i.e., the LED status. */
static void logicdisplay_frame(void)
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

void logicdisplay_init(void)
{
    // init pinout
    // front
    LOGICDISPLAY_FRONT_ROWS_DDR = 0xFF;
    LOGICDISPLAY_FRONT_COLS_DDR = 0xFF;
    // rear
    LOGICDISPLAY_REAR_DDR1 = LOGICDISPLAY_REAR_MASK1;
    LOGICDISPLAY_REAR_DDR2 = LOGICDISPLAY_REAR_MASK2;

    // init frame change
    gpt_init(US100);
    gpt_requestTimer(2500, logicdisplay_frame);
    gpt_requestTimer(5, logicdisplay_step);

    // init random number generator
    srand(1);
}
