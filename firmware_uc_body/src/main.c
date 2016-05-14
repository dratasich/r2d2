/**
 * @file main.c
 * @date 06.05.2016
 * @author Denise Ratasich
 *
 * @brief Tests stepper motor.
 *
 *
 */

#include <avr/sleep.h>
#include "io.h"
#include "uart0.h"
#include "gpt.h"
#include "steppermotor.h"
#include "extint.h"

#define PERIOD_STEP     (1)

static uint8_t steppermotor_step_period = 3; // ms
static int8_t steppermotor_timerid;

void faster(void)
{
  uint16_t i;
  for(i = 0; i < 65500; i++); // debounce

  if (steppermotor_step_period < 100-PERIOD_STEP)
    steppermotor_step_period += PERIOD_STEP;

  gpt_setOverflowTime(steppermotor_step_period, steppermotor_timerid);

  uart0_printUInt8(steppermotor_step_period);
  uart0_println("");
}

void slower(void)
{
  uint16_t i;
  for(i = 0; i < 65500; i++); // debounce

  if (steppermotor_step_period > PERIOD_STEP)
    steppermotor_step_period -= PERIOD_STEP;

  gpt_setOverflowTime(steppermotor_step_period, steppermotor_timerid);

  uart0_printUInt8(steppermotor_step_period);
  uart0_println("");
}

void make_a_step(void)
{
  steppermotor_step(RIGHT);
}

void led_blink(void)
{
  TOGGLE_BIT(PORTL, PL6);
}

int main(void)
{
  uart0_init();
  gpt_init();

  extint_init();
  if (extint_requestInt(4, EXTINT_TRIGGER_FALLING_EDGE, faster) == -1)
    uart0_println("INT4 already used");
  if (extint_requestInt(5, EXTINT_TRIGGER_FALLING_EDGE, slower) == -1)
    uart0_println("INT5 already used");

  steppermotor_init(FULL);
  steppermotor_timerid = gpt_requestTimer(steppermotor_step_period, make_a_step);
  uart0_print("steppermotor timer id: ");
  uart0_printUInt8(steppermotor_timerid);
  uart0_println("");

  // led blink test
  DDRL |= (1<<PL6);
  gpt_requestTimer(1000, led_blink);
  
  uart0_println("initialized");

  while(1) {
    sleep_mode();
  }

  return 0;
}
