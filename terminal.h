/********** terminal.h ************** 
* Authors: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
* Created Date: 01/22/2018
* Last Updated Date: 01/24/2018
Description: Implements interfacing with CLI via UART. See "Command Reference.txt" for command format details.
  If USE_TERMINAL is 0, the UART terminal is not actually supported, and most of these functions are just dummies.
  If TERMINAL_DEBUGGING is 0, debug info messages are suppressed.
*/

#ifndef TERMINAL_H
#define TERMINAL_H

#define USE_TERMINAL 1
#define TERMINAL_DEBUGGING 0

#include "ErrorCodes.h"

/**************terminal_init***************
Description: Initialize UART.
Inputs: none
Outputs: none
*/
void terminal_init(void);

/**************terminal_ReadAndParse***************
Description: Prompts user, reads input from device interfaced via UART, and interperets it as a command.
  The UART connection is initialized automatically, if it has not been initialized already.
Inputs: none
Outputs: ErrorCode
*/
#if USE_TERMINAL
ErrorCode_t terminal_ReadAndParse(void);
#endif

/**************terminal_fatalErrorHandler***************
Description: Prints debug information via UART, in the event of an unrecoverable error.
  The UART connection is initialized automatically, if it has not been initialized already.
  Also disables the operating system.
  This function does not return.
Inputs:
  errorCode - Error code to print
  errorMessage- Message to print
Outputs: None
*/
void terminal_fatalErrorHandler(ErrorCode_t errorCode, char* errorMessage);

/**************terminal_printInfoMessage***************
Description: Prints a string to the UART terminal, preceded by "INFO: ".
  The UART connection is initialized automatically, if it has not been initialized already.
Inputs:
  message - Message to print to the terminal
	severity - 0 to 3
Outputs: None
*/
void terminal_printMessage(char* message, uint8_t severity);

/**************terminal_printMessageNoDebugging***************
Description: Prints a string to the UART terminal, preceded by "INFO: ".
  The UART connection is initialized automatically, if it has not been initialized already.
Inputs:
  message - Message to print to the terminal
	severity - 0 to 3
Outputs: None
*/
void terminal_printMessageNoDebugging(char* message, uint8_t severity);

/**************terminal_printValue***************
Description: Prints a 32-bit integer to the UART terminal in hex format.
  Does not print on a new line, as it is intented to be printed immediately after an info message.
Inputs:
  message - Message to print to the terminal
  Outputs: None
*/
void terminal_printValue(uint32_t value);

/**************terminal_printValueDec***************
Description: Prints a 32-bit integer to the UART terminal in dec format.
  Does not print on a new line, as it is intented to be printed immediately after an info message.
Inputs:
  message - Message to print to the terminal
  Outputs: None
*/
void terminal_printValueDec(uint32_t value);

/**************terminal_printString***************
Description: Prints a 32-bit integer to the UART terminal in dec format.
  Does not print on a new line, as it is intented to be printed immediately after an info message.
Inputs:
  message - Message to print to the terminal
  Outputs: None
*/
void terminal_printString(char * msg);

void HardFault_Handler(void);

#endif
