/**
 * @file gpt.c
 * @author Thomas Schlaudoschitz, Denise Ratasich
 * @date 19.07.2013
 *
 * @brief General purpose timer implementation (1ms resolution).
 *
 * Timer 2 (8-bit timer) used as general purpose timer (GPT). Timer resolution
 * is 1ms. Maximum overflow time is 65535ms.
 */

#include <avr/interrupt.h>
#include "gpt.h"

static uint32_t gptTime = 0;

/** Current number of timers in use. */
static uint8_t gptNrTimers = 0;

/** Timer element, collects information to timer requests. */
static GPTimerStruct_t gptTimerField[GPT_MAX_TIMERS];

void gpt_init()
{
  uint8_t i;

  // mark timer elements as unused
  for (i = 0; i < GPT_MAX_TIMERS; i++)
    gptTimerField[i].overflowTime = 0;

  // Timer 2 (8-Bit Timer) in CTC mode
  TCCR2A = 0x02;
  TCCR2B |= (4<<CS20); //prescaler = 64, timer started
  // tpuls = 4 us
  OCR2A = 249; // 250 pulses => 1 ms interrupt
  TIMSK2 |= (1<<OCIE2A); // enable compare match interrupt

  sei();
}

uint32_t gpt_getTime()
{
  return gptTime;
}

int8_t gpt_requestTimer(uint16_t overflowTime, void (*callback)(void))
{
  uint8_t i;
  int8_t timerId = -1;

  // search for a free timer element
  if (gptNrTimers < GPT_MAX_TIMERS)
  {
    for (i = 0; i < GPT_MAX_TIMERS; i++)
    {
      if (gptTimerField[i].overflowTime == 0)
      {
	// free element found
	gptTimerField[i].callback = callback;
	gptTimerField[i].remainingTime = overflowTime;
	gptTimerField[i].overflowTime = overflowTime;
	timerId = i;
	gptNrTimers++;
	break;
      }
    }
  }

  return timerId;
}

void gpt_setOverflowTime(uint16_t overflowTime, int8_t timerId)
{
  if (timerId >= 0 && timerId < GPT_MAX_TIMERS) {
    gptTimerField[timerId].remainingTime = overflowTime;
    gptTimerField[timerId].overflowTime = overflowTime;
  } 
}

void gpt_reset(int8_t timerId)
{
  if(timerId >= 0 && timerId < GPT_MAX_TIMERS)
    gptTimerField[timerId].remainingTime = gptTimerField[timerId].overflowTime;
}

void gpt_releaseTimer(int8_t timerId)
{
  if(timerId >= 0 && timerId < GPT_MAX_TIMERS) {
    gptTimerField[timerId].overflowTime = 0;
    gptNrTimers--;
  }
}

// called every 1ms
ISR(TIMER2_COMPA_vect)
{
  uint8_t i;

  gptTime++;

  for(i = 0; i < GPT_MAX_TIMERS; i++)
  {
    if(gptTimerField[i].overflowTime != 0) // is timer element used?
    {
      gptTimerField[i].remainingTime--;
      if(gptTimerField[i].remainingTime == 0) // time ellapsed?
      {
	gptTimerField[i].remainingTime = gptTimerField[i].overflowTime;
	(*(gptTimerField[i].callback))(); // call callback-function
      }
    }
  }
}
