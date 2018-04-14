/**	
 * File: Sensors.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing IR and Ping sensor ports and pins.
 */

#include "Simulator.h"
#include "Sensors.h"
#include "tm4c123gh6pm.h"
#include "terminal.h"

#define NUM_PWM_CHANNELS 6
#define PWM_PERIOD 8000 // 1000 Hz

#define PING_HOLDOFF_T 60000 // # of clock cycles equal to 750 us (@80MHz)

uint8_t SensorsInitialized = 0;

// Ping sensor globals
uint32_t CurPingPeriod = 0;

// IR sensor
void initIRChannel(struct sensor * sensor);
uint16_t getDutyFromIRSensorVal(uint32_t val);
void setIRChannelDuty(uint8_t channel, uint16_t duty);

// Ping sensor functions 
// TODO: Ping untested
void initPingChannel(struct sensor * sensor);
uint32_t getPeriodFromPingSensorVal(uint32_t val);

/**
 * Init sensor's required output pins.
 */
void Sensors_Init(struct car * car) {
	uint16_t numSensors = car->numSensors;
	int i;
	struct sensor * sensor;
	
	// Based on sensor numbers, init relevant pins.
	for (i = 0; i < numSensors; i++) {
		sensor = &car->sensors[i];
		switch (sensor->type) {
			case S_TEST:
			case S_PING:
				initPingChannel(sensor);
				break;
			case S_IR:
				initIRChannel(sensor);
				break;
			default:
				terminal_printString("Error: Unsupported sensor type\r\n");
				continue;
		}
	}
}

/**
 * Init PWM channel for a sensor. Currently supports up to 6 channels. PWM on
 * TM4C supports up to 8 different PWM outputs. Channel sensor assigned to 
 * added to sensor struct.
 *
 * Note: PWM output pins come in pairs with same output. Need to be careful
 * which pin is chosen.
 *
 * Channel 0 --> PB6 --> M0PWM0
 * Channel 1 --> PC4 --> M0PWM6
 * Channel 2 --> PD0 --> M1PWM0
 * Channel 3 --> PF0 --> M1PWM4
 * Channel 4 --> PF2 --> M1PWM6
 * Channel 5 --> PE4 --> M1PWM2
 *
 */
