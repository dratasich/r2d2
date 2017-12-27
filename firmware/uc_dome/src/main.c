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
  TOGGLE_BIT(PORTA, PA7);
}

int main(void)
{
  // alive LED
  gpt_init();
  DDRA |= (1<<PA7);
  gpt_requestTimer(1000, led_blink);

  logicdisplay_init();

  uart0_println("initialized");

  while(1) {
    //sleep_mode();
  }

  return 0;
}
