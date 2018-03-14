/**	
 * File: SimLogger.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Log for logging each sim event. Prints results to UART.
 */
 
#include <stdint.h>
#include "SimLogger.h"
#include "Simulator.h"
#include "UART.h"

#define MAX_ROWS 100 // Will log for 100s since ST frequency is 10Hz

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

struct row SimLog[MAX_ROWS];
uint16_t NextRow = 0;

/**
 * Log a row to the SimLogger.
 */
void SimLogger_LogRow(struct car * car, uint32_t numTicks) {
	struct row row;
	uint32_t oldIntrState = StartCritical();
	
	row.numTicks = numTicks;
	row.carX = car->x;
	row.carY = car->y;
	row.carV = car->v;
	row.carDir = car->dir;
	row.sensor0 = car->sensors[0].val;
	row.sensor1 = car->sensors[1].val;
	
	EndCritical(oldIntrState);
	
	SimLog[NextRow++] = row;
}

/**
 * Print log to UART.
 */
void SimLogger_PrintToUART(void) {
	struct row row;
	int i;
	
	UART_OutString("----- Test Results ----- \r\n");
	UART_OutString("numTicks,carX,car Y,car V,carDir,sensor0,sensor1\r\n");
	
	for (i = 0; i < NextRow; i++) {
		row = SimLog[i];
		UART_OutUDec(row.numTicks);
		UART_OutString(",");
		UART_OutUDec(row.carX);
		UART_OutString(",");
		UART_OutUDec(row.carY);
		UART_OutString(",");
		UART_OutUDec(row.carV);
		UART_OutString(",");
		UART_OutUDec(row.carDir);
		UART_OutString(",");
		UART_OutUDec(row.sensor0);
		UART_OutString(",");
		UART_OutUDec(row.sensor1);
		UART_OutString("\r\n");
	}
}
