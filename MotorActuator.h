/**	
 * File: MotorActuator.h
 * Author: Sarah Masimore
 * Last Updated Date: 04/22/2018
 * Description: Manages init'ing ADC and pins to read motor values.
 */
 
 #include <stdint.h>

/**
 * Initializes ADC and pins for reading motor voltage and h-bridge to determine
 * direction.
 */
void MotorActuator_Init(void);

/**
 * Reads ADC and H-bridge values to determine velocity. 
 */
int32_t MotorActuator_GetVelocity(void);
