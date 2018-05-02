/**
 * File: OS.c
 * Authors: Sarah Masimore and Zachary Susskind
 * Last Updated Date: 02/19/2018
 * Description: API to for OS implementation.
 */

#include <stdint.h>
#include <limits.h>
#include "tm4c123gh6pm.h"
#include "ErrorCodes.h"
#include "OS.h"
#include "terminal.h"
#include "PLL.h"
#include "unitConvert.h"
#include "getHighest.h"
#include "OSAux.h"

extern TCB_t * Current_Thread;

uint32_t StartCritical(void); 
void EndCritical(uint32_t oldState);
void DisableInterrupts(void);
void EnableInterrupts(void);
void StartOS(void);

uint8_t OSLaunched = 0;

// Periodic threads
static void (*PeriodicTask)(void) = 0;
unsigned long Period = 0;

uint32_t TimeSliceCycles;
extern uint32_t Systick_Calls;

void initSystemClockTimer(void);

// Running thread structure
TCB_t* ReadyThreads[NUM_PRIORITY_LEVELS]; // Circular
uint32_t priorityOccupied;

// LL pointers
TCB_t * InactiveThreads = 0;
TCB_t * SleepThreads = 0;

// Allocate memory
TCB_t TCBs[MAX_THREADS];
uint32_t ThreadStacks[MAX_THREADS][STACK_SIZE];

void initStack(TCB_t* TCBPt, void (*task)(void)){
	uint32_t* TCBstackPt;
	TCBPt->stackPointer = &ThreadStacks[TCBPt->tid][STACK_SIZE];
	TCBstackPt = TCBPt->stackPointer;
  *(--TCBstackPt) = 0x01000000;   	// thumb bit
	*(--TCBstackPt) = (uint32_t)task;	// user task
  *(--TCBstackPt) = 0x14141414;   	// R14
  *(--TCBstackPt) = 0x12121212;   	// R12
  *(--TCBstackPt) = 0x03030303;   	// R3
  *(--TCBstackPt) = 0x02020202;   	// R2
  *(--TCBstackPt) = 0x01010101;   	// R1
  *(--TCBstackPt) = 0x00000000;   	// R0
  *(--TCBstackPt) = 0x11111111;   	// R11
  *(--TCBstackPt) = 0x10101010;  		// R10
  *(--TCBstackPt) = 0x09090909;  		// R9
  *(--TCBstackPt) = 0x08080808;  		// R8
  *(--TCBstackPt) = 0x07070707;  		// R7
  *(--TCBstackPt) = 0x06060606;  		// R6
  *(--TCBstackPt) = 0x05050505;  		// R5
  *(--TCBstackPt) = 0x04040404;  		// R4
	TCBPt->stackPointer -= 16;
}

/**************OS_Init***************
Description: Initializes OS, including periodic timer to keep system time.
Inputs: none
Outputs: ErrorCode
*/
ErrorCode_t OS_Init(void) { 
  int iii;
	PLL_Init(Bus80MHz);
  // loop through threads and create I ll
  TCBs[0].prevTCB = &TCBs[MAX_THREADS-1];
  for(iii = 0; iii < MAX_THREADS; iii++){
    terminal_printMessage("Initializing TCB ", 0);
    TCBs[iii].tid = iii;
    TCBs[iii].stackPointer = &ThreadStacks[iii][STACK_SIZE];
    terminal_printValue(iii);
    terminal_printMessage("Previous TCB address is ", 0);
    terminal_printValue((uint32_t)(TCBs[iii].prevTCB));
    terminal_printMessage("Current TCB address is ", 0);
    terminal_printValue((uint32_t)(&TCBs[iii]));
    if(iii == MAX_THREADS - 1){
      break;
    }
    
    TCBs[iii].nextTCB = &TCBs[iii+1];
    
    terminal_printMessage("Next TCB address is ", 0);
    terminal_printValue((uint32_t)(TCBs[iii].nextTCB));
    
    TCBs[iii+1].prevTCB = &TCBs[iii];
  }
  TCBs[MAX_THREADS-1].nextTCB = &TCBs[0];
  terminal_printMessage("Next TCB address is ", 0);
  terminal_printValue((uint32_t)(TCBs[MAX_THREADS-1].nextTCB));

  InactiveThreads = &TCBs[0];
  terminal_printMessage("Initialization successful!", 0);
	
	initSystemClockTimer();
	
  return E_SUCCESS;
}

