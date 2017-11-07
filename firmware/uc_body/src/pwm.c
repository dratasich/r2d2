/**
 * @file pwm.c
 * @author Denise Ratasich
 * @date 16.05.2016
 *
 * @brief Implementation of PWM driver.
 * 
 * Generates PWM signals on OC1A.
 */

#include <avr/interrupt.h>
#include "pwm.h"
#include "io.h"	// pin macros

/** Current value of OCR1A. */
static uint16_t ocr1a;

/** Sets current value of OCR1A and applies it to the output compare unit. */
static inline void pwm_setOCR1A(uint16_t newOcr1a)
{
  ocr1a = newOcr1a;
  OCR1A = ocr1a;
}

void pwm_init(uint8_t oc_mask)
{
  // mode "Fast PWM, top ICR" (WGM1: 0xE)
  SET_BIT(TCCR1B, WGM13);
  SET_BIT(TCCR1B, WGM12);
  SET_BIT(TCCR1A, WGM11);
  CLEAR_BIT(TCCR1A, WGM10);

  // set top value of timer (10kHz -> period: 100us)
  ICR1 = PWM_TOP;

  // init compare match units
  if (oc_mask & PWM_OC1A) {
    // clear on compare match, set at bottom
    SET_BIT(TCCR1A, COM1A1);
    CLEAR_BIT(TCCR1A, COM1A0);
    // set initial motor speed to 0
    pwm_setOCR1A(0);
  }

  // clock source: no prescaler
  CLEAR_BIT(TCCR1B, CS12);
  CLEAR_BIT(TCCR1B, CS11);
  SET_BIT(TCCR1B, CS10);
}

void pwm_inc(enum pwm_output oc, uint16_t step)
{
  switch(oc) {
  case PWM_OC1A:
    if (ocr1a <= PWM_TOP - step) {
      pwm_setOCR1A(ocr1a + step);
    }
    break;
  case PWM_OC1B:
  case PWM_OC1C:
    // TODO
    break;
  }
}

void pwm_dec(enum pwm_output oc, uint16_t step)
{
  switch(oc) {
  case PWM_OC1A:
    if (ocr1a >= step) {
      pwm_setOCR1A(ocr1a - step);
    }
    break;
  case PWM_OC1B:
  case PWM_OC1C:
    // TODO
    break;
  }
}

void pwm_set(enum pwm_output oc, uint16_t value)
{
  switch(oc) {
  case PWM_OC1A:
    pwm_setOCR1A(value);
    break;
  case PWM_OC1B:
  case PWM_OC1C:
    // TODO
    break;
  } 
}

uint16_t pwm_get(enum pwm_output oc)
{
  switch(oc) {
  case PWM_OC1A:
    return ocr1a;
  case PWM_OC1B:
  case PWM_OC1C:
    // TODO
    break;
  }

  return 0;
}
