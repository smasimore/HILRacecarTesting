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

#define MOTORS_IN_PARALLEL_MODE

#define MAX_VELOCITY 1114 // mm/s, measured car's velocity at max pwm
#define PB7_ADC_CHANNEL 1
#define PB6_ADC_CHANNEL 3 // channel 2, PE1 not working

extern struct live_data LiveData;

static int32_t getVelocityFromDuty(uint32_t adc_val, uint8_t forward);

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
  int32_t pb7_duty = ADC_In(PB7_ADC_CHANNEL) * 1000 / 4096; 
  int32_t pb6_duty = ADC_In(PB6_ADC_CHANNEL) * 1000 / 4096; 

  // Print to terminal for debugging
  LiveData.motorPB7Duty = pb7_duty;
  LiveData.motorPB6Duty = pb6_duty;
	
#ifdef MOTORS_IN_PARALLEL_MODE
  // Robot moving forwards.
  if (pb7_duty >= pb6_duty) {
    // When running hw actuators in parallel, pin is pulled down. Set as full speed.
    // When not running them in parallel use PB6 duty.
    return getVelocityFromDuty(999, 1);
  }
  
  // Robog moving backwards.
  // When running hw actuators in parallel, pin is pulled down. Set as 70% speed.
  // When not running them in parallel use PB7 duty.
  return getVelocityFromDuty(700, 0);
#endif
  
  // Robot moving forwards.
  if (pb7_duty >= pb6_duty) {
    return getVelocityFromDuty(pb6_duty, 1);
  }
  
  // Robot moving backwards.
  return getVelocityFromDuty(pb7_duty, 0);
}

/**
 * Determine velocity from duty (in tenth of percent). If going backwards, return the negative 
 * vel.
 */
static int32_t getVelocityFromDuty(uint32_t duty, uint8_t forward) {
  int32_t vel = duty * MAX_VELOCITY / 1000;
  
  return forward ? vel : -1 * vel;
}
