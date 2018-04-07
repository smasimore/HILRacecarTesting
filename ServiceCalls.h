/********** ServiceCalls.h ************** 
Name: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
Lab Number: 16000
Created Date: 02/09/2018
Last Updated Date: 02/28/2018
Description: Handles service calls (those which are written in C)
	Note that some service calls are in assembly, and are located in OSasm.s
*/

#ifndef SERVICECALLS_H
#define SERVICECALLS_H

#include <stdint.h>
#include "OS.h"

/******** OS_Transfer_SVC_C ************
 Transfers the currently active TCB to the specified linked list
 Inputs : LL_Pt - a pointer to the linked list
 Outputs: none
*/
void OS_Transfer_SVC_C(TCB_t** LL_Pt);

/**************OS_Sleep_SVC_C***************
 Transfers the currently active TCB to the ordered sleep list
 Inputs : sleepSlices - the number of time slices for which to sleep
 Outputs: None
*/
void OS_Sleep_SVC_C(uint32_t sleepSlices);

/**************OS_Wait_SVC_C***************
 Implements counting and binary semaphore blocking
		If semaphore > 0, decrement and return
		If semaphore == 0, block the calling thread
 Inputs : semaPt - pointer to the semaphore to check
 Outputs: None
*/
void OS_xWait_SVC_C(Sema4Type *semaPt);

#endif
