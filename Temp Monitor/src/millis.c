/*
 * millis.c
 *
 * Created: 8/24/2021 12:51:28 AM
 *  Author: Ben
 */ 
#include "millis.h"
#include <asf.h>

#include <clock.h>

volatile uint32_t MS_Timer = 0;

void SysTick_Handler(void) {
	MS_Timer++;                // Increment global millisecond timer
}

void millis_init(void) {
	// Configure SysTick to trigger every millisecond using the CPU Clock
	SysTick->CTRL = 0;                    // Disable the SysTick Module
	SysTick->LOAD = 47971UL;                // Set the Reload Register for 1mS interrupts
	NVIC_SetPriority(SysTick_IRQn, 3);    // Set the interrupt priority to least urgency
	SysTick->VAL = 0;                    // Clear the Current Value register
	SysTick->CTRL = 0x00000007;        // Enable SysTick, Enable SysTick Exceptions, Use CPU Clock
	NVIC_EnableIRQ(SysTick_IRQn);        // Enable the SysTick Interrupt (Global)
}

uint32_t millis(void) {
	return MS_Timer;
}

void delay(uint16_t mS) {
	uint32_t start = millis();
	for(uint16_t i = start; millis() < start + mS;) nop();
}