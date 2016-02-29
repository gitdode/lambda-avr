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
#include "command.h"
#include "interrupts.h"
#include "sensors.h"
#include "display.h"
#include "alert.h"
#include "pins.h"
#include "airgate.h"

static volatile bool buttonPressed = false;
static volatile uint32_t ints = 0;

/**
 * Called about every 1/32 seconds.
 */
ISR(TIMER0_COMPA_vect) {
	if (! isSimulation()) {
		ints++;
	}
	oscillateBeep();
	if (bit_is_clear(PIN, PIN_BUTTON) && ! buttonPressed) {
		buttonPressed = true;
		cycleDisplay();
	} else if (bit_is_set(PIN, PIN_BUTTON)) {
		buttonPressed = false;
	}
}

ISR(TIMER2_COMPA_vect) {
	makeSteps();
}

uint32_t getTime(void) {
	uint32_t time;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		// 32 ints per second
		time = ints >> 5;
	}

	return time;
}

void addTime(uint32_t const time) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		ints += (time << 5);
	}
}

void formatTime(char* const str, size_t const size) {
	uint32_t time = getTime();
	uint16_t hours = time / 3600;
	uint8_t mins = time % 3600 / 60;
	uint8_t secs = time % 60;

	snprintf(str, size, "%5u:%02u:%02u", hours, mins, secs);
}

void resetTime(void) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		ints = 0;
	}
}

void setupPorts(void) {
	// pull-up resistor for the menu button
	PORT |= (1 << PIN_BUTTON);

	// enable beep output pin
	DDR |= (1 << PIN_BEEPER);

	// enable oxygen sensor heater control output pin
	DDR |= (1 << PIN_HEATER);

	// enable output pins for stepper motor driver
	DDR |= (1 << PIN_SLEEP);
	DDR |= (1 << PIN_STEP);
	DDR |= (1 << PIN_DIR);
	DDR |= (1 << PIN_CURRENT);
}

void setupSleepMode(void) {
	// SLEEP_MODE_ADC corrupts USART TX/RX data and causes a click in the beep
	set_sleep_mode(SLEEP_MODE_IDLE);
}

void initInterrupts(void) {
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);

	// enable timer0 compare match A interrupt
	TIMSK0 |= (1 << OCIE0A);

	// enable timer2 compare match A interrupt
	TIMSK2 |= (1 << OCIE2A);

	// enable USART RX complete interrupt 0
	UCSR0B |= (1 << RXCIE0);

	// enable global interrupts
	sei();
}

void initTimers(void) {
	// timer0 is for time: about 32 ints per second
	// timer0 clear timer on compare match mode, TOP OCR0A
	TCCR0A |= (1 << WGM01);
	TCCR0B |= TIMER0_PRESCALE;
	OCR0A = TIMER0_COMP_MATCH;

	// timer1 is for the beeper square wave
	// timer1 clear timer on compare match mode, TOP OCR1A
	TCCR1B |= (1 << WGM12);
	TCCR1B |= TIMER1_PRESCALE;
	OCR1A = TIMER1_COMP_MATCH;

	// timer2 is for the step of the stepper motor using the DRV8825
	// timer2 clear timer on compare match mode, TOP OCR2A
	TCCR2A |= (1 << WGM21);
	TCCR2B = 0;
}
