/**
 * @file pwm.h
 * @author Denise Ratasich
 * @date 16.05.2016
 *
 * @brief Header of DC motor driver.
 */

#ifndef __PWM_H__
#define __PWM_H__	

#include <avr/io.h>	// e.g. uint16_t

#define PWM_TOP         (800)

enum pwm_output {
  PWM_OC1A = 0x01,
  PWM_OC1B = 0x02,
  PWM_OC1C = 0x04
};

/** Initializes timer 1 for fast PWM mode. */
void pwm_init(uint8_t oc_mask);

/** Increases duty cycle of a PWM signal on output pin. */
void pwm_inc(enum pwm_output oc, uint16_t step);

/** Decreases duty cycle of a PWM signal on output pin. */
void pwm_dec(enum pwm_output oc, uint16_t step);

/** Set duty cycle of an output pin, i.e., OCR1x value. */
void pwm_set(enum pwm_output oc, uint16_t value);

/** Returns current duty cycle of an output pin, i.e., OCR1x value. */
uint16_t pwm_get(enum pwm_output oc);

#endif
