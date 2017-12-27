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
 * Settings: >60 frames per second necessary for the eye.
 *
 * Low on row r and high on column c means the LED_{r,c} is on.
 */

#include "io.h"	// port, pins definition
#include "gpt.h"

#include <avr/io.h>	// e.g., uint8_t
#include <stdlib.h>     // rand, srand

#define LD_FRONT_ROWS           (8)
#define LD_FRONT_COLS           (7)

/** Status of the LEDs (0/1 .. on/off; one byte -- columns -- for each row) */
static volatile uint8_t frame_front[LD_FRONT_ROWS];


/** Applys columns of the frame to the LEDs subsequently. */
static void logicdisplay_step(void)
{
    static uint8_t row = 0;

    // front
    // turn off current row
    LOGICDISPLAY_FRONT_ROWS_PORT = 0;
    // switch to next row
    row = (row + 1) % LD_FRONT_ROWS;
    // apply current column
    LOGICDISPLAY_FRONT_COLS_PORT = frame_front[row];
    // turn on a row
    LOGICDISPLAY_FRONT_ROWS_PORT = (1 << row);

    // rear
    // TODO
}

/** Changes the frame, i.e., the LED status. */
static void logicdisplay_frame(void)
{
    // map rand values to LEDs
    // front
    for (uint8_t r = 0; r < LD_FRONT_ROWS; r++) {
        uint16_t randno = (uint16_t) rand();
        frame_front[r] = randno;
        frame_front[r] = (randno >> 8);
    }
    // rear
    // TODO
}

void logicdisplay_init(void)
{
    // init pinout
    // front
    LOGICDISPLAY_FRONT_ROWS_DDR = 0xFF;
    LOGICDISPLAY_FRONT_COLS_DDR = 0xFF;
    // rear
    // TODO

    // init frame change
    gpt_init();
    gpt_requestTimer(250, logicdisplay_frame);
    gpt_requestTimer(1, logicdisplay_step);

    // init random number generator
    srand(1);
}
