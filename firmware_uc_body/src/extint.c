/**
 * @file extint.c
 * @author Denise Ratasich
 * @date 14.05.2016
 *
 * @brief External interrupts module implementation.
 */

#include <avr/interrupt.h>
#include "extint.h"
#include "io.h" // port, pins definition

/** Number of external interrupts. */
#define EXTINT_NUM      8

/** External interrupt element, collects information to interrupt requests. */
typedef struct {
  void (*callback)(void);
  uint8_t used;
} ExtintStruct_t;

/** External interrupt elements. */
static ExtintStruct_t extints[EXTINT_NUM];

void extint_init()
{
  uint8_t i;

  // mark external interrupt elements as unused
  for (i = 0; i < EXTINT_NUM; i++)
    extints[i].used = 0;
}

int8_t extint_requestInt(int8_t no, uint8_t trigger, void (*callback)(void))
{
  // valid no?
  if (no < 0  ||  no > 7)
    return -1;
  // valid trigger?
  if (trigger > 3)
    return -1;

  // check if external interrupt is unused
  if (extints[no].used)
    return -1;

  // set corresponding external interrupt element
  extints[no].used = 1;
  extints[no].callback = callback;

  // pin and interrupt settings
  switch (no) {
  case 0:
  case 1:
  case 2:
  case 3:
    EXTINTA_PORT |= (1 << no); // activate pullup
    EXTINTA_DDR &= ~(1 << no); // pin is input
    EICRA &= ~(0x03 << (no*2)); // clear bits for sense control
    EICRA |= (trigger << (no*2)); // set sense control
    break;
  case 4:
  case 5:
  case 6:
  case 7:
    EXTINTB_PORT |= (1 << no); // activate pullup
    EXTINTB_DDR &= ~(1 << no); // pin is input
    EICRB &= ~(0x03 << ((no-4)*2)); // clear bits for sense control
    EICRB |= (trigger << ((no-4)*2)); // set sense control
    break;
  default:
    return -1; // should never reach this line
  }

  EIFR &= ~(1 << no); // clear pending interrupts on this pin first
  EIMSK |= (1 << no); // enable interrupt

  sei();

  return no;
}

void extint_releaseInt(int8_t no)
{
  // valid no?
  if (no < 0  ||  no > 7)
    return;

  // deactivate external interrupt
  EIMSK &= ~(1 << no); // disable interrupt
  EIFR &= ~(1 << no); // clear pending flags
  extints[no].used = 0; // mark as unused
}

//
// external interrupt service routines
//

ISR(INT0_vect)
{
  if (extints[0].used)
    (*(extints[0].callback))(); // call callback-function
}

ISR(INT1_vect)
{
  if (extints[1].used)
    (*(extints[1].callback))(); // call callback-function
}

ISR(INT2_vect)
{
  if (extints[2].used)
    (*(extints[2].callback))(); // call callback-function
}

ISR(INT3_vect)
{
  if (extints[3].used)
    (*(extints[3].callback))(); // call callback-function
}

ISR(INT4_vect)
{
  if (extints[4].used)
    (*(extints[4].callback))(); // call callback-function
}

ISR(INT5_vect)
{
  if (extints[5].used)
    (*(extints[5].callback))(); // call callback-function
}

ISR(INT6_vect)
{
  if (extints[6].used)
    (*(extints[6].callback))(); // call callback-function
}

ISR(INT7_vect)
{
  if (extints[7].used)
    (*(extints[7].callback))(); // call callback-function
}
