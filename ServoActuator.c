/**  
 * File: ServoActuator.c
 * Author: Sarah Masimore
 * Last Updated Date: 05/04/2018
 * Description: Manages init'ing ADC to read servo values.
 */
 
#include "ServoActuator.h"
#include "Simulator.h"
#include "ADC.h"

#define SERVO_MAX_DUTY 125 // Duty (res = tenth of %) --> 30 deg to left
#define SERVO_MID_DUTY 75 // Duty --> 0 degrees
#define SERVO_MIN_DUTY 26 // Duty --> 30 degrees to the right
#define SERVO_MAX_DEG 30
#define SERVO_MIN_DEG 330
#define SERVO_ADC_CHANNEL 0 // PE3

extern struct live_data LiveData;

static uint16_t getDuty(void);
static uint16_t getDirectionFromDutyVal(uint16_t duty_val);

/**
 * Initializes ADC channel for reading servo duty to determine car direction.
 */
void ServoActuator_Init(void) {
  ADC_Open(SERVO_ADC_CHANNEL);
}

/**
 * Determines duty of servo motor and maps to absolute direction of car.
 */
uint16_t ServoActuator_GetDirection(void) {
  uint16_t duty = getDuty();
  return getDirectionFromDutyVal(duty);
}

/**
 * Gets duty of servo using ADC. Returns 0 - 1000 (res of .1%).
 */
static uint16_t getDuty(void) {
	uint16_t adc_val = ADC_In(SERVO_ADC_CHANNEL);
  
	// Log adc value for debugging.
	LiveData.servoAdc = adc_val;
	
  return adc_val * 1000 / 4096;
}

/**
 * Determine car's direction in degrees based on servo duty value.
 */
static uint16_t getDirectionFromDutyVal(uint16_t duty) {
	// .266 v is SERVOMAX, left
	// 
	
	
  if (duty <= SERVO_MIN_DUTY) {
    return SERVO_MIN_DUTY;
  }
  
  if (duty >= SERVO_MAX_DUTY) {
    return SERVO_MAX_DUTY;
  }

  if (duty >= SERVO_MID_DUTY) {
    return (SERVO_MAX_DUTY - duty) * SERVO_MAX_DEG / 
           (SERVO_MAX_DUTY - SERVO_MID_DUTY);
  }
  
  return SERVO_MIN_DEG + (duty - SERVO_MIN_DUTY) * (360 - SERVO_MIN_DEG) / 
         (SERVO_MAX_DUTY - SERVO_MID_DUTY);
}
