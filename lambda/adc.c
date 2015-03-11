/*
 * adc.c
 *
 *  Created on: 11.03.2015
 *      Author: dode@luniks.net
 *
 */
#include <avr/interrupt.h>
#include <avr/sleep.h>

// TODO right place for these definitions? Put in makefile?
// #define AREF_MV 4850
#define AREF_MV 5000
#define ADC_OFFSET_MV 7
// #define TEMPO_OP_OFFSET_MV 441
#define TEMPO_OP_OFFSET_MV 454

EMPTY_INTERRUPT(ADC_vect);

void setupADC(void) {
	ADMUX |= (1 << REFS0); // use AVCC as reference voltage
	// ADCSRA |= (1 << ADPS1) | (1 << ADPS2); // ADC clock prescaler /64
	ADCSRA |= (1 << ADPS2); // ADC clock prescaler /16
	ADCSRA |= (1 << ADEN); // enable ADC
}

void setupSleepMode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
	ADCSRA |= (1 << ADIE); // enable ADC interrupt
	sei(); // enable global interrupts
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
