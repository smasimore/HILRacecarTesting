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

#define NUM_US_CHANNELS 3

// Holdoff period from Ping sensor datasheet. # of clock cycles equal to 
// 750 us (@80MHz).
#define PING_HOLDOFF_T 60000 
#define AIR_TMPTR_CELSIUS 23
#define MACH_MM_PER_SECOND (331400 + 600 * AIR_TMPTR_CELSIUS)

// todo, need 3
uint32_t CurPingPeriod = 0;

static uint32_t getPeriodFromPingSensorVal(uint32_t val);

/**
 * Currently only supports one Ping sensor on PA2 and Timer0.
 * TODO: make this work for one and then scale to 3
 */ 
void USSensor_Init(struct sensor * sensor) {
  volatile unsigned long delay;
	static uint8_t nextIRChannel = 0;

	if (nextIRChannel == NUM_US_CHANNELS) {
		terminal_printString("Error: Unsupported number of Ping sensors\r\n");
		return;
	}

	// Initialize pin based on next available PWM channel.
	switch (nextIRChannel) {
		
		// PA3, TIMER0
		case 0:
			// Initialize pin to use for output and input signal
			SYSCTL_RCGCGPIO_R |= 0x1; // Init Port A
			delay = SYSCTL_RCGCGPIO_R;
			GPIO_PORTA_AFSEL_R &= ~0x8; // Disable alternate function for PA3
			GPIO_PORTA_PCTL_R &= ~0xF000; // Clear PA3 (GPIO)
			GPIO_PORTA_AMSEL_R &= ~0x8; // Disable analog functionality on PA3
			GPIO_PORTA_DEN_R |= 0x8; // Enable digital I/O on PA3
			GPIO_PORTA_DIR_R &= ~0x8; // Start PA3 as in 
			GPIO_PORTA_PDR_R |= 0x8; // Pull down register
			
			// Enable interrupt on rising edge
			GPIO_PORTA_IS_R &= ~0x8; // PA3 is edge-sensitive
			GPIO_PORTA_IBE_R &= ~0x8; // PA3 is not both edges
			GPIO_PORTA_IEV_R |= 0x8; // PA3 rising edge event
			GPIO_PORTA_IM_R |= 0x8; // Arm interrupt
			GPIO_PORTA_ICR_R = 0x8; // Clear flag
			NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF00)|0x40; // Priority 2
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
			
			break;
		
		// PA4, TIMER1
		case 1:
			GPIO_PORTA_AFSEL_R &= ~0x10; // Disable alternate function for PA4
			GPIO_PORTA_PCTL_R &= ~0xF0000; // Clear PA4 (GPIO)
			GPIO_PORTA_AMSEL_R &= ~0x10; // Disable analog functionality on PA4
			GPIO_PORTA_DEN_R |= 0x10; // Enable digital I/O on PA4
			GPIO_PORTA_DIR_R &= ~0x10; // Start PA4 as in 
			GPIO_PORTA_PDR_R |= 0x10; // Pull down register
			
			// Enable interrupt on rising edge
			GPIO_PORTA_IS_R &= ~0x10; // PA4 is edge-sensitive
			GPIO_PORTA_IBE_R &= ~0x10; // PA4 is not both edges
			GPIO_PORTA_IEV_R |= 0x10; // PA4 rising edge event
			GPIO_PORTA_IM_R |= 0x10; // Arm interrupt
			GPIO_PORTA_ICR_R = 0x10; // Clear flag
			
			// Initialize timer to interrupt and send Ping response
			SYSCTL_RCGCTIMER_R |= 0x2; // Activate timer0
			TIMER1_CTL_R = 0; // Disable until receive pulse from racecar
			TIMER1_CFG_R = 0; // Configure for 32-bit timer mode (uses A and B)
			TIMER1_TAMR_R = 0x1; // Configure for one-shot mode, default down-count settings
			TIMER1_TAILR_R = PING_HOLDOFF_T; // Period
			TIMER1_TAPR_R = 0; // Prescale value for trigger
			TIMER1_IMR_R = 1; // Arm interrupt
			
			NVIC_EN0_R |= 1<<21; // Enable interrupt 21 in NVIC
			NVIC_PRI5_R = (NVIC_PRI4_R&(~0xF000)) | 0x4000; // Priority 2
			TIMER1_ICR_R = 0x1; // Clear flag
			break;

		// PA5, TIMER2
		case 2:
			GPIO_PORTA_AFSEL_R &= ~0x20; // Disable alternate function for PA5
			GPIO_PORTA_PCTL_R &= ~0xF00000; // Clear PA5 (GPIO)
			GPIO_PORTA_AMSEL_R &= ~0x20; // Disable analog functionality on PA5
			GPIO_PORTA_DEN_R |= 0x20; // Enable digital I/O on PA5
			GPIO_PORTA_DIR_R &= ~0x20; // Start PA5 as in 
			GPIO_PORTA_PDR_R |= 0x20; // Pull down register
			
			// Enable interrupt on rising edge
			GPIO_PORTA_IS_R &= ~0x20; // PA5 is edge-sensitive
			GPIO_PORTA_IBE_R &= ~0x20; // PA5 is not both edges
			GPIO_PORTA_IEV_R |= 0x20; // PA5 rising edge event
			GPIO_PORTA_IM_R |= 0x20; // Arm interrupt
			GPIO_PORTA_ICR_R = 0x20; // Clear flag
			
			// Initialize timer to interrupt and send Ping response
			SYSCTL_RCGCTIMER_R |= 0x4; // Activate timer2
			TIMER2_CTL_R = 0; // Disable until receive pulse from racecar
			TIMER2_CFG_R = 0; // Configure for 32-bit timer mode (uses A and B)
			TIMER2_TAMR_R = 0x1; // Configure for one-shot mode, default down-count settings
			TIMER2_TAILR_R = PING_HOLDOFF_T; // Period
			TIMER2_TAPR_R = 0; // Prescale value for trigger
			TIMER2_IMR_R = 1; // Arm interrupt
			
			NVIC_EN0_R |= 1<<23; // Enable interrupt 23 in NVIC
			NVIC_PRI5_R = (NVIC_PRI4_R&(~0xF0000000)) | 0x40000000; // Priority 2
			TIMER2_ICR_R = 0x1; // Clear flag
			break;
	}

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
	return CLOCK_FREQ / MACH_MM_PER_SECOND * 500 * 2;
}

