/**	
 * File: SimLogger.h
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Log for logging each sim event. Prints results to UART.
 */

#include <stdint.h>
#include "Simulator.h"

/**
 * Log a row to the SimLogger.
 */
void SimLogger_LogRow(struct car * car, uint32_t numTicks);

/**
 * Print log to UART.
 */
void SimLogger_PrintToTerminal(void);
