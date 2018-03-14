/**	
 * File: Sensors.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing sensor ports and pins, mapping environment
 * 							output to voltage out on pin to racecar. Uses Ports B and C.
 */

#include "Simulator.h"
#include "Sensors.h"

uint8_t SensorsInitialized = 0;

/**
 * Init ports and relevant pins. Currently supports up to 6 sensors.
 *
 * S0 --> PB4 --> M0PWM2
 * S1 --> PB5 --> M0PWM3
 * S2 --> PB6 --> M0PWM0
 * S3 --> PB7 --> M0PWM1
 * S4 --> PB7 --> M0PWM1
 * S5 --> PC4 --> M0PWM6
 */
void Sensors_Init(struct car * car) {
	uint16_t numSensors = car->numSensors;
	
	// Initialize port B
	
	// If more than 5 sensors, initialize port C.
	
	// Based on pwm numbers, init X number of ports and pins.
	//for (int i = 0; i < numSensors; i++) {
		// Initialize pwm pin based on sensor number.
	//}
}

/**
 * Based on sensor value, determine and update voltage on its pin.
 */
void Sensors_UpdateVoltages(struct car * car) {
	
}
