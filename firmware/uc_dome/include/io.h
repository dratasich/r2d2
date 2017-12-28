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

// ----------------------------------------------------------------------
// modules
// ----------------------------------------------------------------------

// Timer 2 as general purpose timer
// UART0 used for debugging

// ----------------------------------------------------------------------
// ports, pins, ADC-channels, etc.
// ----------------------------------------------------------------------
// front logic display
#define LOGICDISPLAY_FRONT_ROWS_PORT    PORTA
#define LOGICDISPLAY_FRONT_ROWS_DDR     DDRA
#define LOGICDISPLAY_FRONT_COLS_PORT    PORTC
#define LOGICDISPLAY_FRONT_COLS_DDR     DDRC
// rear logic display
#define LOGICDISPLAY_REAR_PORT1         PORTF
#define LOGICDISPLAY_REAR_DDR1          DDRF
#define LOGICDISPLAY_REAR_MASK1         (0xFE) // used pins of PORT1
#define LOGICDISPLAY_REAR_PORT2         PORTK
#define LOGICDISPLAY_REAR_DDR2          DDRK
#define LOGICDISPLAY_REAR_MASK2         (0xFF) // used pins of PORT2

// UART0 pins are automatically controlled (so there are no
// pin definitions needed)
// RXD0: PE0
// TXD0: PE1

// ----------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------
#define SET_BIT(PORT, BITNUM)    ((PORT) |=  (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM)  ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^=  (1<<(BITNUM)))

#define IS_BIT_SET(PORT, BITNUM)    ((PORT) & (1<<(BITNUM)))

#endif
