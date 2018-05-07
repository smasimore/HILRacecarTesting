/**  
 * File: ServoActuator.c
 * Author: Sarah Masimore
 * Last Updated Date: 05/04/2018
 * Description: Manages init'ing ADC to read servo values.
 */
 
#include "ServoActuator.h"
#include "Simulator.h"
#include "ADC.h"

// Calibration constants for actuator pins directly into test controller w/
// hw actuators running in parallel. SERVO_MAX_DUTY and SERVO_MIN_DUTY require
// tuning in any environments (e.g. diff power, time robot powered).
// NOTE: precision of servo signal poor, so binning into L, M, R. 
#define SERVO_DUTY_L 40 // Duty (res = tenth of %) --> 30 deg to left
#define SERVO_DUTY_R 0 // Duty --> 30 degrees to the right
#define SERVO_DEG_L 30 
#define SERVO_DEG_M 0
#define SERVO_DEG_R -30
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
  
  // Log duty value for debugging.
  LiveData.servoDuty = duty;
  
  return getDirectionFromDutyVal(duty);
}

/**
 * Gets duty of servo using ADC. Returns 0 - 1000 (res of .1%).
 */
static uint16_t getDuty(void) {
  uint16_t adc_val = ADC_In(SERVO_ADC_CHANNEL);
  
  return adc_val * 1000 / 4096;
}

/**
 * Determine steering direction in degrees based on servo duty value. 
 * Precision of servo input highly variable so binning into L, R, and M.
 */
static uint16_t getDirectionFromDutyVal(uint16_t duty) {
  if (duty <= SERVO_DUTY_R) {
    return 360 + SERVO_DEG_R;
  }
  
  if (duty >= SERVO_DUTY_L) {
    return SERVO_DEG_L;
  }
  
  return SERVO_DEG_M;
}
