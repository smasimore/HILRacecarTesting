/*	
 * File: HILMain.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Controller managing simulator, actuators, sensors, and timer 
 * 							interrupt for racecar HIL testing.
*/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "Simulator.h"
#include "Sensors.h"
#include "Actuators.h"

#define NUM_SENSORS 2
#define NUM_ACTUATORS 2
#define NUM_WALLS 2

struct car Car;
struct environment Environment;
struct sensor Sensors[NUM_SENSORS];
struct actuator Actuators[NUM_ACTUATORS];
struct wall Walls[NUM_WALLS];

void initObjects(void);

int main(void){
	// Inits
	PLL_Init(Bus80MHz);
	initObjects();
	Sensors_Init(Car);
	Actuators_Init(Car);
	// SimRunLogger_Init --> inits uart
	
	// Starts timer.
	//startSimulation();
	// SimRunLogger_PrintToUART

  return 0;
}

/*
 * Current init state:
 *        _______ <-- finish line
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |   C   |
 *        -------
 */
void initObjects(void) {
	Walls[0].startX = 1000;
	Walls[0].startY = 0;
	Walls[0].endX = 1000;
	Walls[0].endY = 5000;

	Walls[1].startX = 3000;
	Walls[1].startY = 0;
	Walls[1].endX = 3000;
	Walls[1].endY = 5000;

	Environment.numWalls = NUM_WALLS;
	Environment.walls = Walls;	
	Environment.finishLineY = 5000;	
	
	Sensors[0].type = S_TEST;
	Sensors[0].pwmNum = 0;

	Sensors[1].type = S_TEST;
	Sensors[1].pwmNum = 1;

	Actuators[0].type = A_TEST;
	Actuators[0].adcChannel = 0;

	Actuators[1].type = A_TEST;
	Actuators[1].adcChannel = 1;

	Car.numSensors = NUM_SENSORS;
	Car.numActuators = NUM_ACTUATORS;
	Car.sensors = Sensors;
	Car.actuators = Actuators;

	// Start car in the middle of the two walls, no velocity, facing north.
	Car.x = 2000;
	Car.y = 0;
	Car.v = 0;
	Car.dir = 90;
}
