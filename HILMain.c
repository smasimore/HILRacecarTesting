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
void endSim(void);

uint32_t NumTicks = 0;
uint8_t SimComplete = 0;

int main(void){
	// Inits
	PLL_Init(Bus80MHz);
	initObjects();
	Sensors_Init(&Car);
	Actuators_Init();
	UART_Init();
	
	// Set sensors to initial state.
	Simulator_UpdateSensors(&Car);
	Sensors_UpdateVoltages(&Car);
	
	// Print initial message
	UART_OutString("Beginning test... \r\n");
	
	// Starts simulation
	initSystick();

	while (!SimComplete) {}

	SimLogger_PrintToUART();
	
	// Give UART time to finish emptying FIFO. Otherwise exits before
	// finishes printing.
  while (1) {}
}

/**
 * Used to create discrete events for sim. Every time ST interrupt is triggered
 * the sim collects actuator value, updates car's position in environment, and
 * produces next set of sensor values.
 */
void initSystick(void) {
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 80000000 / SIM_FREQ; // Clock freq (80MHz) / sim freq
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x1FFFFFFF);	// Priority 0
	NVIC_ST_CTRL_R = 7;
}

/**
 * 1) Updates actuator values. 
 * 2) Logs actuator values (set this st), sensor values (set last st), and car
 *		state (set last st). This is done so that the car state that caused the
 *		sensor and actuator values is logged.
 * 3) Update car state using new actuator values.
 * 4) Update sensor values.
 * 5) Increment NumTicks.
 */
void SysTick_Handler(void) {
	// Store car's previous x,y to later check if hit a wall.
	uint32_t prevX = Car.x;
	uint32_t prevY = Car.y;
	
	// Update actuator values (velocity and direction) and log.
	Actuators_UpdateVelocityAndDirection(&Car);
	SimLogger_LogRow(&Car, NumTicks);
		
	// Update car position based on current position, velocity, and direction.
	Simulator_MoveCar(&Car, SIM_FREQ);
	
	// Check if hit wall
	if (Simulator_HitWall(prevX, prevY, Car.x, Car.y)) {
		UART_OutString("Car crashed into wall!\r\n");
		endSim();
	}
	
	// Update sensor vals and update voltages being outputted to car.
	Simulator_UpdateSensors(&Car);
	Sensors_UpdateVoltages(&Car);
	
	NumTicks++;
	
	if (NumTicks == MAX_NUM_TICKS) {
		UART_OutString("Sim hit max num ticks: ");
		UART_OutUDec(NumTicks);
		UART_OutString("\r\n");
		endSim();
	}
}

/**
 * Disable systick and set SimComplete.
 */
void endSim(void) {
	NVIC_ST_CTRL_R = 0;
	SimComplete = 1;	
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
 * C.v = 0, V.dir = 90
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
