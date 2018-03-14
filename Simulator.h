/**	
 * File: Simulator.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages simulation, including environment, car, sensors, and 
 * 							actuators. Units in centimeters.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>

/**
 * Sensor type used to map environment values (e.g. 10 cm away from wall) to
 * voltage the sensor should output to the car.
 */
enum sensor_type {
	S_TEST,
};

struct sensor {
	enum sensor_type type;
	uint8_t pwmNum;  // All sensors are on the same port.
};

/**
 * Actuator type used to map voltage input from racecar to environment values 
 * (e.g. 5 cm/s).
 */
enum actuator_type {
	A_TEST,
};

/** 
 * All actuators on same port.
 */
struct actuator {
	enum actuator_type type;
	uint8_t adcChannel;
};

/**
 * Car object is represented as one point in the environment. 
 */
struct car {
	// Position. Bottom left corner of environment is (0, 0). Units in cm.
	uint32_t x;	
	uint32_t y;
	
	// Velocity and direction. Velocity of var in cm/s. Direction of car in 
	// degrees.	If top of environment is north, dir = 0 --> car pointing east. 
	// dir = 90 --> car pointing north.
	uint32_t v; 
  uint16_t dir;
	
	// Sensors and actuators.
	uint8_t numSensors;
	uint8_t numActuators;
	struct sensor * sensors;
	struct actuator * actuators;	
};

/** 
 * Wall in environment.
 */
struct wall {
	uint32_t startX;
	uint32_t startY;
	uint32_t endX;
	uint32_t endY;	
};

struct environment {
	uint32_t finishLineY;
	uint8_t numWalls;
	struct wall * walls;
};

#endif // SIMULATOR_H
