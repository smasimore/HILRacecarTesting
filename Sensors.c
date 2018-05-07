/**  
 * File: Sensors.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing and updating sensors.
 */

#include "Simulator.h"
#include "Sensors.h"
#include "tm4c123gh6pm.h"
#include "terminal.h"
#include "IRSensor.h"
#include "USSensor.h"

uint8_t SensorsInitialized = 0;

/**
 * Init sensor's required output pins.
 */
void Sensors_Init(struct car * car) {
  uint16_t numSensors = car->numSensors;
  int i;
  struct sensor * sensor;
  
  // Based on sensor numbers, init relevant pins.
  for (i = 0; i < numSensors; i++) {
    sensor = &car->sensors[i];
    switch (sensor->type) {
      case S_US:
        USSensor_Init(sensor);
        break;
      case S_IR:
        IRSensor_Init(sensor);
        break;
      default:
        terminal_printString("Error: Unsupported sensor type\r\n");
        continue;
    }
  }
}

/**
 * Based on sensor value, determine and update value sent to robot.
 */
void Sensors_UpdateOutput(struct car * car) {
  int i;
  struct sensor * sensor;  
  
  for (i = 0; i < car->numSensors; i++) {
    sensor = &car->sensors[i];
    switch (sensor->type) {
      case S_IR:
        IRSensor_UpdateOutput(sensor);
        break;
      case S_US:
        USSensor_UpdateOutput(sensor);
        break;
      default:
        terminal_printString("Error: Unsupported sensor type\r\n");
        continue;  
    }
  }
}
