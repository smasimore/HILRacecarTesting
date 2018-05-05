/**	
 * File: ServoActuator.h
 * Author: Sarah Masimore
 * Last Updated Date: 04/22/2018
 * Description: Manages init'ing ADC to read servo values.
 */

#include <stdint.h>

/**
 * Initializes ADC for reading servo voltage to determine car direction.
 */
void ServoActuator_Init(void);

/**
 * Reads ADC value to determine direction of steering relative to car.
 */
uint16_t ServoActuator_GetDirection(void);
