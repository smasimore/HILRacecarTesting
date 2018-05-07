/**  
 * File: Actuators.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing actuator ports and pins, mapping input voltage
 *              to environment variables (i.e. new velocity, direction of car).
 *              Uses PE0 --> AIN3 (velocity) and PE1 --> AIN2 (direction).
 */

#include "Simulator.h"
#include "Actuators.h"
#include "MotorActuator.h"
#include "ServoActuator.h"

// Uncomment to mock actuator values (e.g. for testing sim)
//#define MOCK_ACTUATORS

#ifdef MOCK_ACTUATORS
  extern uint32_t NumSimTicks;
  // Example of completion
  //uint32_t TestDir[34] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90};  
  // Example of crash
  uint32_t TestDir[34] = {90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  

#endif

/**
 * Init ports and relevant pins for velocity and direction actuators.
 */
void Actuators_Init(void) {
  MotorActuator_Init();
  ServoActuator_Init();
}

/**
 * Gets input voltage values, maps to environment values, and stores in Car.
 */
void Actuators_UpdateVelocityAndDirection(struct car * car) {
#ifdef MOCK_ACTUATORS
  car->dir = TestDir[NumSimTicks];
#else
  uint16_t dir;
  car->vel = MotorActuator_GetVelocity();
  dir = car->dir + ServoActuator_GetDirection();
  car->dir = dir > 360 ? dir - 360 : dir;
#endif
}
