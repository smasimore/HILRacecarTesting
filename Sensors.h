/**	
 * File: Sensors.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing sensor ports and pins, mapping environment
 * 							output to voltage out on pin to racecar. Uses Ports B and C.
 */

 
#include "Simulator.h"
 
/**
 * Init ports and relevant pins.
 */
void Sensors_Init(struct car * car);

/**
 * Based on sensor value, determine and update voltage on its pin.
 */
void Sensors_UpdateVoltages(struct car * car);
