/**	
 * File: Actuators.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing actuator ports and pins, mapping input voltage
 *							to environment variables (i.e. new velocity, direction of car).
 */

#include "Simulator.h"

/**
 * Init ports and relevant pins for velocity and direction actuators.
 */
void Actuators_Init(struct car * car);

/**
 * Gets input voltage values, maps to environment values, and stores in Car.
 */
void Actuators_UpdateVelocityAndDirection(struct car * car);
