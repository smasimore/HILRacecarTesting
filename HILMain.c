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
#include "UART.h"
#include "SimLogger.h"
#include "OS.h"
#include "terminal.h"

#define CLOCK_FREQ 80000000 // 80 Mhz
#define SIM_FREQ 10 // Hz
#define MAX_NUM_TICKS 100 // Keep this in sync with SimLogger MAX_ROWS

#define NUM_SENSORS 6
#define NUM_WALLS 2

struct car Car;
struct environment Environment;
struct sensor Sensors[NUM_SENSORS];
struct wall Walls[NUM_WALLS];

void initObjects(void);
void initSystick(void);
void simThread(void);
void foregroundThread(void);

uint32_t NumSimTicks = 0;
uint8_t SimComplete = 0; // Note: This is shared by bg and fg threads, so there
                         //       is minor race (log print will potentially be 
												 //       delayed by one loop in fg thread).

int main(void){
  OS_Init();
	
	// Simulator inits
	terminal_init();
	initObjects();
	Sensors_Init(&Car);
	Actuators_Init();
	
	// Set sensors to initial state.
	Simulator_UpdateSensors(&Car);
	Sensors_UpdateOutput(&Car);
	
	// Background sim thread
  OS_AddPeriodicThread(&simThread, CLOCK_FREQ / SIM_FREQ, 3); // 10 hz, lower priority than UART
	
	// Foreground user communication thread
	OS_AddThread(&foregroundThread,128,2); 

	OS_Launch(TIME_2MS);
}

/**
 * Sim background thread. Creates discrete events for sim. Every time timer 
 * interrupt is triggered the sim collects actuator value, updates car's 
 * position in environment, and produces next set of sensor values.
 *
 * 1) Updates actuator values. 
 * 2) Logs actuator values (set this st), sensor values (set last st), and car
 *		state (set last st). This is done so that the car state that caused the
 *		sensor and actuator values is logged.
 * 3) Update car state using new actuator values.
 * 4) Update sensor values.
 * 5) Increment NumSimTicks.
 */
void simThread(void) {
	// Store car's previous x,y to later check if hit a wall.
	uint32_t prevX = Car.x;
	uint32_t prevY = Car.y;
	
	// Update actuator values (velocity and direction) and log.
	Actuators_UpdateVelocityAndDirection(&Car);
	SimLogger_LogRow(&Car, NumSimTicks);
		
	// Update car position based on current position, velocity, and direction.
	Simulator_MoveCar(&Car, SIM_FREQ);
	
	// Check if hit wall
	if (Simulator_HitWall(prevX, prevY, Car.x, Car.y)) {
		terminal_printString("Car crashed into wall!\r\n");
		SimLogger_PrintToTerminal();
		SimComplete = 1;
		OS_RemovePeriodicThread();
	}
	
	// Update sensor vals and update voltages being outputted to car.
	Simulator_UpdateSensors(&Car);
	Sensors_UpdateOutput(&Car);
	
	NumSimTicks++;
	
	if (NumSimTicks == MAX_NUM_TICKS) {
		SimComplete = 1;
		OS_RemovePeriodicThread();
	}
}

/**
 * Terminal foreground thread.
 */
void foregroundThread(void) {
	while(1) {
		if (SimComplete) {
			terminal_printString("Sim hit max num ticks: ");
			terminal_printValueDec(NumSimTicks);
			terminal_printString("\r\n\r\n");
			SimLogger_PrintToTerminal();
			terminal_printString("\r\nTest complete.\r\n\r\n");

			SimComplete = 0;
		}
	}
}

/**
 * Init state:
 *        _______ <-- finish line
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |   C   |
 *        -------
 *
 * C.v = 0, C.dir = 90
 * 
 */
void initObjects(void) { // initObjectsSimple
	int i;
	
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
	
	for (i = 0; i < NUM_SENSORS; i++) {
		Sensors[i].type = S_TEST;
		Sensors[i].val = 0;
		Sensors[i].dir = 0;
	}
	
	Car.numSensors = NUM_SENSORS;
	Car.sensors = Sensors;

	// Start car in the middle of the two walls, no velocity, facing north.
	Car.x = 2000;
	Car.y = 0;
	Car.v = 0;
	Car.dir = 90;
}
