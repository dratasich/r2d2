/**
 * @file gpt.c
 * @author Thomas Schlaudoschitz, Denise Ratasich
 * @date 19.07.2013
 *
 * @brief General purpose timer implementation (1ms resolution).
 *
 * Timer 2 (8-bit timer) used as general purpose timer (GPT). Timer resolution
 * is 1ms. Maximum overflow time is 65535ms (i.e., 65s).
 */

#include <avr/interrupt.h>
#include "gpt.h"

typedef struct {
    void (*callback)(void);
    uint16_t overflowTime;
    uint16_t remainingTime;
} GPTimer_t;

static volatile uint32_t time = 0;

/** Current number of timers in use. */
static uint8_t numTimers = 0;

/** Timer element, collects information to timer requests. */
static volatile GPTimer_t timers[GPT_MAX_TIMERS];

/** Initialized flag. */
static gpt_resolution_t initialized = UNSPEC;

gpt_resolution_t gpt_init(gpt_resolution_t resolution)
{
    if (initialized != UNSPEC)
        return initialized;

    // mark timer elements as unused
    for (uint8_t i = 0; i < GPT_MAX_TIMERS; i++)
        timers[i].overflowTime = 0;

    // init Timer 2 (8-Bit Timer)
    TCCR2A = 0x02; // CTC mode
    switch(resolution) {
    case MS1:
        // prescaler = 64 (tpuls = 4us), starts timer
        TCCR2B |= (4<<CS20);
        OCR2A = 249; // 250 pulses => 1 ms interrupt
        break;
    case US100:
        // prescaler = 8 (tpuls = 0.5 us), starts timer
        TCCR2B |= (2<<CS20);
        OCR2A = 199; // 200 pulses => 0.1 ms interrupt
        break;
    default:
        // timer won't be started
        return initialized = UNSPEC;
    }

    // enable compare match interrupt
    TIMSK2 |= (1<<OCIE2A);
    sei();

    return initialized = resolution;
}

uint32_t gpt_getTime(void)
{
    return time;
}

int8_t gpt_requestTimer(uint16_t overflowTime, void (*callback)(void))
{
    uint8_t i;
    int8_t timerId = -1;

    // search for a free timer element
    if (numTimers < GPT_MAX_TIMERS)
    {
        for (i = 0; i < GPT_MAX_TIMERS; i++)
        {
            if (timers[i].overflowTime == 0)
            {
                // free element found
                timers[i].callback = callback;
                timers[i].remainingTime = overflowTime;
                timers[i].overflowTime = overflowTime;
                timerId = i;
                numTimers++;
                break;
            }
        }
    }

    return timerId;
}

void gpt_setOverflowTime(uint16_t overflowTime, int8_t timerId)
{
    if (timerId >= 0 && timerId < GPT_MAX_TIMERS) {
        timers[timerId].remainingTime = overflowTime;
        timers[timerId].overflowTime = overflowTime;
    }
}

void gpt_reset(int8_t timerId)
{
    if(timerId >= 0 && timerId < GPT_MAX_TIMERS)
        timers[timerId].remainingTime = timers[timerId].overflowTime;
}

void gpt_releaseTimer(int8_t timerId)
{
    if(timerId >= 0 && timerId < GPT_MAX_TIMERS) {
        timers[timerId].overflowTime = 0;
        numTimers--;
    }
}

// called every 1ms
ISR(TIMER2_COMPA_vect)
{
    uint8_t i;

    time++;

    for(i = 0; i < GPT_MAX_TIMERS; i++)
    {
        if(timers[i].overflowTime != 0) // is timer element used?
        {
            timers[i].remainingTime--;
            if(timers[i].remainingTime == 0) // time ellapsed?
            {
                timers[i].remainingTime = timers[i].overflowTime;
                (*(timers[i].callback))(); // call callback-function
            }
        }
    }
}
