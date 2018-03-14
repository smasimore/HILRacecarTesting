/**	
 * File: Actuators.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing actuator ports and pins, mapping input voltage
 *							to environment variables (i.e. new velocity, direction of car).
 */

#include "Simulator.h"
#include "Actuators.h"

/**
 * Init ports and relevant pins for velocity and direction actuators.
 */
void Actuators_Init(struct car * car) {
	 // Init adc and channels (?)
}

/**
 * Gets input voltage values, maps to environment values, and stores in Car.
 */
void Actuators_UpdateVelocityAndDirection(struct car * car) {
	// Get pin values
	// Map based on actuator type
	// Store to car
}
