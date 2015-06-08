/*
 * interrupts.c
 *
 * ISRs and functions to handle and set up interrupts and timers.
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include "interrupts.h"
#include "sensors.h"
#include "display.h"
#include "alert.h"

static volatile bool buttonPressed = false;
static volatile uint32_t ints = 0;

/**
 * Called about every 16.4 ms.
 */
ISR(TIMER0_OVF_vect) {
	ints++;
	oscillateBeep();
	if (bit_is_clear(PINB, PB0) && ! buttonPressed) {
		buttonPressed = true;
		cycleDisplay();
	} else if (bit_is_set(PINB, PB0)) {
		buttonPressed = false;
	}
}

uint32_t getInts(void) {
	uint32_t atomicInts;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		atomicInts = ints;
	}
	return atomicInts;
}

uint32_t getTime(void) {
	return getInts() / INTS_PER_SEC;
}

void resetTime(void) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		ints = 0;
	}
}

void setupPorts(void) {
	// pull-up resistor for the mouton
	PORTB |= (1 << PB0);

	// enable beep output pin
	DDRB |= (1 << PB1);

	// enable oxygen sensor heating control output pin
	DDRB |= (1 << PB2);
}

void setupSleepMode(void) {
	// SLEEP_MODE_ADC corrupts USART TX/RX data and causes a click in the beep
	set_sleep_mode(SLEEP_MODE_IDLE);
}

void initInterrupts(void) {
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	// enable timer 0 overflow interrupt
	TIMSK0 |= (1 << TOIE0);

	// enable USART RX complete interrupt 0
	UCSR0B |= (1 << RXCIE0);
	// enable data register empty interrupt 0
	// UCSR0B |= (1 << UDRIE0);

	// enable global interrupts
	sei();
}

void initTimers(void) {
	// timer in normal mode is default
	// timer0 clock prescaler/64 = 15.625 kHz overflowing every 16.4 ms
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// timer1 Clear Timer on Compare Match mode, TOP OCR1A
	TCCR1B |= (1 << WGM12);
	// timer1 clock prescaler/8
	TCCR1B |= (1 << CS11);
	// toggles PB1 at 7.8 kHz generating a 3.9 kHz beep
	// OCR1A = 15;
	// 2 kHz is less noisy on the small piezo beeper
	OCR1A = 31;
}