void initSystemClockTimer(void) {
	volatile int delay;
	
	SYSCTL_RCGCWTIMER_R |= 0x01;
	delay = 42;
	WTIMER0_CTL_R &= ~TIMER_CTL_TAEN;
	WTIMER0_CFG_R = 0x00;
	WTIMER0_TAILR_R = 0xFFFFFFFF;
	WTIMER0_TBILR_R = 0xFFFFFFFF;
	WTIMER0_TAMR_R |= (TIMER_TAMR_TACDIR | TIMER_TAMR_TAMR_1_SHOT);
	WTIMER0_CTL_R |= TIMER_CTL_TAEN;
}

/**************OS_AddThread***************
Description: Adds thread.
Inputs: none
Outputs: 1 if successful
*/
unsigned long OS_AddThread(void (*task)(void), unsigned long stackSize, unsigned long priority) {
  TCB_t* newTCB;
  uint8_t newTID, prevTID, nextTID;
	uint32_t oldIntrState;
  terminal_printMessage("Adding new thread", 0);
  // There is the possibility of a critical section if two threads are being created or destroyed at the same time.
  oldIntrState = StartCritical();
  if (InactiveThreads == 0){
    EndCritical(oldIntrState);
    terminal_printMessage("Failed to add thread!", 3);
  }
  
  newTCB = InactiveThreads;
	if(InactiveThreads != InactiveThreads->nextTCB){
		InactiveThreads->nextTCB->prevTCB = InactiveThreads->prevTCB;
		InactiveThreads->prevTCB->nextTCB = InactiveThreads->nextTCB;
		InactiveThreads = InactiveThreads->nextTCB;
	}
	else{
		InactiveThreads = 0;
	}
  
  EndCritical(oldIntrState);
  
  newTCB->priority = priority;
  initStack(newTCB, task);
  
  oldIntrState = StartCritical();
  if (ReadyThreads[priority] == 0){
    ReadyThreads[priority] = newTCB;
		priorityOccupied |= 1 << (NUM_PRIORITY_LEVELS - priority - 1);
    newTCB->prevTCB = newTCB;
    newTCB->nextTCB = newTCB;
  }
  else{
    newTCB->prevTCB = ReadyThreads[priority]->prevTCB;
    newTCB->nextTCB = ReadyThreads[priority];
    ReadyThreads[priority]->prevTCB->nextTCB = newTCB;
    ReadyThreads[priority]->prevTCB = newTCB;
  }
  
  newTID = newTCB->tid;
  prevTID = newTCB->prevTCB->tid;
  nextTID = newTCB->nextTCB->tid;
  EndCritical(oldIntrState);
	
  if (InactiveThreads == 0){
    terminal_printMessage("No remaining inactive threads", 1);
  }
  terminal_printMessage("Successfully added new thread with TID ", 0);
  terminal_printValue(newTID);
  terminal_printMessage("Previous thread has TID ", 0);
  terminal_printValue(prevTID);
  terminal_printMessage("Next thread has TID ", 0);
  terminal_printValue(nextTID);
	
	return 1;
}

