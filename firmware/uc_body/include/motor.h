/**
 * @file motor.h
 * @author Denise Ratasich
 * @date 16.05.2016
 *
 * @brief Header of DC motor driver.
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__	

#include <avr/io.h>	// e.g. uint16_t

/** Initializes pins, position, mode. */
void motor_init(void);

/** Changes speed of the motor. */
void motor_inc(uint16_t step);
void motor_dec(uint16_t step);

/** Returns speed (including direction). */
int16_t motor_getSpeed(void);
/** Returns direction, i.e., sign(speed). */
int8_t motor_getDirection(void);

#endif
