/**	
 * File: SimLogger.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Log for logging each sim event. Prints results to UART.
 */
 
#include <stdint.h>
#include "SimLogger.h"
#include "Simulator.h"
#include "terminal.h"

uint32_t StartCritical(void); 
void EndCritical(uint32_t oldState);

struct row {
	uint32_t numTicks;
	uint32_t carX;
	uint32_t carY;
	uint32_t carV;
	uint16_t carDir;
	uint32_t sensor0;
	uint32_t sensor1;
	uint32_t actuator0;
	uint32_t actuator1;	
};

struct row SimLog[MAX_NUM_TICKS];
uint16_t NextRow = 0;

/**
 * Log a row to the SimLogger.
 */
void SimLogger_LogRow(struct car * car, uint32_t numTicks) {
	struct row row;
	
	uint32_t oldIntrState = StartCritical();

	if (NextRow == MAX_NUM_TICKS) {
		EndCritical(oldIntrState);
		return;
	}
	
	row.numTicks = numTicks;
	row.carX = car->x;
	row.carY = car->y;
	row.carV = car->vel;
	row.carDir = car->dir;
	row.sensor0 = car->sensors[0].val;
	row.sensor1 = car->sensors[1].val;
	
	SimLog[NextRow++] = row;
	
	EndCritical(oldIntrState);
}

/**
 * Print log to UART.
 */
void SimLogger_PrintToTerminal(void) {
	struct row row;
	int i;
	
	terminal_printString("----- Test Results ----- \r\n");
	terminal_printString("numTicks,carX,car Y,car V,carDir,sensor0,sensor1\r\n");
	
	for (i = 0; i < NextRow; i++) {
		row = SimLog[i];
		terminal_printValueDec(row.numTicks);
		terminal_printString(",");
		terminal_printValueDec(row.carX);
		terminal_printString(",");
		terminal_printValueDec(row.carY);
		terminal_printString(",");
		terminal_printValueDec(row.carV);
		terminal_printString(",");
		terminal_printValueDec(row.carDir);
		terminal_printString(",");
		terminal_printValueDec(row.sensor0);
		terminal_printString(",");
		terminal_printValueDec(row.sensor1);
		terminal_printString("\r\n");
	}
}
