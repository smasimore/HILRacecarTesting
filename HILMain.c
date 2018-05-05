/**
 * File: HILMain.c
 * Author: Sarah Masimore
 * Last Updated Date: 04/22/2018
 * Description: Controller managing simulator, actuators, sensors, and timer 
 *              interrupt for racecar HIL testing.
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
#include "FIFO.h"

#define NUM_SENSORS 7
#define NUM_WALLS 6

struct car Car;
struct environment Environment;
struct sensor Sensors[NUM_SENSORS];
struct wall Walls[NUM_WALLS];

uint32_t NumSimTicks = 0;
uint8_t SimComplete = 0;
struct live_data LiveData;

static void initObjects(void);
static void addSimFGThread(void);
static void simThread(void);
static void dataOut(void);
static void endSim(char * message);

// Fifo for storing live data to be printed to terminal in dataOut thread.
AddIndexFifo(LiveData, 1, struct live_data, 1, 0);

int main(void){
  OS_Init();
  
  // Simulator inits
  terminal_init();
  initObjects();
  Sensors_Init(&Car);
  Actuators_Init();
	LiveDataFifo_Init();
  
  // Set sensors to initial state.
  Simulator_UpdateSensors(&Car, &Environment);
  Sensors_UpdateOutput(&Car);
  
  // Background sim thread
  OS_AddPeriodicThread(&addSimFGThread, CLOCK_FREQ / SIM_FREQ, 2); // 10 hz
  
  // Foreground data output thread, lowest priority.
  OS_AddThread(&dataOut, 128, 9); 
  
  terminal_printString("\r\n Starting test...\r\n");
  OS_Launch(TIME_2MS);
}

/**
 * Periodic background thread that adds the simThread to the foreground. 
 * simThread should not be run in an ISR because it a) is relatively long and 
 * b) if it fills up the UART buffer, it will hang because the UART interrupt 
 * won't be able to run to clear the buffer (simThread will hang).
 */
static void addSimFGThread(void) {
  OS_AddThread(&simThread, 128, 1); // 10 hz, higher priority than terminal
}

/**
 * Sim background thread. Creates discrete events for sim. Every time timer 
 * interrupt is triggered the sim collects actuator value, updates car's 
 * position in environment, and produces next set of sensor values.
 *
 * 1) Updates actuator values. 
 * 2) Logs actuator values (set this st), sensor values (set last st), and car
 *    state (set last st). This is done so that the car state that caused the
 *    sensor and actuator values is logged.
 * 3) Update car state using new actuator values.
 * 4) Update sensor values.
 * 5) Increment NumSimTicks.
 */
static void simThread(void) {	
  // Store car's previous x,y to later check if hit a wall.
  uint32_t prevX = Car.x;
  uint32_t prevY = Car.y;
  
  // Update actuator values (velocity and direction) and log.
  Actuators_UpdateVelocityAndDirection(&Car);
  
  // Log event after velocity and dir have been updated but before
  // car location has been updated.
  SimLogger_LogRow(&Car, NumSimTicks);
	
	// Add data to FIFO to be printed in dataOut low priority thread
	LiveData.time = OS_Time();
	LiveData.x = Car.x;
	LiveData.y = Car.y;
	LiveData.vel = Car.vel;
	LiveData.dir = Car.dir;
	LiveDataFifo_Put(LiveData);
    
  // Update car position based on current position, velocity, and direction.
  Simulator_MoveCar(&Car, MS_PER_SIM_TICK);
  
  // Check if hit wall.
  if (Simulator_HitWall(&Environment, prevX, prevY, Car.x, Car.y)) {
    endSim("Car crashed into wall!");
  }
  
  // If got to this point and car's y position is higher than finish line,
  // race is over.
  if (Car.y >= Environment.finishLineY) {
    endSim("Car completed race!");
  }    
  
  // Update sensor vals and update voltages being outputted to car.
  Simulator_UpdateSensors(&Car, &Environment);
  Sensors_UpdateOutput(&Car);
	
  NumSimTicks++;
  
  if (NumSimTicks == MAX_NUM_TICKS) {
    endSim("Sim hit max num ticks");
  }
  
  OS_Kill();
}

