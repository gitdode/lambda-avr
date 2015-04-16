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
	// INT0 and INT1 are assigned to the LCD, a bit of a shame
	PORTB |= (1 << PB0);

	// PB1 as output
	DDRB |= (1 << PB1);
}

void setupSleepMode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
}

void initInterrupts(void) {
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	// enable PC interrupts
	// PCICR |= (1 << PCIE0);
	// PCMSK0 |= (1 << PB0);

	// enable timer 0 overflow interrupt
	TIMSK0 |= (1 << TOIE0);

	// enable global interrupts
	sei();
}

void initTimers(void) {
	// timer in normal mode is default
	// timer clock prescaler /64 = 15.625 kHz overflowing every 16.2 ms
	TCCR0B |= (1 << CS00) | (1 << CS01);
}