/**
 * Triggered when get signal from racecar's Ping sensor pin.
 * Currently only supports PA2.
 */
void GPIOPortA_Handler(void){
	// PA3
  if(GPIO_PORTA_RIS_R&0x8){
		// Acknowledge
    GPIO_PORTA_ICR_R = 0x8;
		GPIO_PORTA_IM_R &= ~0x8; // Disarm interrupt
		
		// Enable Timer0
		TIMER0_TAILR_R = PING_HOLDOFF_T; 
		TIMER0_CTL_R = 1;
	
	// PA4
  } else if (GPIO_PORTA_RIS_R&0x10){
		// Acknowledge
    GPIO_PORTA_ICR_R = 0x10;
		GPIO_PORTA_IM_R &= ~0x10; // Disarm interrupt
		
		// Enable Timer1
		TIMER1_TAILR_R = PING_HOLDOFF_T; 
		TIMER1_CTL_R = 1;
 
	// PA5
  } else if (GPIO_PORTA_RIS_R&0x20){
		// Acknowledge
    GPIO_PORTA_ICR_R = 0x20;
		GPIO_PORTA_IM_R &= ~0x20; // Disarm interrupt
		
		// Enable Timer1
		TIMER2_TAILR_R = PING_HOLDOFF_T; 
		TIMER2_CTL_R = 1;
  }
}

/**
 * This triggers in pairs. The first time, send a high over the Ping pin.
 * The second time, stop sending high and set PA3 to input again.
 */
void Timer0A_Handler(void){
	static uint8_t sendHigh = 1;
	
	// Acknowledge
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;

	// Start sending high signal to racecar's Ping input pin
	if (sendHigh) {
		// Flip pin to output
		GPIO_PORTA_DIR_R |= 0x8; // Set as output
		GPIO_PORTA_DATA_R |= 0x8; // Send high signal
		
		// Reset timer with period required to represent distance to nearest wall
		TIMER0_CTL_R = 1;
		TIMER0_TAILR_R = CurPingPeriod;
		
		sendHigh = 0;
	} else {
		// Stop sending high
		GPIO_PORTA_DIR_R &= ~0x8; // Set as input
		GPIO_PORTA_DATA_R &= ~0x8; // Set low
		
		// Arm interrupt
	  GPIO_PORTA_ICR_R = 0x8; // Clear flag
		GPIO_PORTA_IM_R |= 0x8; // Arm interrupt
		
		// Disable timer
		TIMER0_CTL_R = 0;
		
		sendHigh = 1;
  }
}

/**
 * This triggers in pairs. The first time, send a high over the Ping pin.
 * The second time, stop sending high and set PA4 to input again.
 */
void Timer1A_Handler(void){
	static uint8_t sendHigh = 1;
	
	// Acknowledge
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;

	// Start sending high signal to racecar's Ping input pin
	if (sendHigh) {
		// Flip pin to output
		GPIO_PORTA_DIR_R |= 0x10; // Set as output
		GPIO_PORTA_DATA_R |= 0x10; // Send high signal
		
		// Reset timer with period required to represent distance to nearest wall
		TIMER1_CTL_R = 1;
		TIMER1_TAILR_R = CurPingPeriod;
		
		sendHigh = 0;
	} else {
		// Stop sending high
		GPIO_PORTA_DIR_R &= ~0x10; // Set as input
		GPIO_PORTA_DATA_R &= ~0x10; // Set low
		
		// Arm interrupt
	  GPIO_PORTA_ICR_R = 0x10; // Clear flag
		GPIO_PORTA_IM_R |= 0x10; // Arm interrupt
		
		// Disable timer
		TIMER1_CTL_R = 0;
		
		sendHigh = 1;
  }
}

/**
 * This triggers in pairs. The first time, send a high over the Ping pin.
 * The second time, stop sending high and set PA5 to input again.
 */
void Timer2A_Handler(void){
	static uint8_t sendHigh = 1;
	
	// Acknowledge
	TIMER2_ICR_R = TIMER_ICR_TATOCINT;

	// Start sending high signal to racecar's Ping input pin
	if (sendHigh) {
		// Flip pin to output
		GPIO_PORTA_DIR_R |= 0x20; // Set as output
		GPIO_PORTA_DATA_R |= 0x20; // Send high signal
		
		// Reset timer with period required to represent distance to nearest wall
		TIMER2_CTL_R = 1;
		TIMER2_TAILR_R = CurPingPeriod;
		
		sendHigh = 0;
	} else {
		// Stop sending high
		GPIO_PORTA_DIR_R &= ~0x20; // Set as input
		GPIO_PORTA_DATA_R &= ~0x20; // Set low
		
		// Arm interrupt
	  GPIO_PORTA_ICR_R = 0x20; // Clear flag
		GPIO_PORTA_IM_R |= 0x20; // Arm interrupt
		
		// Disable timer
		TIMER2_CTL_R = 0;
		
		sendHigh = 1;
  }
}
