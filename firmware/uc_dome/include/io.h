/**
 * @file io.h
 * @author Denise Ratasich
 * @date 2017-11-07
 *
 * @brief Collects all ports and pins together.
 *
 * All defines of ports and pins for the modules are only here - to
 * have an overview about what hardware is already in use.
 */

#ifndef __IO_H__
#define __IO_H__

#include <avr/io.h>	// port definitions

// ----------------------------------------------------------------------
// uC properties
// ----------------------------------------------------------------------
#define FOSZ			16000000

// Timer 2 as general purpose timer

// ----------------------------------------------------------------------
// ports, pins, ADC-channels, etc.
// ----------------------------------------------------------------------
// front logic display
#define LOGICDISPLAY_FRONT_ROWS_PORT    PORTC
#define LOGICDISPLAY_FRONT_ROWS_DDR     DDRC
#define LOGICDISPLAY_FRONT_COLS_PORT    PORTL
#define LOGICDISPLAY_FRONT_COLS_DDR     DDRL

// UART0 and UART1 pins are automatically controlled (so there are no
// pin definitions needed)
// RXD0: PE0,
// TXD0: PE1,
// RXD1: PD2,
// TXD1: PD3

// ----------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------
#define SET_BIT(PORT, BITNUM)    ((PORT) |=  (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM)  ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^=  (1<<(BITNUM)))

#define IS_BIT_SET(PORT, BITNUM)    ((PORT) & (1<<(BITNUM)))

#endif
