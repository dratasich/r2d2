/**
 * @file main.c
 * @date 2017-11-07
 * @author Denise Ratasich
 *
 * @brief Controls front logic display.
 */

#include <avr/sleep.h>
#include "io.h" // toggle bit
#include "uart0.h"
#include "gpt.h"
#include "logicdisplay.h"


/** Toggles alive LED. */
void led_blink(void)
{
  TOGGLE_BIT(PORTB, PB7);
}

int main(void)
{
  uart0_println("[INFO ] init UART0");
  uart0_init();

  uart0_println("[INFO ] init logic display");
  logicdisplay_init();

  uart0_println("[INFO ] init alive LED");
  gpt_resolution_t res = gpt_init(MS1);
  DDRB |= (1<<PB7);
  switch(res) {
  case MS1:
    gpt_requestTimer(1000, led_blink);
    break;
  case US100:
    gpt_requestTimer(10000, led_blink);
    break;
  default:
    uart0_println("[ERROR] initializing alive LED - unknown resolution");
    break;
  }

  uart0_println("[INFO ] initialization done");
  uart0_println("[INFO ] start main loop ...");

  while(1) {
    //sleep_mode();
  }

  return 0;
}
