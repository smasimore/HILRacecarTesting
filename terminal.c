/********** terminal.c ************** 
* Authors: Sarah Masimore and Zachary Susskind 
* Created Date: 01/22/2018
* Last Updated Date: 02/20/2018
Description: Implements interfacing with CLI via UART. See "Command Reference.txt" for command format details.
*/

#define VERSION_NUM "1.1.1"

#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "UART.h"
#include "ErrorCodes.h"
#include "ADC.h"
#include "OS.h"
#include "splash.h"
#include "terminal.h"

#define MAX_BUFFER_LEN 50
#define MAX_PARAM_LEN 25

#if USE_TERMINAL
static volatile uint8_t Initialized;

void OutCRLF(void){    // Moves the terminal cursor to a new line (plays nice with Windows).
  UART_OutChar(CR);
  UART_OutChar(LF);
}

void OutPrompt(void){ // Moves the cursor to a new line, then displays a '>' prompt.
  OutCRLF();
  UART_OutChar('>');
}

/**************readInt***************
Description: Reads data from the buffer as a string terminated by the space character.
Inputs:
  inString -  Input data buffer
  inStringOffset - Offset from beginning of data buffer to begin reading
  outInt - Pointer to integer, value read from data buffer
Outputs: ErrorCode
Side Effects:
  The value of inStringOffset is changed outside the scope of the function.
  The read string is interpereted as an integer and written to outInt.
  May print error information to the console.
*/

ErrorCode_t readInt(char* inString, uint8_t* inStringOffset, int* outIntPt){ int isNegative = 0;
  *outIntPt = 0;
  if(inString[*inStringOffset] == '\0'){  // Input buffer is empty
    UART_OutString("\r\nERROR: Missing parameter(s)");
    return E_MISSING_PARAM;
  }
  if(inString[*inStringOffset] == '-'){
    isNegative = 1;
    *inStringOffset += 1;
  }
  while((inString[*inStringOffset] != ' ') && (inString[*inStringOffset] != '\0')){
    if ((inString[*inStringOffset] < '0') || (inString[*inStringOffset] > '9')){  // Read character is not a valid ASCII number
      UART_OutString("\r\nERROR: Integer parameter is not integer");
      return E_INVALID_PARAM;
    }
    if(*outIntPt > ((INT_MAX / 10) - 1)){
      UART_OutString("\r\nERROR: Integer parameter is too large");
      return E_OVERFLOW;
    }
    *outIntPt *= 10;                      // Decimal leftshift result
    *outIntPt += inString[(*inStringOffset)++] - 0x30;    // Convert ASCII number to integer, then add to result
  }
  if(isNegative) *outIntPt = -(*outIntPt);
  while (inString[*inStringOffset] == ' ') (*inStringOffset)++;  // Advance pointer to next paramter or null terminator
  return E_SUCCESS;
}

/**************readString***************
Description: Reads data from the buffer as a string terminated by the space character.
Inputs:
  inString -  Input data buffer
  inStringOffset - Offset from beginning of data buffer to begin reading
  outString - Output data buffer
Outputs: ErrorCode
Side Effects:
  The value of inStringOffset is changed outside the scope of the function.
  The read string is converted to lowercase and written to outString.
  May print error information to the console.
*/
ErrorCode_t readString(char* inString, uint8_t* inStringOffset, char* outString){ int iii;
  uint8_t ignoreSpaces = 0;
  if(inString[*inStringOffset] == '\0'){
    UART_OutString("\r\nERROR: Missing parameter(s)");
    return E_MISSING_PARAM;
  }
  for(iii = 0; ((inString[*inStringOffset] != ' ') || ignoreSpaces) && (inString[*inStringOffset] != '\0'); iii++){
    if(iii == MAX_PARAM_LEN - 1) {
      UART_OutString("\r\nERROR: String parameter is too long");
      return E_PARAM_TOO_LONG;
    }
    if(inString[*inStringOffset] == '\\'){              // Escape character handling
      char nextChar = inString[*inStringOffset + 1];
      switch(nextChar){
        case '\\':
        case '"':
          outString[iii] = nextChar;
          break;
        case 'n':
          outString[iii] = '\n';
          break;
        default:
          UART_OutString("\r\nERROR: Invalid escape character \\");
          UART_OutChar(nextChar);
          return E_UNKNOWN_ESC_CHARACTER;
      }
        *inStringOffset += 2;
    }
    else if(inString[*inStringOffset] == '"'){          // For strings containing spaces
      ignoreSpaces ^= 1;
      iii--;
      (*inStringOffset)++;
    }
    else outString[iii] = inString[(*inStringOffset)++];
  }
  
  if (ignoreSpaces){
    UART_OutString("\r\nERROR: Missing closing quotes for string");
    return E_NO_CLOSING_QUOTES;
  }
  outString[iii] = '\0';
  while (inString[*inStringOffset] == ' ') (*inStringOffset)++;  // Advance pointer to next paramter or null terminator
  return E_SUCCESS;
}

/**************readStringLower***************
Description: Reads data from the buffer as a string terminated by the space character, then converts it to lowercase.
Inputs:
  inString -  Input data buffer
  inStringOffset - Offset from beginning of data buffer to begin reading
  outString - Output data buffer
Outputs: ErrorCode
Side Effects:
  The value of inStringOffset is changed outside the scope of the function.
  The read string is converted to lowercase and written to outString.
  May print error information to the console.
*/
ErrorCode_t readStringLower(char* inString, uint8_t* inStringOffset, char* outString){ ErrorCode_t error; int iii;
  if((error = readString(inString, inStringOffset, outString)) != E_SUCCESS) return error;
  for(iii = 0; outString[iii] != '\0'; iii++) outString[iii] = tolower(outString[iii]);
  return E_SUCCESS;
}

