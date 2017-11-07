/**
 * @file main.c
 * @date 06.05.2016
 * @author Denise Ratasich
 *
 * @brief Tests dome motor.
 *
 *
 */

#include <avr/sleep.h>
#include "io.h"
#include "uart0.h"
#include "gpt.h"
#include "motor.h"
#include "pwm.h"
#include "extint.h"

#define STEP     (100)

void faster(void)
{
  uint16_t i;
  for(i = 0; i < 65500; i++); // debounce

  motor_inc(100);

  uart0_printInt16(motor_getSpeed());
  uart0_println("");
}

void slower(void)
{
  uint16_t i;
  for(i = 0; i < 65500; i++); // debounce

  motor_dec(100);

  uart0_printInt16(motor_getSpeed());
  uart0_println("");
}

void led_blink(void)
{
  TOGGLE_BIT(PORTA, PA7);
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

  motor_init();

  // led blink test
  DDRA |= (1<<PA7);
  gpt_requestTimer(1000, led_blink);
  
  uart0_println("initialized");

  while(1) {
    //sleep_mode();
  }

  return 0;
}
