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
#include "interrupts.h"
#include "sensors.h"
#include "display.h"
#include "alert.h"

static volatile bool buttonPressed = false;
static volatile bool usartReceived = false;
static volatile uint8_t intCount = 0;

static char usartData[64];

/**
 * Called every 16 ms.
 */
ISR(TIMER0_OVF_vect) {
	intCount++;
	oscillateBeep();
	if (bit_is_clear(PINB, PB0) && ! buttonPressed) {
		buttonPressed = true;
		cycleDisplay();
	} else if (bit_is_set(PINB, PB0)) {
		buttonPressed = false;
	}
}

/**
 * Called when data was received via USART.
 */
ISR(USART_RX_vect) {
	if (bit_is_set(UCSR0A, RXC0) && ! usartReceived) {
		char data = UDR0;
		size_t length = strlen(usartData);
		if (length < sizeof(usartData) - 1 && data != '\n' && data != '\r') {
			usartData[length] = data;
		} else {
			usartData[length] = '\0';
			usartReceived = true;
		}
	}
}

bool isUSARTReceived(void) {
	return usartReceived;
}

void getUSARTData(char* const data, size_t const size) {
	if (size > 0) {
	    data[0] = '\0';
	    strncat(data, usartData, size - 1);
	}
	memset(usartData, 0, sizeof(usartData));
	usartReceived = false;
}

bool hasIntCount(uint8_t const count, bool const reset) {
	if (intCount >= count) {
		if (reset) {
			intCount = 0;
		}
		return true;
	} else {
		return false;
	}
}

void setupPorts(void) {
	// pull-up resistor for the mouton
	PORTB |= (1 << PB0);

	// enable beep output pin
	DDRB |= (1 << PB1);
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
	// timer0 clock prescaler/64 = 15.625 kHz overflowing every 16 ms
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