/**
 * Whenever there's data to print and nothing more important to run, prints to
 * terminal. Foreground thread.
 */
static void dataOut(void) {
	struct live_data live_data;
  while(1) {
	  if (LiveDataFifo_Get(&live_data) && !SimComplete) {
			terminal_printString("t: ");
			terminal_printValueDec(live_data.time / 80000000);
			terminal_printString(" | x: ");
			terminal_printValueDec(live_data.x);
			terminal_printString(" | y: ");
			terminal_printValueDec(live_data.y);
			terminal_printString(" | vel: ");
			terminal_printValueDec(live_data.vel);
			terminal_printString(" | dir: ");
			terminal_printValueDec(live_data.dir);
			terminal_printString(" | servo adc: ");
			terminal_printValueDec(live_data.servoAdc);
			terminal_printString("\r\n");
			terminal_printString("\r\n");
		}
	}
}


/**
 * Initialize environment and car.
 * 
 */
static void initObjects(void) { // initObjectsSimple
  Walls[0].startX = 1000;
  Walls[0].startY = 0;
  Walls[0].endX = 1000;
  Walls[0].endY = 1500;

  Walls[1].startX = 2000;
  Walls[1].startY = 0;
  Walls[1].endX = 2000;
  Walls[1].endY = 500;  

  Walls[2].startX = 1000;
  Walls[2].startY = 1500;
  Walls[2].endX = 2500;
  Walls[2].endY = 1500;

  Walls[3].startX = 2000;
  Walls[3].startY = 500;
  Walls[3].endX = 3000;
  Walls[3].endY = 500;  
  
  Walls[4].startX = 2500;
  Walls[4].startY = 1500;
  Walls[4].endX = 2500;
  Walls[4].endY = 5000;

  Walls[5].startX = 3000;
  Walls[5].startY = 500;
  Walls[5].endX = 3000;
  Walls[5].endY = 5000;  
  
  Environment.numWalls = NUM_WALLS;
  Environment.walls = Walls;  
  Environment.finishLineY = 2000;  
  
  // Front center
  Sensors[0].type = S_US;
  Sensors[0].val = 0;
  Sensors[0].dir = 0;

  // Mid left
  Sensors[1].type = S_US;
  Sensors[1].val = 0;
  Sensors[1].dir = 90;

  // Mid right
  Sensors[2].type = S_US;
  Sensors[2].val = 0;
  Sensors[2].dir = 270;

  // Front far left
  Sensors[3].type = S_IR;
  Sensors[3].val = 0;
  Sensors[3].dir = 90;

  // Front far right
  Sensors[4].type = S_IR;
  Sensors[4].val = 0;
  Sensors[4].dir = 270;

  // Front mid left
  Sensors[5].type = S_IR;
  Sensors[5].val = 0;
  Sensors[5].dir = 15;

  // Front mid right
  Sensors[6].type = S_IR;
  Sensors[6].val = 0;
  Sensors[6].dir = 345;

  Car.numSensors = NUM_SENSORS;
  Car.sensors = Sensors;

  // Start car in the middle of the two walls, no velocity, facing north.
  Car.x = 1500;
  Car.y = 0;
  Car.vel = 1000; // 1000 mm/s
  Car.dir = 90;
}

static void endSim(char * message) {
  OS_RemovePeriodicThread();
  terminal_printString("\r\n");
  SimLogger_PrintToTerminal();
  terminal_printString("\r\n");
  terminal_printString(message);
  terminal_printString("\r\n");
  terminal_printString("Test complete.\r\n\r\n");
	SimComplete = 1;
}

/* 

TEST ENVIRONMENTS

 *        _______ <-- finish line
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |       |
 *       |   C   |
 *        -------

  STRAIGHT

  Walls[0].startX = 1000;
  Walls[0].startY = 0;
  Walls[0].endX = 1000;
  Walls[0].endY = 5000;

  Walls[1].startX = 2000;
  Walls[1].startY = 0;
  Walls[1].endX = 2000;
  Walls[1].endY = 5000;  

*/