/**************OS_AddPeriodicThread***************
Description: Initializes periodic timer thread.
Inputs:
  task - pointer to function to run in ISR
  period - period in clock cycles
  priority - priority of interrupt (0-7)
Outputs: ErrorCode
*/
ErrorCode_t OS_AddPeriodicThread(void (*task)(void), unsigned long period, 
                                 unsigned long priority) {
  if (priority > 7) {
    return E_INVALID_PRIORITY;
  }
  
	SYSCTL_RCGCTIMER_R |= 0x20; // Activate timer5
	PeriodicTask = task;
	TIMER5_CTL_R = 0; // Disable during setup
	TIMER5_CFG_R = 0; // Configure for 32-bit timer mode (uses A and B)
	TIMER5_TAMR_R = 0x2; // Configure for periodic mode, default down-count settings
	TIMER5_TAILR_R = period; // Configure for periodic mode, default down-count settings
	TIMER5_TAPR_R = 0; // Prescale value for trigger
	TIMER5_IMR_R = 1; // Arm interrupt
	NVIC_EN2_R |= 1<<28; // Enable interrupt 92 in NVIC
	NVIC_PRI25_R = (NVIC_PRI25_R&(~0xF0)) | (priority << 5); // Set priority
	TIMER5_ICR_R = 0x1; // Clear flag
	
	Period = period;
	
	if (OSLaunched) {
		TIMER5_CTL_R = 0x1; // Enable timer
	}
  
  return E_SUCCESS;
}
																 
ErrorCode_t OS_RemovePeriodicThread(void) {
	TIMER5_CTL_R = 0;
	return E_SUCCESS;
}

/**************OS_Disable***************
Description: Disables SysTick and OS, in the event of a fatal error.
Inputs: none
Outputs: none
*/
void OS_Disable(void){
  NVIC_ST_CTRL_R &= ~(0x00010007);
}

/**************Timer5A_Handler***************
Description: Calls periodic task.
Inputs: none
Outputs: none
*/
void Timer5A_Handler(void) {
	TIMER5_ICR_R = 0x1; // Acknowledge
  (*PeriodicTask)(); // Execute user task
}

//******** OS_Id *************** 
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
unsigned long OS_Id(void) {
  return ReadyThreads[getHighestPriority()]->tid;
}

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(unsigned long sleepTime) {
	uint32_t slicesToSleep = ms2slices(sleepTime);
	__asm{
		SVC #2, {r0=slicesToSleep}
	}
} 

// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void) {
	TCB_t** LL_Pt = &InactiveThreads;
	__asm{
		SVC #1, {r0=LL_Pt}
	}
}

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void) {
	__asm {
		SVC #0
	}
}


// ******** OS_Time ************
// return the system time 
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
// The time resolution should be less than or equal to 1us, and the precision 32 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
uint64_t OS_Time(void) {
	uint32_t lowBits, highBits;
	uint64_t currentTime;
	highBits = WTIMER0_TBR_R;
	lowBits = WTIMER0_TAR_R;
	if (highBits != WTIMER0_TBR_R) {
		highBits = WTIMER0_TBR_R;
		lowBits = WTIMER0_TAR_R;
	}
	currentTime = highBits;
	currentTime <<= 32;
	currentTime += lowBits;
	return currentTime;
}

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: absolute time difference in 12.5ns units 
// The time resolution should be less than or equal to 1us, and the precision at least 12 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
uint64_t OS_TimeDifference(uint64_t start, uint64_t stop) {
  return stop - start;
}

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1). Uses Systick_Calls to approximate.
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
// It is ok to make the resolution to match the first call to OS_AddPeriodicThread
unsigned long OS_MsTime(void) {
  return (Systick_Calls * TimeSliceCycles) / 80000;
}

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined TimeSlice field
// It is ok to limit the range of theTimeSlice to match the 24-bit SysTick
void OS_Launch(unsigned long theTimeSlice) {
	if (theTimeSlice > 0x00FFFFFF){
		terminal_fatalErrorHandler(E_INVALID_TIME_SLICE, "Time slice out of bounds");
	}
	if (theTimeSlice < 8000){
		terminal_printMessage("Time slice size less than 100 us", 2);
	}
	TimeSliceCycles = theTimeSlice;
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = theTimeSlice;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x1FFFFFFF) | 0xE0000000;	// SysTick Priority 7
	NVIC_SYS_PRI2_R = (NVIC_SYS_PRI2_R&0x1FFFFFFF) | 0xC0000000;	// SVC Priority 6
	
	if (Period) {
		TIMER5_CTL_R = 0x1; // Enable timer
	}

	OSLaunched = 1;
	
	StartOS();
}
