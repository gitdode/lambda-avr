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
}

void setupSleepMode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
}

void initInterrupts(void) {
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	// enable timer 0 overflow interrupt
	TIMSK0 |= (1 << TOIE0);

	// enable global interrupts
	sei();
}

void initTimers(void) {
	// timer in normal mode is default
	// timer clock prescaler/64 = 15.625 kHz overflowing every 16 ms
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// toggle pin PB1 on compare match
	TCCR1A |= (1 << COM1A0);
	// CTC mode, TOP OCR1A
	TCCR1B |= (1 << WGM12);
	// timer clock prescaler/8
	TCCR1B |= (1 << CS11);
	// toggles PB1 at 3.9 kHz generating a 1.95 kHz beep
	OCR1A = 32;
}