void initIRChannel(struct sensor * sensor) {
  volatile unsigned long delay;
	static uint8_t nextIRChannel = 0;
	
	if (nextIRChannel == NUM_PWM_CHANNELS) {
		terminal_printString("Error: Unsupported number of PWM channels\r\n");
		return;
	}

	// Initialize pin based on next available PWM channel.
	switch (nextIRChannel) {
		case 0:
			// Initialize PWM0 and Port B
			SYSCTL_RCGCPWM_R |= 0x01;
			SYSCTL_RCGCGPIO_R |= 0x02;
			delay = SYSCTL_RCGCGPIO_R; // Noop to allow time to finish init'ing

			// General PWM initialization
			SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // Use PWM divider
			SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; // Clear PWM divider field
			SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2; // Configure for /2 divider
		
			// PB6
			GPIO_PORTB_AFSEL_R |= 0x40; // Enable alternate function for PBP6
			GPIO_PORTB_PCTL_R &= ~0xF000000; // Clear PB6
			GPIO_PORTB_PCTL_R |= 0x4000000; // Configure PB6 as M0PWM2
			GPIO_PORTB_AMSEL_R &= ~0x40; // Disable analog functionality on PB6
			GPIO_PORTB_DEN_R |= 0x40; // Enable digital I/O on PB6
		
			// M0PWM0 maps to PWM0_0
			PWM0_0_CTL_R = 0; // Re-loading down-counting mode
			PWM0_0_GENA_R = (PWM_0_GENA_ACTCMPBD_ONE|PWM_0_GENA_ACTLOAD_ZERO);
			PWM0_0_LOAD_R = PWM_PERIOD - 1; // Cycles needed to count down (goes low on load)
			PWM0_0_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
			PWM0_0_CTL_R |= 0x00000001; // Start PWM0 0
			PWM0_ENABLE_R |= 0x1; // Enable PB6/M0PWM0
			break;
		
		case 1:
			// Initialize Port C 
			SYSCTL_RCGCGPIO_R |= 0x04;
			delay = SYSCTL_RCGCGPIO_R; // Noop to allow time to finish init'ing
		
			// PC4
			GPIO_PORTC_AFSEL_R |= 0x10; // Enable alternate function for PC4
			GPIO_PORTC_PCTL_R &= ~0xF0000; // Clear PC4
			GPIO_PORTC_PCTL_R |= 0x40000; // Configure PC4 as M0PWM6
			GPIO_PORTC_AMSEL_R &= ~0x10; // Disable analog functionality on PC4
			GPIO_PORTC_DEN_R |= 0x10; // Enable digital I/O on PC4
		
			// M0PWM6 maps to PWM0_3
			PWM0_3_CTL_R = 0; // Re-loading down-counting mode
			PWM0_3_GENA_R = (PWM_3_GENA_ACTCMPBD_ONE|PWM_3_GENA_ACTLOAD_ZERO);
			PWM0_3_LOAD_R = PWM_PERIOD - 1; // Cycles to count down (goes low on load)
			PWM0_3_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
			PWM0_3_CTL_R |= 0x00000001; // Start PWM0 3
			PWM0_ENABLE_R |= 0x40; // Enable PC4/M0PWM6
			break;
		
		case 2:
			// Initialize PWM1 and Port D
			SYSCTL_RCGCPWM_R |= 0x2;
			SYSCTL_RCGCGPIO_R |= 0x8;
			delay = SYSCTL_RCGCGPIO_R; // Noop to allow time to finish init'ing
		
			// PD0
			GPIO_PORTD_AFSEL_R |= 0x1; // Enable alternate function for PB4
			GPIO_PORTD_PCTL_R &= ~0xF; // Clear PD0
			GPIO_PORTD_PCTL_R |= 0x5; // Configure PD0 as M1PWM0
			GPIO_PORTD_AMSEL_R &= ~0x1; // Disable analog functionality on PD0
			GPIO_PORTD_DEN_R |= 0x1; // Enable digital I/O on PD0
		
			// M1PWM0 maps to PWM1_0
			PWM1_0_CTL_R = 0; // Re-loading down-counting mode
			PWM1_0_GENA_R = (PWM_1_GENA_ACTCMPBD_ONE|PWM_1_GENA_ACTLOAD_ZERO);
			PWM1_0_LOAD_R = PWM_PERIOD - 1; // Cycles to count down (goes low on load)
			PWM1_0_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
			PWM1_0_CTL_R |= 0x00000001; // Start PWM1 0
			PWM1_ENABLE_R |= 0x1; // Enable PD0/M1PWM0
			break;
		
		case 3:
			// Initialize Port F
			SYSCTL_RCGCGPIO_R |= 0x20;
			delay = SYSCTL_RCGCGPIO_R; // Noop to allow time to finish init'ing
			
			// Unlock Port F pins (special case)
			GPIO_PORTF_LOCK_R = 0x4C4F434B; // Unlock GPIO Port F
		
			// PF0
			GPIO_PORTF_CR_R = 0x1; // Allow changes to PF0
			GPIO_PORTF_AFSEL_R |= 0x1; // Enable alternate function for PF0
			GPIO_PORTF_PCTL_R &= ~0xF; // Clear PF0
			GPIO_PORTF_PCTL_R |= 0x5; // Configure PF0 as M1PWM4
			GPIO_PORTF_AMSEL_R &= ~0x1; // Disable analog functionality on PF0
			GPIO_PORTF_DEN_R |= 0x1; // Enable digital I/O on PF0
		
			// M1PWM4 maps to PWM1_2
			PWM1_2_CTL_R = 0; // Re-loading down-counting mode
			PWM1_2_GENA_R = (PWM_1_GENA_ACTCMPBD_ONE|PWM_1_GENA_ACTLOAD_ZERO);
			PWM1_2_LOAD_R = PWM_PERIOD - 1; // Cycles to count down (goes low on load)
			PWM1_2_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
			PWM1_2_CTL_R |= 0x00000001; // Start PWM1 2
			PWM1_ENABLE_R |= 0x10; // Enable PF0/M1PWM4 
			break;
		
		case 4:
			// PF2
			GPIO_PORTF_CR_R = 0x4; // Allow changes to PF2
			GPIO_PORTF_AFSEL_R |= 0x4; // Enable alternate function for PF2
			GPIO_PORTF_PCTL_R &= ~0xF00; // Clear PF2
			GPIO_PORTF_PCTL_R |= 0x500; // Configure PF2 as M1PWM6
			GPIO_PORTF_AMSEL_R &= ~0x4; // Disable analog functionality on PF2
			GPIO_PORTF_DEN_R |= 0x4; // Enable digital I/O on PF2
		
			// M1PWM6 maps to PWM1_3
			PWM1_3_CTL_R = 0; // Re-loading down-counting mode
			PWM1_3_GENA_R = (PWM_1_GENA_ACTCMPBD_ONE|PWM_1_GENA_ACTLOAD_ZERO);
			PWM1_3_LOAD_R = PWM_PERIOD - 1; // Cycles to count down (goes low on load)
			PWM1_3_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
			PWM1_3_CTL_R |= 0x00000001; // Start PWM1 2
			PWM1_ENABLE_R |= 0x40; // Enable PF2/M1PWM6 
			break;

		case 5:
			// Init Port E
			SYSCTL_RCGCGPIO_R |= 0x10;
			delay = SYSCTL_RCGCGPIO_R; // Noop to allow time to finish init'ing
		
			// PE4
			GPIO_PORTE_CR_R = 0x4; // Allow changes to PE4
			GPIO_PORTE_AFSEL_R |= 0x10; // Enable alternate function for PE4
			GPIO_PORTE_PCTL_R &= ~0xF0000; // Clear PE4
			GPIO_PORTE_PCTL_R |= 0x50000; // Configure PE4 as M1PWM2
			GPIO_PORTE_AMSEL_R &= ~0x10; // Disable analog functionality on PE4
			GPIO_PORTE_DEN_R |= 0x10; // Enable digital I/O on PE4
		
			// M1PWM2 maps to PWM1_1
			PWM1_1_CTL_R = 0; // Re-loading down-counting mode
			PWM1_1_GENA_R = (PWM_1_GENA_ACTCMPBD_ONE|PWM_1_GENA_ACTLOAD_ZERO);
			PWM1_1_LOAD_R = PWM_PERIOD - 1; // Cycles to count down (goes low on load)
			PWM1_1_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
			PWM1_1_CTL_R |= 0x00000001; // Start PWM1 1
			PWM1_ENABLE_R |= 0x4; // Enable PE4/M1PWM2 
			break;
		
		default:
			terminal_printString("Error: Unsupported number of IR sensors\r\n");
			return;
	}
	
	// Set sensor's channel and increment.
	sensor->channel = nextIRChannel++;
}

