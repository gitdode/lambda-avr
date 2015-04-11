/*
 * interrupts.c
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "interrupts.h"

void setupPorts(void) {
	// pull-up resistor for the mouton
	PORTB |= (1 << PB0);

	// PB1 as output
	// DDRB |= (1 << PB1);
}

void setupSleepMode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
}

void initInterrupts(void) {
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	// enable PC interrupts
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PB0);

	// enable global interrupts
	sei();
}
