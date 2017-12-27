/**
 * @file gpt.h
 * @date 19.07.2013
 * @author Thomas Schlaudoschitz, Denise Ratasich
 *
 * @brief Header of the general purpose timer with 1ms resolution.
 */

#ifndef __GPTIMER_H__
#define __GPTIMER_H__

#include <avr/io.h>

#define	GPT_MAX_TIMERS		10

typedef enum { MS1, US100, UNSPEC } gpt_resolution_t;

/** Initializes general purpose timer. */
gpt_resolution_t gpt_init(gpt_resolution_t resolution);

/** Returns current ms from startup (overflow after about 49 days!). If you
 * want to count on your own. */
uint32_t gpt_getTime(void);

/** Request a GPT. */
int8_t gpt_requestTimer(uint16_t overflowTime, void (*callback)(void));

/** Change overflow time of a GPT. */
void gpt_setOverflowTime(uint16_t overflowTime, int8_t timerId);

/** Resets the remaining time to overflow time of a GPT (for watchdog-like
 * usage). */
void gpt_reset(int8_t timerId);

/** Release a timer, e.g., if not needed any more. */
void gpt_releaseTimer(int8_t timerId);

// Timer 2 compare interrupt

#endif
