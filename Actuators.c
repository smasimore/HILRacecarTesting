/**	
 * File: Actuators.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing actuator ports and pins, mapping input voltage
 *							to environment variables (i.e. new velocity, direction of car).
 *							Uses PE0 --> AIN3 (velocity) and PE1 --> AIN2 (direction).
 */

#include "Simulator.h"
#include "Actuators.h"

/**
 * Init ports and relevant pins for velocity and direction actuators.
 */
void Actuators_Init(void) {
	 // Init adc and channels (?)
	
	// motor
	// MOTOR_SETDUTY, other MOTOR_* funcs change mode
		// PWM0_0_CMPA_R and PWM0_1_CMPA_R (should be same.. assume same)
	// DIR: 
		// on fwd, PB7 = 0x80 PB4 = 0x10; 
		// on reverse, PB6 = 0x40, PB5 = 0x20
	
	// servo
	// do measurement with robot and map
	/*
	This is duty relative to 25000 period
	#define SERVOMAX 2600 --> 30 degrees to the left
#define SERVOMID 1975
#define SERVOMIN 1375 --> 30 degrees to the right
	*/
}

/**
 * Gets input voltage values, maps to environment values, and stores in Car.
 */
void Actuators_UpdateVelocityAndDirection(struct car * car) {
	// Get pin values
	// Map based on actuator type
	// Store to car
}
