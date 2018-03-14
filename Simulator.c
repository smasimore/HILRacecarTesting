/*	
 * File: Simulator.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages simulation, including environment, car, sensors, and 
 * 							actuators. Units in centimeters.
*/

#include "Simulator.h"

/**
 * Based on velocity, direction, and sim_freq update car's position.
 * Don't need to worry about walls since that will be checkekd in 
 * Simulator_HitWall.
 */
void Simulator_MoveCar(struct car * car, uint32_t simFreq) {
	
	
}

/**
 * Based on previous and next location, determine if hit wall.
 */
uint8_t Simulator_HitWall(uint32_t prevX, uint32_t prevY, 
													uint32_t nextX, uint32_t nextY) {
	

	return 0;
}

/**
 * Update sensor values relative to environment. For each sensor, based on 
 * sensor's direction, car's direction, and car's position determine distance
 * to closest wall.
 */
void Simulator_UpdateSensors(struct car * car) {
	// based on where
}
