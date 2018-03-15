/**	
 * File: Sensors.c
 * Author: Sarah Masimore
 * Last Updated Date: 03/14/2018
 * Description: Manages init'ing sensor ports and pins, mapping environment
 * 							output to voltage out on pin to racecar. Uses Ports B and C.
 */

#include "Simulator.h"
#include "Sensors.h"
#include "tm4c123gh6pm.h"
#include "UART.h"

#define PWM_PERIOD 8000 // 1000 Hz

uint8_t SensorsInitialized = 0;

/**
 * Init ports and relevant pins. Currently supports up to 6 sensors. PWM on
 * TM4C supports up to 8 different PWM outputs. 
 *
 * Note: PWM output pins come in pairs with same output. Need to be careful
 * which pin is chosen.
 *
 * S0 --> PB4 --> M0PWM2
 * S1 --> PB6 --> M0PWM0
 * S2 --> PC4 --> M0PWM6
 * S3 --> PD0 --> M1PWM0
 * S4 --> PF0 --> M1PWM4
 * S5 --> PF2 --> M1PWM6
 *
 */
void Sensors_Init(struct car * car) {
  volatile unsigned long delay;
	uint16_t numSensors = car->numSensors;
	int i;

	// LEFT OFF: 
	// 1) Setting up ports and pins. Got pb6 to work, test pb4 and having 2 
	// 		diff values. 
	// 2) ADC initialization.
	// 3) Simulator math
	
	// Based on sensor numbers, init relevant pins.
	for (i = 0; i < numSensors; i++) {
		// Initialize pwm pin based on sensor number.
		switch (i) {
			case 0:
				// Initialize PWM0 and Port B
				SYSCTL_RCGCPWM_R |= 0x01;
				SYSCTL_RCGCGPIO_R |= 0x02;
				delay = SYSCTL_RCGCGPIO_R; // Noop to allow time to finish init'ing

				// General PWN initialization
				SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // Use PWM divider
				SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; // Clear PWM divider field
				SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2; // Configure for /2 divider
			
				// PB4
				GPIO_PORTB_AFSEL_R |= 0x10; // Enable alternate function for PB4
				GPIO_PORTB_PCTL_R &= ~0xF0000; // Clear PB4
				GPIO_PORTB_PCTL_R |= 0x40000; // Configure PB4 as M0PWM2
				GPIO_PORTB_AMSEL_R &= ~0x10; // Disable analog functionality on PB4
				GPIO_PORTB_DEN_R |= 0x10; // Enable digital I/O on PB4
			
				// M0PWM2 maps to PWM0_1
				PWM0_1_CTL_R = 0; // Re-loading down-counting mode
				PWM0_1_GENA_R = (PWM_1_GENA_ACTCMPBD_ONE|PWM_1_GENA_ACTLOAD_ZERO);
				PWM0_1_LOAD_R = PWM_PERIOD - 1; // Cycles to count down (goes low on load)
				PWM0_1_CMPB_R = PWM_PERIOD / 2; // Count value when output rises
				PWM0_1_CTL_R |= 0x00000001; // Start PWM0 1
				PWM0_ENABLE_R |= 0x4; // Enable PB4/M0PWM2
				break;
				
			case 1:
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
				PWM0_0_CMPB_R = PWM_PERIOD / 3; // Count value when output rises
				PWM0_0_CTL_R |= 0x00000001; // Start PWM0 0
				PWM0_ENABLE_R |= 0x1; // Enable PB6/M0PWM0
				break;
			
			case 2:
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
			
			case 3:
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
			
			case 4:
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
			
			case 5:
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
			
			default:
				UART_OutString("Error: Unsupported sensor number\r\n");
		}
	}
}

/**
 * Based on sensor value, determine and update voltage on its pin.
 */
void Sensors_UpdateVoltages(struct car * car) {
	
}
