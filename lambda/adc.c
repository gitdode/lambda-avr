/*
 * adc.c
 *
 * Functions to set up the ADC and get voltages from its pins.
 *
 *  Created on: 11.03.2015
 *      Author: dode@luniks.net
 *
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "adc.h"
#include "pins.h"

/**
 * Returns to where sleep_mode() was called when ADC conversion is complete.
 */
EMPTY_INTERRUPT(ADC_vect);

void setupADC(void) {
	// use AVCC as reference voltage
	ADMUX |= (1 << REFS0);
	// disable digital input on the ADC inputs
	// http://www.openmusiclabs.com/learning/digital/atmega-adc/
	DIDR0 = 0b00111111;
	// ADC clock prescaler/8
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	// enable ADC
	ADCSRA |= (1 << ADEN);
}

uint16_t getVoltage(uint8_t const pin) {
	ADMUX = (0b11110000 & ADMUX) | pin;

	uint32_t overValue = 0;
	for (uint8_t i = 0; i < 16; i++) {
		sleep_mode();
		overValue += ADC;
	}
	int16_t mV = (((overValue >> 2) * AREF_MV) >> 12);

	return mV;
}
