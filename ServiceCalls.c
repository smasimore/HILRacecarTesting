/********** ServiceCalls.h ************** 
* Authors: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
* Created Date: 02/09/2018
* Last Updated Date: 02/28/2018
Description: Handles service calls (those which are written in C)
	Note that some service calls are in assembly, and are located in OSasm.s
*/

#include <stdint.h>
#include "OS.h"
#include "terminal.h"
#include "getHighest.h"


extern TCB_t* ReadyThreads[NUM_PRIORITY_LEVELS];
extern uint32_t priorityOccupied;

extern TCB_t * SleepThreads;

extern TCB_t TCBs[10];

extern uint32_t CS_Ignore;
extern uint32_t Systick_Calls;
extern TCB_t* Current_Thread;

/******** removeActiveThread ************
 Removes the previously active thread from the ready list
		Throws an error if there is no next thread
 Inputs: none
 Outputs: oldTCB_Pt - Pointer to the previously active thread
 Note: This function does NOT pack the context of the currently active thread - that needs to have been done previously!
*/
void removeActiveThread(void){
	int priority = Current_Thread->priority;
	if(Current_Thread->nextTCB != Current_Thread){
		Current_Thread->nextTCB->prevTCB = Current_Thread->prevTCB;
		Current_Thread->prevTCB->nextTCB = Current_Thread->nextTCB;
		ReadyThreads[priority] = Current_Thread->nextTCB;
	}
	else{
		ReadyThreads[priority] = 0;
		priorityOccupied &= ~(1 << (NUM_PRIORITY_LEVELS - priority - 1));
		if(priorityOccupied == 0){		// No threads remaining
			terminal_fatalErrorHandler(E_STOPPED_SOLE_ACTIVE_THREAD, "Attempted to stop the only running thread");
		}
	}
}

/******** OS_Transfer_SVC_C ************
 Transfers the currently active TCB to the specified linked list
 Inputs : LL_Pt - a pointer to the linked list
 Outputs: none
*/
void OS_Transfer_SVC_C(TCB_t** LL_Pt){
	removeActiveThread();
	
	CS_Ignore = 1;
	
	if(*LL_Pt != 0){
		Current_Thread->nextTCB = *LL_Pt;
		Current_Thread->prevTCB = (*LL_Pt)->prevTCB;
		(*LL_Pt)->prevTCB->nextTCB = Current_Thread;
		(*LL_Pt)->prevTCB = Current_Thread;
	}
	else{
		*LL_Pt = Current_Thread;
		Current_Thread->nextTCB = Current_Thread;
		Current_Thread->prevTCB = Current_Thread;
	}
}

/**************OS_Sleep_SVC_C***************
 Transfers the currently active TCB to the ordered sleep list
 Inputs : sleepSlices - the number of time slices for which to sleep
 Outputs: None
*/
void OS_Sleep_SVC_C(uint32_t sleepSlices){
	uint32_t wakeSlice = Systick_Calls + sleepSlices;
	
	removeActiveThread();
	Current_Thread->sleepUntil = wakeSlice;
	
	CS_Ignore = 1;
	
	if(SleepThreads != 0){
		TCB_t* TCB_Pt;
		int found = 1;
		int first = 1;
		TCB_Pt = SleepThreads;
		while(TCB_Pt->sleepUntil < Current_Thread->sleepUntil){
			if(TCB_Pt->nextTCB == 0){
				found = 0;
				break;
			}
			first = 0;
			TCB_Pt = TCB_Pt->nextTCB;
		}
		if(found){
			Current_Thread->prevTCB = TCB_Pt->prevTCB;
			Current_Thread->nextTCB = TCB_Pt;
			if (TCB_Pt -> prevTCB != 0){
				TCB_Pt->prevTCB->nextTCB = Current_Thread;
			}
			TCB_Pt->prevTCB = Current_Thread;
			if(first){
				SleepThreads = Current_Thread;
			}
		}
		else{
			Current_Thread->prevTCB = TCB_Pt;
			Current_Thread->nextTCB = 0;
			TCB_Pt->nextTCB = Current_Thread;
		}
	}
	else{
		Current_Thread->nextTCB = 0;
		SleepThreads = Current_Thread;
		Current_Thread->prevTCB = 0;
	}
}
