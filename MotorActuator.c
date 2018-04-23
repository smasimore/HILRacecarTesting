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
	// todo: input capture init and pin inputs for H-bridge
	
	// MOTOR_SETDUTY, other MOTOR_* funcs change mode
		// PWM0_0_CMPA_R and PWM0_1_CMPA_R (should be same.. assume same)
	// DIR: 
		// on fwd, PB7 = 0x80 PB4 = 0x10; 
		// on reverse, PB6 = 0x40, PB5 = 0x20
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
