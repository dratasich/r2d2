/**
 * @file extint.h 
 * @date 14.05.2016
 * @author Denise Ratasich
 *
 * @brief Header of the external interrupt module.
 */

#ifndef __EXTINT_H__
#define __EXTINT_H__

#include <avr/io.h>

#define EXTINT_TRIGGER_LOW_LEVEL        (0)
#define EXTINT_TRIGGER_ANY_EDGE         (1)
#define EXTINT_TRIGGER_FALLING_EDGE     (2)
#define EXTINT_TRIGGER_RISING_EDGE      (3)

/** Initializes this module. */
void extint_init();

/** Request an external interrupt. */
int8_t extint_requestInt(int8_t no, uint8_t trigger, void (*callback)(void));

/** Release an external interrupt, e.g., if not needed any more. */
void extint_releaseInt(int8_t no);

// external interrupt routines INT0 .. INT7

#endif
