/**
 * File: ADC.c
 * Authors: Sarah Masimore and Zachary Susskind
 * Last Updated Date: 01/31/2018
 * Description: API to initialize and sample ADC one time or using timer. Uses
 *              ADC0, ADC Seq 3, and Timer2. Code based on ADCSWTrigger.c and 
 *              ADCT0ATrigger.c.
 */

#include "tm4c123gh6pm.h"
#include "ADC.h"

const uint8_t NUM_CHANNELS = 12;
const uint8_t HW_AVG = ADC_SAC_AVG_OFF;

typedef enum ChannelStatus {CLOSED, OPEN} ChannelStatus;
ChannelStatus ChannelStatuses[NUM_CHANNELS]; 

static void initADC(void);
static void initChannelArr(void);
static void initChannel(uint8_t channel);

/**************ADC_Open***************
Description: Initializes ADC, port corresponding to specified channel,
  and Timer2.
Inputs:
  channel - ADC channel to initialize (0-11). Will fail silently if OOB.
Outputs: ErrorCode
*/
void ADC_Open(uint8_t channel) {
  // Keep track of components that only need to be init'd once.
  static uint8_t firstInit = 1;

  if (firstInit) {
    // Need to activate ADC1 before the ports and ports before the rest of 
    // ADC0.
    SYSCTL_RCGCADC_R |= 0x2; // Activate ADC1
    initChannelArr();
    initADC();
    firstInit = 0;
  }
  
  if (ChannelStatuses[channel] == CLOSED) {
    initChannel(channel);
    ChannelStatuses[channel] = OPEN;
  } 
}

/**************ADC_In***************
Description: Samples ADC on channel specified once.
Inputs:
  channel - ADC channel to sample
Outputs: ADC value
*/
uint16_t ADC_In(uint8_t channel) { 
	uint16_t val;
	
  // Init SW trigger.
  ADC1_EMUX_R &= ~0xF000; // Set Seq3 as software trigger
  ADC1_SSMUX3_R = channel; // Set channel
  ADC1_IM_R &= ~0x8; // Disable SS3 interrupts
  ADC1_ACTSS_R |= 0x8; // Enable Seq3
  
  // Get sample
  ADC1_PSSI_R = 0x8; // Initiate SS3
  while((ADC1_RIS_R&0x8)==0){}; // Wait for conversion to complete
  val = ADC1_SSFIFO3_R&0xFFF; // Read result
  ADC1_ISC_R = 0x8; // Acknowledge completion
  
  return val;
}

/**************initADC***************
Description: Initializes common parts of ADC0 using sequencer 3.
Inputs: none
Outputs: none
*/
static void initADC(void) {
  volatile uint32_t delay;
  ADC1_PC_R = 0x1; // Configure for 125k samples/sec
  ADC1_SSPRI_R = 0x3210; // Set priorities of sequencers, 3 is lowest
  ADC1_ACTSS_R &= ~0x08; // Disable sample sequencer 3
  ADC1_SSCTL3_R = 0x06; // Set flag (IE0) and end (END0)
  ADC1_SAC_R = HW_AVG; // Set hardware averaging  
}

/**************initChannelArr***************
Description: Initializes Timer2 (does not arm).
Inputs: none
Outputs: none
*/
static void initChannelArr(void) {
  int i;
  for (i = 0; i < NUM_CHANNELS; i++) {
    ChannelStatuses[i] = CLOSED;
  }
}

