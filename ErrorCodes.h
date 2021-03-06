/********** ErrorCodes.h ************** 
* Authors: Sarah Masimore and Zachary Susskind
* Created Date: 01/21/2018
* Last Updated Date: 01/27/2018
Description: Error code constants for RTOS.
*/

#ifndef ERRORCODES_H
#define ERRORCODES_H

typedef enum ErrorCodes {
  // Generic
  E_SUCCESS, 
  E_OVERFLOW,
  E_NOT_INITIALIZED,
  E_ALREADY_INITIALIZED,
  E_UNKNOWN_ERROR = 99,

  // DisplayErrorCodes
  E_OUT_OF_BOUNDS = 100, 
  E_BUFFER_TOO_SMALL,

  // Terminal
  E_MISSING_PARAM = 200, 
  E_UNEXPECTED_PARAM, 
  E_INVALID_COMMAND, 
  E_INVALID_PARAM, 
  E_PARAM_TOO_LONG,
  E_UNKNOWN_ESC_CHARACTER,
  E_NO_CLOSING_QUOTES,
  E_ADC_CURRENTLY_COLLECTING,
  E_NO_ADC_SAMPLE_HISTORY,
  
  // ADC
  E_CHANNEL_CLOSED = 300,
  E_CHANNEL_ALREADY_OPEN,
  E_INVALID_CHANNEL_NUMBER,
  E_FREQUENCY_OUT_OF_BOUNDS,
  E_ALREADY_COLLECTING,
  
  // Timers
  E_INVALID_PRIORITY = 400,
  E_NO_PERIODIC_TIMERS_LEFT,
  
  // OS
  E_INVALID_TIME_SLICE = 500,
  E_NO_SPACE_FOR_THREAD,
  E_STOPPED_SOLE_ACTIVE_THREAD,
  E_NO_ACTIVE_THREADS,
  
} ErrorCode_t;

#endif
