/********** OSAux.c ************** 
 * Authors: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
 * Created Date: 02/15/2018
 * Last Updated Date: 02/28/2018
 Description: Handles suppemental OS functions
	These should not be called by a process
*/

#include "OS.h"
#include "getHighest.h"

uint32_t StartCritical(void); // Disable interrupts
void EndCritical(uint32_t oldState);  // Enable interrupts

extern TCB_t * ReadyThreads[NUM_PRIORITY_LEVELS];
extern uint32_t priorityOccupied;

extern TCB_t * SleepThreads;

extern uint32_t CS_Ignore;
extern uint32_t Systick_Calls;
extern TCB_t* Current_Thread;

/******** restoreBlockedThread ************
 Restores the first highest priority thread from specified block list
 Inputs: semaPt - pointer to the semaphore we wish to unblock from
 Outputs: none
*/
void restoreBlockedThread(Sema4Type *semaPt){
		uint8_t priority;
		TCB_t *firstHighest, *firstTCB, *currentTCB;
		firstTCB = semaPt->Queue;
		firstHighest = firstTCB;
		for(currentTCB = firstTCB->nextTCB; currentTCB != firstTCB; currentTCB = currentTCB->nextTCB){
			if (firstHighest->priority == 0) break;
			if(currentTCB->priority < firstHighest->priority){
				firstHighest = currentTCB;
			}
		}
		
		// dequeue from blocked list
		if(firstHighest->nextTCB != firstHighest){
			firstHighest->nextTCB->prevTCB = firstHighest->prevTCB;
			firstHighest->prevTCB->nextTCB = firstHighest->nextTCB;
			if(firstHighest == firstTCB){
				semaPt->Queue = firstTCB->nextTCB;
			}
		}
		else{
			semaPt->Queue = 0;
		}
		
		// enqueue in ready
		priority = firstHighest->priority;
		if(ReadyThreads[priority] == 0){
			ReadyThreads[priority] = firstHighest;
			priorityOccupied |= 1 << (NUM_PRIORITY_LEVELS - priority - 1);
			firstHighest->prevTCB = firstHighest;
			firstHighest->nextTCB = firstHighest;
		}
		else{
			firstHighest->prevTCB = ReadyThreads[priority]->prevTCB;
			firstHighest->nextTCB = ReadyThreads[priority];
			ReadyThreads[priority]->prevTCB->nextTCB = firstHighest;
			ReadyThreads[priority]->prevTCB = firstHighest;
		}
}

void OSAux_Wake(void){
	TCB_t* TCB_Pt;
	uint32_t criticalStatus = StartCritical();
	TCB_Pt = SleepThreads;
	while ((TCB_Pt != 0) && (TCB_Pt->sleepUntil <= Systick_Calls)){
		SleepThreads = SleepThreads->nextTCB;
		if(SleepThreads != 0){
			SleepThreads->prevTCB = 0;
		}
		
		// No TCB's in priority list
		if (!ReadyThreads[TCB_Pt->priority]) {
			ReadyThreads[TCB_Pt->priority] = TCB_Pt;
			TCB_Pt->nextTCB = TCB_Pt;
			TCB_Pt->prevTCB = TCB_Pt;
			priorityOccupied |= 1 << (NUM_PRIORITY_LEVELS - TCB_Pt->priority - 1);
		} else {
			// Otherwise, append to back
			TCB_Pt->prevTCB = ReadyThreads[TCB_Pt->priority]->prevTCB;
			TCB_Pt->nextTCB = ReadyThreads[TCB_Pt->priority];
			ReadyThreads[TCB_Pt->priority]->prevTCB->nextTCB = TCB_Pt;
			ReadyThreads[TCB_Pt->priority]->prevTCB = TCB_Pt;
		}
		
		TCB_Pt = SleepThreads;
	}
	EndCritical(criticalStatus);
}
