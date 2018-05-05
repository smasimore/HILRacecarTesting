/**  
 * File: MotorActuator.c
 * Author: Sarah Masimore
 * Last Updated Date: 04/22/2018
 * Description: Inits and reads input capture and pins to determine robot velocity.
 */
 
#include "MotorActuator.h"

#define MAX_VELOCITY 1114 // mm/s, measured car's velocity at max pwm

static uint16_t getDuty(void);
static uint8_t getDirection(void);
static int16_t getVelocityFromDuty(uint16_t adc_val, uint8_t forward);

/**
 * Initializes ADC and pins for reading motor voltage and h-bridge to determine
 * direction.
 */
void MotorActuator_Init(void) {
// Version 6 hardware (use program main)
// to go forward on right motor
// PB7 A+  regular GPIO level high (1)
// PB6 A-  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// to go backward on right motor
// PB7 A+  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// PB6 A-  regular GPIO level high (1)
// coast on right motor (fast decay)
// PB7 A+  regular GPIO level low (0)
// PB6 A-  regular GPIO level low (0)
// to go forward on left motor
// PB5 B+  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// PB4 B-  regular GPIO level high (1) 
// to go backward on left motor
// PB5 B+  regular GPIO level high (1)
// PB4 B-  PWM 100 Hz, PWM negative logic (e.g., 10% duty cycle is 90% power)
// coast on left motor (fast decay)
// PB5 B+  regular GPIO level low (0)
// PB4 B-  regular GPIO level low (0)
	
	/* thoughts:
	ignore coasting, only time robot uses is after 180s
	ignore differential motor speeds
	
	PB7: 
	  - when fwd, 3.1 even when stopped
		- when bwd, .9
	
	PB6
	  - when fwd .5
		- when bwd 3.1
		
	if PB7 high, fwd, PB6 is pwm
	if PB6 high, bwd, PB7 is pwm
	
	set up 2 adc channels
	
	monitor one motor (right)	
	*/
}

/**
 * Reads input capture and H-bridge values to determine velocity. 
 */
int32_t MotorActuator_GetVelocity(void) {
  uint16_t duty = getDuty();
  uint8_t direction = getDirection();
  return getVelocityFromDuty(duty, direction);
}

/**
 * Get duty and period for motor PWM. Assumes both motors operating at same 
 * rps.
 */
static uint16_t getDuty(void) {
  // todo: input capture
  return 0;
}

/**
 * Determines if car is going forwards or backwards based on H-Bridge values.
 * Returns 1 for forwards, 0 for backwards.
 */
static uint8_t getDirection(void) {
  // todo: read H-bridge inputs
  return 1;
}

/**
 * Determine velocity from duty (in tenth of percent). If going backwards, return the negative 
 * vel.
 */
static int16_t getVelocityFromDuty(uint16_t duty, uint8_t forward) {
  uint16_t vel = duty * MAX_VELOCITY / 1000;
  return forward ? (int16_t)vel : -1 * (int16_t)vel;
}
