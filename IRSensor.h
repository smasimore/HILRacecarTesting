/**	
 * File: IRSensor.h
 * Author: Sarah Masimore
 * Last Updated Date: 04/19/2018
 * Description: Manages init'ing and updating IR sensor ports and pins.
 */
 
 #include "Simulator.h"
 
/**
 * Init PWM channel for a sensor. Currently supports up to 6 channels. PWM on
 * TM4C supports up to 8 different PWM outputs. Channel sensor assigned to 
 * added to sensor struct.
 *
 * Note: PWM output pins come in pairs with same output. Need to be careful
 * which pin is chosen.
 *
 * Channel 0 --> PB6 --> M0PWM0
 * Channel 1 --> PC4 --> M0PWM6
 * Channel 2 --> PD0 --> M1PWM0
 * Channel 3 --> PF0 --> M1PWM4
 * Channel 4 --> PF2 --> M1PWM6
 * Channel 5 --> PE4 --> M1PWM2
 *
 */
void IRSensor_Init(struct sensor * sensor);

void IRSensor_UpdateOutput(struct sensor * sensor);
