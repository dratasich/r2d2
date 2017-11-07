/**
 * @file motor.c
 * @author Denise Ratasich
 * @date 16.05.2016
 *
 * @brief Implementation of DC motor driver.
 * 
 * Driver for a DC motor (2 inputs + 1 enable). Pins are connected to a L298N
 * motor driver IC.
 */

#include "motor.h"
#include "pwm.h"
#include "io.h"	// port, pins definition
static int16_t speed; // -PWM_TOP .. PWM_TOP

void motor_init(void)
{
  // init pins
  // value
  SET_BIT(MOTOR_PORT, MOTOR_IN1);
  CLEAR_BIT(MOTOR_PORT, MOTOR_IN2);
  CLEAR_BIT(MOTOR_PORT, MOTOR_ENA);
  // direction
  SET_BIT(MOTOR_DDR, MOTOR_IN1);
  SET_BIT(MOTOR_DDR, MOTOR_IN2);
  SET_BIT(MOTOR_DDR, MOTOR_ENA);

  pwm_init(PWM_OC1A);

  speed = 0;
  pwm_set(PWM_OC1A, 0);
}

void motor_inc(uint16_t step)
{
  int8_t changeDir = 0;

  if (speed < 0  &&  speed + step > 0)
    changeDir = 1;

  // motor direction if sign of speed changes
  if (changeDir) {
    // TODO: set pins on port in 1 assignment
    SET_BIT(MOTOR_PORT, MOTOR_IN1);
    CLEAR_BIT(MOTOR_PORT, MOTOR_IN2);
  }

  // increase, what possible
  if (speed <= ((int16_t)(PWM_TOP - step)))
    speed += step;
  else
    speed = PWM_TOP;

  // adapt speed
  if (speed < 0)
    pwm_set(PWM_OC1A, -speed);
  else
    pwm_set(PWM_OC1A, speed);
}

void motor_dec(uint16_t step)
{
  int8_t changeDir = 0;

  if (speed >= 0  &&  speed - step < 0)
    changeDir = 1;

  // motor direction if sign of speed changes
  if (changeDir) {
    // TODO: set pins on port in 1 assignment
    CLEAR_BIT(MOTOR_PORT, MOTOR_IN1);
    SET_BIT(MOTOR_PORT, MOTOR_IN2);
  }

  // decrease, what possible
  if (speed >= ((int16_t)(-PWM_TOP + step)))
    speed -= (int16_t) step;
  else
    speed = -PWM_TOP;

  // adapt speed
  if (speed < 0)
    pwm_set(PWM_OC1A, -speed);
  else
    pwm_set(PWM_OC1A, speed);
}

int16_t motor_getSpeed(void)
{
  return speed;
}

int8_t motor_getDirection(void)
{
  if (speed > 0)
    return 1;

  if (speed < 0)
    return -1;

  return 0;
}
