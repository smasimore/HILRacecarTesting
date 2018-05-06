/**  
 * File: MotorActuator.c
 * Author: Sarah Masimore
 * Last Updated Date: 04/22/2018
 * Description: Inits and reads input capture and pins to determine robot velocity.
 * NOTE: currently PB6 not reading expected values so assuming fwd at constant speed.
 */
 
#include "MotorActuator.h"
#include "ADC.h"
#include "Simulator.h"

#define MAX_VELOCITY 1114 // mm/s, measured car's velocity at max pwm
#define PB7_ADC_CHANNEL 1
#define PB6_ADC_CHANNEL 3 // channel 2, PE2 not working

extern struct live_data LiveData;

static int16_t getVelocityFromDuty(uint16_t adc_val, uint8_t forward);

/**
 * Initializes ADC and pins for reading motor voltage and h-bridge to determine
 * direction.
 */
void MotorActuator_Init(void) {
  ADC_Open(PB7_ADC_CHANNEL);
  ADC_Open(PB6_ADC_CHANNEL);
}

/**
 * Reads input capture and H-bridge values to determine velocity. Coasting 
 * state and differential motor speeds are ignored. 
 */
int32_t MotorActuator_GetVelocity(void) {
  uint16_t pb7_duty = ADC_In(PB7_ADC_CHANNEL) * 1000 / 4096; 
  uint16_t pb6_duty = ADC_In(PB6_ADC_CHANNEL) * 1000 / 4096; 

	// Print to terminal for debugging
	LiveData.motorPB7Duty = pb7_duty;
	LiveData.motorPB6Duty = pb6_duty;
	
	// Robot moving forwards.
	if (pb7_duty > pb6_duty) {
		return getVelocityFromDuty(pb6_duty, 1);
	}
	
	// Robog moving backwards.
	return getVelocityFromDuty(pb7_duty, 0);
}

/**
 * Determine velocity from duty (in tenth of percent). If going backwards, return the negative 
 * vel.
 */
static int16_t getVelocityFromDuty(uint16_t duty, uint8_t forward) {
  uint16_t vel = duty * MAX_VELOCITY / 1000;
  return forward ? (int16_t)vel : -1 * (int16_t)vel;
}
