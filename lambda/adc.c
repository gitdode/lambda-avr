/*
 * adc.c
 *
 * Functions to set up the ADC and get voltages from its pins.
 *
 *  Created on: 11.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "adc.h"

// set up a fake ISR to just return to where sleep_mode() was called
// when ADC conversion is complete (ADC_vect)
EMPTY_INTERRUPT(ADC_vect);

void setupADC(void) {
	// use AVCC as reference voltage
	ADMUX |= (1 << REFS0);
	// disable digital input on the ADC inputs
	// http://www.openmusiclabs.com/learning/digital/atmega-adc/
	DIDR0 |= (1 << PC0) | (1 << PC2) | (1 << PC5);
	// ADC clock prescaler/8
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	// enable ADC
	ADCSRA |= (1 << ADEN);
}

void setupSleepMode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);
	// enable global interrupts
	sei();
}

uint16_t getVoltage(uint8_t pin) {

	ADMUX = (0b11110000 & ADMUX) | pin;

	uint32_t overValue = 0;
	for (uint8_t i = 0; i < 16; i++) {
		sleep_mode();
		overValue += ADC;
	}
	int16_t mV = (((overValue >> 2) * AREF_MV) >> 12) + ADC_OFFSET_MV;

	return mV;
}
