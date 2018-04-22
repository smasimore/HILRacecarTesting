/**	
 * File: USSensor.c
 * Author: Sarah Masimore
 * Last Updated Date: 04/19/2018
 * Description: Manages init'ing and updating Ultrasonic Ping sensor ports and 
 *              pins.
 */
 
 #include "tm4c123gh6pm.h"
 #include "USSensor.h"
 #include "terminal.h"
 #include "USSensorLookup.h"
 
// Holdoff period from Ping sensor datasheet. # of clock cycles equal to 
// 750 us (@80MHz).
#define PING_HOLDOFF_T 60000 
#define AIR_TMPTR_CELSIUS				23
#define MACH_MM_PER_SECOND			331400 + 600 * AIR_TMPTR_CELSIUS

uint32_t CurPingPeriod = 0;

static uint32_t getPeriodFromPingSensorVal(uint32_t val);

/**
 * Currently only supports one Ping sensor on PA2 and Timer0.
 * TODO: make this work for one and then scale to 3
 */ 
void USSensor_Init(struct sensor * sensor) {
  volatile unsigned long delay;
	static uint8_t nextIRChannel = 0;
	
	if (nextIRChannel > 0) {
		terminal_printString("Error: Unsupported number of Ping sensors\r\n");
		return;
	}

	// Initialize pin to use for output and input signal
	SYSCTL_RCGCGPIO_R |= 0xC; // Init Port A
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTA_AFSEL_R &= ~0xC; // Disable alternate function for PA2
	GPIO_PORTA_PCTL_R &= ~0xFF00; // Clear PA2 (GPIO)
	GPIO_PORTA_AMSEL_R &= ~0xC; // Disable analog functionality on PA0=2
	GPIO_PORTA_DEN_R |= 0xC; // Enable digital I/O on PA2
 	GPIO_PORTA_DIR_R &= ~0x4; // Start port 0 as in 
	GPIO_PORTA_PDR_R |= 0x4; // Pull down register

  // TESTING todo rm
	GPIO_PORTA_DIR_R |= 0x8;	
	GPIO_PORTA_DATA_R = 0x8; // PA3 high for testing
	
	// Enable interrupt on rising edge
  GPIO_PORTA_IS_R &= ~0x4; // PA0 is edge-sensitive
  GPIO_PORTA_IBE_R &= ~0x4; // PA0 is not both edges
  GPIO_PORTA_IEV_R |= 0x4; // PA0 rising edge event
  GPIO_PORTA_ICR_R = 0x4; // Clear flag
  GPIO_PORTA_IM_R |= 0x4; // Arm interrupt
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFFFFFF00)|0x40; // PPriority 2
  NVIC_EN0_R |= 0x1; // Enable interrupt 0 in NVIC
	
	// Initialize timer to interrupt and send Ping response
	SYSCTL_RCGCTIMER_R |= 0x1; // Activate timer0
	TIMER0_CTL_R = 0; // Disable until receive pulse from racecar
	TIMER0_CFG_R = 0; // Configure for 32-bit timer mode (uses A and B)
	TIMER0_TAMR_R = 0x1; // Configure for one-shot mode, default down-count settings
	TIMER0_TAILR_R = PING_HOLDOFF_T; // Period
	TIMER0_TAPR_R = 0; // Prescale value for trigger
	TIMER0_IMR_R = 1; // Arm interrupt
	NVIC_EN0_R |= 1<<19; // Enable interrupt 19 in NVIC
	NVIC_PRI4_R = (NVIC_PRI4_R&(~0xF0000000)) | 0x40000000; // Priority 2
	TIMER0_ICR_R = 0x1; // Clear flag

	sensor->channel = nextIRChannel++;
}

/**
 * TODO: comment
 */
void USSensor_UpdateOutput(struct sensor * sensor) {
  CurPingPeriod = getPeriodFromPingSensorVal(sensor->val);
}
 
/**
 * Calculates period in cycles to elapse before sending response.
 */
static uint32_t getPeriodFromPingSensorVal(uint32_t val) {
	return PING_HOLDOFF_T + val * CLOCK_FREQ * 2 / MACH_MM_PER_SECOND;
}

// TODO: looks like this is triggering off of PA1... is it armed?
/**
 * Triggered when get signal from racecar's Ping sensor pin.
 * Currently only supports PA2.
 */
void GPIOPortA_Handler(void){
	// PA2
  if(GPIO_PORTA_RIS_R&0x04){
		// Acknowledge
    GPIO_PORTA_ICR_R = 0x4;
		GPIO_PORTA_IM_R |= ~0x4; // Disarm interrupt
		
		// Enable Timer0
		TIMER0_TAILR_R = PING_HOLDOFF_T; 
		TIMER0_CTL_R = 1;
  }
}

/**
 * This triggers in pairs. The first time, send a high over the Ping pin (PA2).
 * The second time, stop sending high and set PA2 to input again.
 */
void Timer0A_Handler(void){
	static uint8_t sendHigh = 1;
	
	// Acknowledge
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;

	// Start sending high signal to racecar's Ping input pin
	if (sendHigh) {
		// Flip pin to output
		GPIO_PORTA_DIR_R |= 0x4; // Set as output
		GPIO_PORTA_DATA_R = 0x4; // Send high signal
		
		// Reset timer with period required to represent distance to nearest wall
		TIMER0_CTL_R = 1;
		TIMER0_TAILR_R = CurPingPeriod;
		
		sendHigh = 0;
	} else {
		// Stop sending high
		GPIO_PORTA_DATA_R &= ~0x4; // Set low
		GPIO_PORTA_DIR_R &= ~0x4; // Set as input
		
		// Arm interrupt
	  GPIO_PORTA_ICR_R = 0x4; // Clear flag
		GPIO_PORTA_IM_R |= 0x4; // Arm interrupt
		
		// Disable timer
		TIMER0_CTL_R = 0;
		
		sendHigh = 1;
  }
}
