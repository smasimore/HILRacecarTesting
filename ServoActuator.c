/**	
 * File: ServoActuator.c
 * Author: Sarah Masimore
 * Last Updated Date: 04/22/2018
 * Description: Manages init'ing DUTY to read servo values.
 */
 
#include "ServoActuator.h"


#define SERVO_MAX_DUTY 104 // Duty (res = tenth of %) --> 30 deg to left
#define SERVO_MID_DUTY 80 // Duty --> 0 degrees
#define SERVO_MIN_DUTY 5 // Duty --> 30 degrees to the right
#define SERVO_MAX_DEG 30
#define SERVO_MIN_DEG 330

static uint16_t getDuty(void);
static uint16_t getDirectionFromDUTYVal(uint16_t duty_val);

/**
 * Initializes input capture for reading servo duty to determine car direction.
 */
void ServoActuator_Init(void) {
	// todo: initialize input capture
}

/**
 * Determines duty of servo motor and maps to direction of car.
 */
uint16_t ServoActuator_GetDirection(void) {
	uint16_t duty = getDuty();
	return getDirectionFromDUTYVal(duty);
}

/**
 * Gets duty of servo using input capture.
 */
static uint16_t getDuty(void) {
	// todo: get duty from input capture
	return 0;
}

/**
 * Determine car's direction in degrees based on servo DUTY value.
 */
static uint16_t getDirectionFromDUTYVal(uint16_t duty) {
	if (duty <= SERVO_MIN_DUTY) {
		return SERVO_MIN_DUTY;
  }
	
	if (duty >= SERVO_MAX_DUTY) {
		return SERVO_MAX_DUTY;
	}

	if (duty >= SERVO_MID_DUTY) {
		return (SERVO_MAX_DUTY - duty) * SERVO_MAX_DEG / 
			     (SERVO_MAX_DUTY - SERVO_MID_DUTY);
	}
	
	return SERVO_MIN_DEG + (duty - SERVO_MIN_DUTY) * (360 - SERVO_MIN_DEG) / 
				 (SERVO_MAX_DUTY - SERVO_MID_DUTY);
}