/**
 * Currently only supports one Ping sensor on PA2 and Timer0.
 */ 
void initPingChannel(struct sensor * sensor) {
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
 * Based on sensor value, determine and update voltage on its pin.
 */
void Sensors_UpdateOutput(struct car * car) {
	int i;
	struct sensor * sensor;	
	uint16_t duty;
	
	// Sensor number maps to a pwm (see Sensor_Init).
	// Using sensor.val (distance to wall), use sensor's val --> voltage mapping
	// to set PWM.
	for (i = 0; i < car->numSensors; i++) {
		sensor = &car->sensors[i];
		switch (sensor->type) {
			case S_IR:
			  duty = getDutyFromIRSensorVal(sensor->val);
				setIRChannelDuty(sensor->channel, duty);
				break;
			case S_TEST:
			case S_PING:
				CurPingPeriod = getPeriodFromPingSensorVal(sensor->val);
				break;
			default:
				terminal_printString("Error: Unsupported sensor type\r\n");
				continue;	
		}
	}
}

/**
 * Given value (distance from closest object in path of sensor), determine
 * duty value to pass PWM. Duty is % time low (resolution .1%).
 */
uint16_t getDutyFromIRSensorVal(uint32_t val) {
	// todo
	return 500; // 50.0%
}

uint32_t getPeriodFromPingSensorVal(uint32_t val) {
	// todo
	return 8000;
}

/**
 * Used to test pwm outputs. Duty cycle is % of time low (resolution .1%).
 */
void setIRChannelDuty(uint8_t channel, uint16_t duty) {
	uint32_t duty_ticks;
	
	if (duty > 1000) {
		terminal_printString("Error: Duty above max \r\n");
		return;
	}
	
	if (channel >= NUM_PWM_CHANNELS) {
		terminal_printString("Error: PWM channel not supported \r\n");
		return;
	}
	
	duty_ticks = PWM_PERIOD - PWM_PERIOD * duty / 1000 - 1;
	
	// 100% duty isn't supported, get next best thing.
	if (duty_ticks == PWM_PERIOD - 1) {
		duty_ticks = PWM_PERIOD - 2;
	}
	
	// Set count value when output rises
	switch (channel) {
		case 0:
			PWM0_0_CMPB_R = duty_ticks;
			break;
	
		case 1:
			PWM0_3_CMPB_R = duty_ticks;
			break;
		
		case 2:
			PWM1_0_CMPB_R = duty_ticks;
			break;
		
		case 3:
			PWM1_2_CMPB_R = duty_ticks;
			break;
		
		case 4:
			PWM1_3_CMPB_R = duty_ticks;
			break;
		
		case 5:
			PWM1_1_CMPB_R = duty_ticks;
			break;
		
		default:
			terminal_printString("Error: Unsupported PWM channel\r\n");
			return;
	}
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
