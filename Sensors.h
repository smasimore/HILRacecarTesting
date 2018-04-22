/**	
 * File: Sensors.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing and updating sensors.
 */
 
#include "Simulator.h"
 
/**
 * Init ports and relevant pins.
 */
void Sensors_Init(struct car * car);

/**
 * Based on sensor value, determine and update voltage on its pin.
 */
void Sensors_UpdateOutput(struct car * car);
