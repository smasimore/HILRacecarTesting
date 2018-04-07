/********** ADC.c ************** 
 Name: Sarah Masimore (sm66498) and Zachary Susskind (zjs362)
 Lab Number: 16000
 Created Date: 01/21/2018
 Last Updated Date: 01/21/2018
 Description: API to initialize and sample ADC one time or using timer. Uses
  ADC0, ADC Seq 3, and Timer2. Code based on ADCSWTrigger.c and ADCT0ATrigger.c.
*/

#include <stdint.h>

/**************ADC_Open***************
Description: Initializes ADC 1 and port corresponding to specified channel.
Inputs:
  channel - ADC channel to initialize
Outputs: None
*/
void ADC_Open(uint8_t channel);

/**************ADC_In***************
Description: Samples ADC on channel specified once.
Inputs:
  channel - ADC channel to sample
  adcVal - pointer to int where adcVal will be stored
Outputs: ErrorCode
*/
uint16_t ADC_In(uint8_t channel);
