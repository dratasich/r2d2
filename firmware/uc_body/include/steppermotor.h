/**
 * @file steppermotor.h
 * @author Denise Ratasich
 * @date 06.05.2016
 *
 * @brief Header of stepper motor driver.
 */

#ifndef __STEPPERMOTOR_H__
#define __STEPPERMOTOR_H__	

#include <avr/io.h>	// e.g. uint16_t

enum steppermotor_direction {
  RIGHT = 0,
  LEFT
};

enum steppermotor_mode {
  HALF = 0,
  FULL
};

/** Initializes pins, position, mode. */
void steppermotor_init(enum steppermotor_mode mode);

/** 
 * @brief Performs a single step.
 *
 * @param clockwise Direction of rotation (clockwise/right or
 * counter-clockwise/left). 
 */
void steppermotor_step(enum steppermotor_direction dir);

/** Start exciting, turn on. */
void steppermotor_on(void);

/** Stop exciting, all coils off. */
void steppermotor_off(void);

/** Returns current position in steps taken from startup. */
int32_t steppermotor_position(void);

#endif
