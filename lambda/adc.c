/*
 * adc.c
 *
 * Functions to set up the ADC and get voltages from its ports.
 *
 *  Created on: 11.03.2015
 *      Author: dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU Lesser General Public License Version 2.1 (the "LGPL License"), in which
 * case the provisions of the LGPL License are applicable instead of those
 * above.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU (Lesser) General Public License for more details.
 *
 * DISCLAIMER: I'm experienced with Java and I like C, but I'm new here.
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "adc.h"

// TODO what exactly does this do?
EMPTY_INTERRUPT(ADC_vect);

void setupADC(void) {
	// use AVCC as reference voltage
	ADMUX |= (1 << REFS0);
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

int16_t getVoltage(uint8_t port) {

	ADMUX = (0b11110000 & ADMUX) | port;

	uint32_t overValue = 0;
	for (uint8_t i = 0; i < 16; i++) {
		sleep_mode();
		overValue += ADC;
	}
	int16_t mV = (((overValue >> 2) * AREF_MV) >> 12) + ADC_OFFSET_MV;

	return mV;
}
