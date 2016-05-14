/**
 * @file steppermotor.c
 * @author Denise Ratasich
 * @date 06.05.2016
 *
 * @brief Implementation of stepper motor driver.
 * 
 * Driver for a 4-phase unipolar stepper motor (8 wires, 4 connected to
 * VCC). Coils are named A1, A2, B1, B2 (see io.h). These pins should be
 * connected to transistors, i.e., '1' should pull the end of a coil to ground.
 *
 * https://learn.adafruit.com/all-about-stepper-motors/types-of-steppers
 */

#include "steppermotor.h"
#include "io.h"	// port, pins definition

/** Number of steps in full step mode. */
#define NUM_STEPS_FULL	4
/** Number of steps in half step mode. */
#define NUM_STEPS_HALF	8

/** Counts steps taken from the beginning. */
static int32_t csteps = 0;

/** Current step in half/full step mode. */
static volatile uint8_t step = 0;

/** Exciting sequence of coils in full step mode. */
static uint8_t steps_full[NUM_STEPS_FULL] = {
  (1<<STEPPERMOTOR_A1 | 1<<STEPPERMOTOR_A2),
  (1<<STEPPERMOTOR_A2 | 1<<STEPPERMOTOR_B1),
  (1<<STEPPERMOTOR_B1 | 1<<STEPPERMOTOR_B2),
  (1<<STEPPERMOTOR_B2 | 1<<STEPPERMOTOR_A1)
};

/** Exciting sequence of coils in half step mode. */
static uint8_t steps_half[NUM_STEPS_HALF] = {
  (1<<STEPPERMOTOR_A1),
  (1<<STEPPERMOTOR_A1 | 1<<STEPPERMOTOR_A2),
  (1<<STEPPERMOTOR_A2),
  (1<<STEPPERMOTOR_A2 | 1<<STEPPERMOTOR_B1),
  (1<<STEPPERMOTOR_B1),
  (1<<STEPPERMOTOR_B1 | 1<<STEPPERMOTOR_B2),
  (1<<STEPPERMOTOR_B2),
  (1<<STEPPERMOTOR_B2 | 1<<STEPPERMOTOR_A1)
};

static uint8_t *steps = steps_full;
static uint8_t num_steps = NUM_STEPS_FULL;

void steppermotor_init(enum steppermotor_mode mode)
{
  // init pins
  // value
  CLEAR_BIT(STEPPERMOTOR_PORT, STEPPERMOTOR_A1);
  CLEAR_BIT(STEPPERMOTOR_PORT, STEPPERMOTOR_A2);
  CLEAR_BIT(STEPPERMOTOR_PORT, STEPPERMOTOR_B1);
  CLEAR_BIT(STEPPERMOTOR_PORT, STEPPERMOTOR_B2);
  // direction
  SET_BIT(STEPPERMOTOR_DDR, STEPPERMOTOR_A1);
  SET_BIT(STEPPERMOTOR_DDR, STEPPERMOTOR_A2);
  SET_BIT(STEPPERMOTOR_DDR, STEPPERMOTOR_B1);
  SET_BIT(STEPPERMOTOR_DDR, STEPPERMOTOR_B2);

  // init variables
  step = 0; // goes from 0 to num_steps

  if (mode == HALF) {
    steps = steps_half;
    num_steps = NUM_STEPS_HALF;
  } else {
    steps = steps_full;
    num_steps = NUM_STEPS_FULL;
  }
}

void steppermotor_step(enum steppermotor_direction dir)
{
  // increase or decrease step
  if (dir == RIGHT) {
    step = (step + 1) % num_steps;
    csteps++;
  } else if (dir == LEFT) {
    if (step == 0)
      step = num_steps - 1;
    else
      step--;
    csteps--;
  }

  // apply step
  steppermotor_on();
}

void steppermotor_on(void)
{
  // read port
  uint8_t cur = STEPPERMOTOR_PORT;
  // clear pins
  cur &= ~(1<<STEPPERMOTOR_A1 | 1<<STEPPERMOTOR_A2 | 1<<STEPPERMOTOR_B1 | 1<<STEPPERMOTOR_B2);
  // set pins
  cur |= steps[step];

  // assign to port
  STEPPERMOTOR_PORT = cur;
}

void steppermotor_off(void)
{
  STEPPERMOTOR_PORT &= ~(1<<STEPPERMOTOR_A1 | 1<<STEPPERMOTOR_A2 | 1<<STEPPERMOTOR_B1 | 1<<STEPPERMOTOR_B2);
}

int32_t steppermotor_position(void)
{
  return csteps;
}
