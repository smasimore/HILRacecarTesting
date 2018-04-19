/********** getHighest.c ************** 
 * Authors: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
 * Created Date: 02/25/2018
 * Last Updated Date: 02/25/2018
 Description: Implements a function to get the highest priority of any active thread
*/

#include <stdint.h>
#include "OS.h"

extern uint32_t priorityOccupied;

uint8_t getHighestPriority(void){
	uint8_t leading;
	__asm{
		CLZ leading, priorityOccupied
	}
	return (leading + NUM_PRIORITY_LEVELS) - 32;
}