/**************initChannel***************
Description: Initializes port corresponding to ADC channel.
Inputs:
  channel - channel to initialize
Outputs: none
*/
static void initChannel(uint8_t channel) {
  volatile uint32_t delay;
  // Activate clock
  switch(channel){
    case 0:
    case 1:
    case 2:
    case 3:
    case 8:
    case 9:
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; 
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; 
      break;
    case 10:
    case 11:
      SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; 
      break;
    default: 
      return;
  }
  
  delay = SYSCTL_RCGCGPIO_R; // Wait for clock
  switch(channel){
    case 0:                       //      Ain0 is on PE3
      GPIO_PORTE_DIR_R &= ~0x08;  // 3.0) make PE3 input
      GPIO_PORTE_AFSEL_R |= 0x08; // 4.0) enable alternate function on PE3
      GPIO_PORTE_DEN_R &= ~0x08;  // 5.0) disable digital I/O on PE3
      GPIO_PORTE_AMSEL_R |= 0x08; // 6.0) enable analog functionality on PE3
      break;
    case 1:                       //      Ain1 is on PE2
      GPIO_PORTE_DIR_R &= ~0x04;  // 3.1) make PE2 input
      GPIO_PORTE_AFSEL_R |= 0x04; // 4.1) enable alternate function on PE2
      GPIO_PORTE_DEN_R &= ~0x04;  // 5.1) disable digital I/O on PE2
      GPIO_PORTE_AMSEL_R |= 0x04; // 6.1) enable analog functionality on PE2
      break;
    case 2:                       //      Ain2 is on PE1
      GPIO_PORTE_DIR_R &= ~0x02;  // 3.2) make PE1 input
      GPIO_PORTE_AFSEL_R |= 0x02; // 4.2) enable alternate function on PE1
      GPIO_PORTE_DEN_R &= ~0x02;  // 5.2) disable digital I/O on PE1
      GPIO_PORTE_AMSEL_R |= 0x02; // 6.2) enable analog functionality on PE1
      break;
    case 3:                       //      Ain3 is on PE0
      GPIO_PORTE_DIR_R &= ~0x01;  // 3.3) make PE0 input
      GPIO_PORTE_AFSEL_R |= 0x01; // 4.3) enable alternate function on PE0
      GPIO_PORTE_DEN_R &= ~0x01;  // 5.3) disable digital I/O on PE0
      GPIO_PORTE_AMSEL_R |= 0x01; // 6.3) enable analog functionality on PE0
      break;
    case 4:                       //      Ain4 is on PD3
      GPIO_PORTD_DIR_R &= ~0x08;  // 3.4) make PD3 input
      GPIO_PORTD_AFSEL_R |= 0x08; // 4.4) enable alternate function on PD3
      GPIO_PORTD_DEN_R &= ~0x08;  // 5.4) disable digital I/O on PD3
      GPIO_PORTD_AMSEL_R |= 0x08; // 6.4) enable analog functionality on PD3
      break;
    case 5:                       //      Ain5 is on PD2
      GPIO_PORTD_DIR_R &= ~0x04;  // 3.5) make PD2 input
      GPIO_PORTD_AFSEL_R |= 0x04; // 4.5) enable alternate function on PD2
      GPIO_PORTD_DEN_R &= ~0x04;  // 5.5) disable digital I/O on PD2
      GPIO_PORTD_AMSEL_R |= 0x04; // 6.5) enable analog functionality on PD2
      break;
    case 6:                       //      Ain6 is on PD1
      GPIO_PORTD_DIR_R &= ~0x02;  // 3.6) make PD1 input
      GPIO_PORTD_AFSEL_R |= 0x02; // 4.6) enable alternate function on PD1
      GPIO_PORTD_DEN_R &= ~0x02;  // 5.6) disable digital I/O on PD1
      GPIO_PORTD_AMSEL_R |= 0x02; // 6.6) enable analog functionality on PD1
      break;
    case 7:                       //      Ain7 is on PD0
      GPIO_PORTD_DIR_R &= ~0x01;  // 3.7) make PD0 input
      GPIO_PORTD_AFSEL_R |= 0x01; // 4.7) enable alternate function on PD0
      GPIO_PORTD_DEN_R &= ~0x01;  // 5.7) disable digital I/O on PD0
      GPIO_PORTD_AMSEL_R |= 0x01; // 6.7) enable analog functionality on PD0
      break;
    case 8:                       //      Ain8 is on PE5
      GPIO_PORTE_DIR_R &= ~0x20;  // 3.8) make PE5 input
      GPIO_PORTE_AFSEL_R |= 0x20; // 4.8) enable alternate function on PE5
      GPIO_PORTE_DEN_R &= ~0x20;  // 5.8) disable digital I/O on PE5
      GPIO_PORTE_AMSEL_R |= 0x20; // 6.8) enable analog functionality on PE5
      break;
    case 9:                       //      Ain9 is on PE4
      GPIO_PORTE_DIR_R &= ~0x10;  // 3.9) make PE4 input
      GPIO_PORTE_AFSEL_R |= 0x10; // 4.9) enable alternate function on PE4
      GPIO_PORTE_DEN_R &= ~0x10;  // 5.9) disable digital I/O on PE4
      GPIO_PORTE_AMSEL_R |= 0x10; // 6.9) enable analog functionality on PE4
      break;
    case 10:                      //       Ain10 is on PB4
      GPIO_PORTB_DIR_R &= ~0x10;  // 3.10) make PB4 input
      GPIO_PORTB_AFSEL_R |= 0x10; // 4.10) enable alternate function on PB4
      GPIO_PORTB_DEN_R &= ~0x10;  // 5.10) disable digital I/O on PB4
      GPIO_PORTB_AMSEL_R |= 0x10; // 6.10) enable analog functionality on PB4
      break;
    case 11:                      //       Ain11 is on PB5
      GPIO_PORTB_DIR_R &= ~0x20;  // 3.11) make PB5 input
      GPIO_PORTB_AFSEL_R |= 0x20; // 4.11) enable alternate function on PB5
      GPIO_PORTB_DEN_R &= ~0x20;  // 5.11) disable digital I/O on PB5
      GPIO_PORTB_AMSEL_R |= 0x20; // 6.11) enable analog functionality on PB5
      break;
  }
}