/**************assertBufferEmpty***************
Description: Ensure that the buffer is empty, implying no additional parameters.
  Throw an error if the buffer is not empty.
Inputs:
  buffer - char array. Should be empty.
Outputs: ErrorCode
*/
ErrorCode_t assertBufferEmpty(char* buffer, uint8_t* bufferOffset){
  if(buffer[*bufferOffset] != '\0'){
    UART_OutString("\r\nERROR: Unexpected parameter(s) after command: ");
    UART_OutString(buffer);
    return E_UNEXPECTED_PARAM;
  }
  return E_SUCCESS;
}

/**************terminal_init***************
Description: Initializes the UART connection and prints a header for the command prompt.
  Sets the 'initialized' flag so that it can be skipped in the future.
Inputs: none
Outputs: none
*/
void terminal_init(void){
  UART_Init();
  UART_OutString("\r\nRunning Hardware-In-The-Loop Program\r\n");
  Initialized = 1;
}

/**************terminal_ReadAndParse***************
Description: Prompts user, reads input from device interfaced via UART, and interperets it as a command.
  The UART connection is initialized automatically, if it has not been initialized already.
Inputs: none
Outputs: ErrorCode
Side Effects: May print error details to the console.
*/
ErrorCode_t terminal_ReadAndParse(void){ ErrorCode_t error;
  char buffer[MAX_BUFFER_LEN];
  char command[MAX_PARAM_LEN];
  uint8_t bufferOffset = 0;
  
  if(!Initialized) terminal_init();
  
  OutPrompt();
  UART_InString(buffer, MAX_BUFFER_LEN);
  
  if(*buffer == '\0') return E_SUCCESS;
  if((error = readStringLower(buffer, &bufferOffset, command)) != E_SUCCESS) return error;
  
	// NO COMMANDS CURRENTLY SUPPORTED FOR THIS PROGRAM
	
	UART_OutString("\r\nERROR: Unknown command: ");
	UART_OutString(command);
	return E_INVALID_COMMAND;
}

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
void terminal_fatalErrorHandler(ErrorCode_t errorCode, char* errorMessage){
  if(!Initialized) terminal_init();
  UART_OutString(fatalErrorText);  
  UART_OutString(errorMessage);
  UART_OutString("\r\nError Code: ");
  UART_OutUDec(errorCode);
  OS_Disable();
  while(1);
}

/**************terminal_printInfoMessage***************
Description: Prints a string to the UART terminal, preceded by "INFO: ".
  The UART connection is initialized automatically, if it has not been initialized already.
Inputs:
  message - Message to print to the terminal
	severity - 0 to 3
Outputs: None
*/
void terminal_printMessage(char* message, uint8_t severity){
  #if TERMINAL_DEBUGGING
  if(!Initialized) terminal_init();
	switch (severity){
		case 0:
			UART_OutString("\r\nINFO: ");
			break;
		case 1:
			UART_OutString("\r\n-ADVISORY-: ");
			break;
		case 2:
			UART_OutString("\r\n!-WARNING-!: ");
			break;
		default:
			UART_OutString("\r\n!!!-CRITICAL WARNING-!!!: ");
	}
  UART_OutString(message);
  #endif
}

/**************terminal_printMessageNoDebugging***************
Description: Prints a string to the UART terminal, preceded by "INFO: ".
  The UART connection is initialized automatically, if it has not been initialized already.
Inputs:
  message - Message to print to the terminal
	severity - 0 to 3
Outputs: None
*/
void terminal_printMessageNoDebugging(char* message, uint8_t severity){
  if(!Initialized) terminal_init();
	switch (severity){
		case 0:
			UART_OutString("\r\nINFO: ");
			break;
		case 1:
			UART_OutString("\r\n-ADVISORY-: ");
			break;
		case 2:
			UART_OutString("\r\n!-WARNING-!: ");
			break;
		default:
			UART_OutString("\r\n!!!-CRITICAL WARNING-!!!: ");
	}
  UART_OutString(message);
}

/**************terminal_printValue***************
Description: Prints a 32-bit integer to the UART terminal in hex format.
  Does not print on a new line, as it is intented to be printed immediately after an info message.
Inputs:
  message - Message to print to the terminal
  Outputs: None
*/
void terminal_printValue(uint32_t value){
  #if TERMINAL_DEBUGGING
  if(!Initialized) terminal_init();
  UART_OutUHex(value);
  #endif
}

/**************terminal_printValueDec***************
Description: Prints a 32-bit integer to the UART terminal in dec format.
  Does not print on a new line, as it is intented to be printed immediately after an info message.
Inputs:
  message - Message to print to the terminal
  Outputs: None
*/
void terminal_printValueDec(uint32_t value){
  if(!Initialized) terminal_init();
  UART_OutUDec(value);
}

/**************terminal_printString***************
Description: Prints a 32-bit integer to the UART terminal in dec format.
  Does not print on a new line, as it is intented to be printed immediately after an info message.
Inputs:
  message - Message to print to the terminal
  Outputs: None
*/
void terminal_printString(char * msg){
  if(!Initialized) terminal_init();
  UART_OutString(msg);
}

#endif

void HardFault_Handler(void){ // needs to be fixed!
  while(1);
}
