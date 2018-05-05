/**	
 * File: Simulator.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages simulation, including environment, car, sensors, and 
 * 							actuators. Units in millimeters.
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>

#define CLOCK_FREQ 80000000 // 80 Mhz
#define SIM_FREQ 1 // Hz, how often state transitions occur
#define MS_PER_SIM_TICK 100
#define MAX_NUM_TICKS 100 // Keep this in sync with SimLogger MAX_ROWS

#define MAX_SENSOR_LINE_OF_SIGHT 10000 // 10 meters
#define MAX_U32INT 1U << 31
#define MIN_32INT 1 << 31
#define MAX_32INT ~(1 << 31)



// STRUCTS


/**
 * Sensor type used to map environment values (e.g. 10 mm away from wall) to
 * voltage the sensor should output to the car.
 */
enum sensor_type {
	S_US,
	S_IR,
};

struct sensor {
	enum sensor_type type; // Ping, IR, etc. Influences mapping from val to voltage
  uint32_t dir; // direction in angles, 0 - 360
	uint32_t val; // distance from nearest wall in path of sensor
	uint8_t channel; // hardware channel output is on, set by Sensors_Init
};

/**
 * Car object is represented as one point in the environment. 
 */
struct car {
	// Position. Bottom left corner of environment is (0, 0). Units in mm.
	uint32_t x;	
	uint32_t y;
	
	// Velocity and direction. If top of environment is north, dir = 0 --> car 
	// pointing east, dir = 90 --> car pointing north.
	int32_t vel; // mm/s, negative if going backwards
  uint32_t dir; // direction relative to environment bottom boundary, degrees, 
	              // 0 - 360
	
	// Sensors and actuators.
	uint8_t numSensors;
	struct sensor * sensors;
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

/**
 * Environment (track).
 */
struct environment {
	uint32_t finishLineY;
	uint8_t numWalls;
	struct wall * walls;
};

/**
 * Struct to hold data for printing while sim is running.
 */
struct live_data {
	uint32_t time;
	uint32_t x;	
	uint32_t y;
	int32_t vel;
  uint32_t dir;
	uint16_t servoAdc;
};

// FUNCTIONS

/**
 * Based on velocity, direction, and sim_freq update car's position.
 */
void Simulator_MoveCar(struct car * car, uint32_t timePassedMs);

/**
 * Based on previous and next location, determine if hit wall.
 */
uint8_t Simulator_HitWall(struct environment * env, uint32_t prevX, 
	                        uint32_t prevY, uint32_t nextX, uint32_t nextY);

/**
 * Update sensor values relative to environment. For each sensor, based on 
 * sensor's direction, car's direction, and car's position determine distance
 * to closest wall.
 */
void Simulator_UpdateSensors(struct car * car, struct environment * env);

#endif // SIMULATOR_H
