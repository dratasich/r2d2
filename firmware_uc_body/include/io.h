/**
 * @file io.h
 * @author Denise Ratasich
 * @date 06.05.2016
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

// ---------------------------------------------------------------
// external interrupts ports
// ---------------------------------------------------------------
#define EXTINTA_PORT    PORTD   // pin 0..3, INT0..3
#define EXTINTA_DDR     DDRD
#define EXTINTB_PORT    PORTE   // pin 4..7, INT4..7
#define EXTINTB_DDR     DDRE

// ----------------------------------------------------------------------
// ports, pins, ADC-channels, etc.
// ----------------------------------------------------------------------
// stepper motor
#define STEPPERMOTOR_PORT	PORTA
#define STEPPERMOTOR_DDR	DDRA
#define STEPPERMOTOR_A1		PA3 // red
#define STEPPERMOTOR_A2		PA2 // green
#define STEPPERMOTOR_B1		PA1 // black
#define STEPPERMOTOR_B2		PA0 // yellow

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
