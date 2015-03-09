/*
 * lambda.c
 *
 *  Created on: 22.02.2015
 *      Author: dode@luniks.net
 *
 * TODO attribution
 * TODO have a look at string.h
 */
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "USART.h"
#include "sensors.h"
#include "integers.h"

EMPTY_INTERRUPT(ADC_vect);

/**
 * Sets up reference voltage and clock prescaler of the ADC and enables it.
 */
void setupADC(void) {
	ADMUX |= (1 << REFS0); // use AVCC as reference voltage
	// ADCSRA |= (1 << ADPS1) | (1 << ADPS2); // ADC clock prescaler /64
	ADCSRA |= (1 << ADPS2); // ADC clock prescaler /16
	ADCSRA |= (1 << ADEN); // enable ADC
}

/**
 * Sets up sleep mode and enables ADC and global interrupts.
 */
void setupSleepMode(void) {
	set_sleep_mode(SLEEP_MODE_ADC);
	ADCSRA |= (1 << ADIE); // enable ADC interrupt
	sei(); // enable global interrupts
}

/**
 * Initializes the USART transmitter and receiver, sets up the ADC
 * and sleep mode and then infinitely measures with a 1 second delay
 * in between.
 * TODO DIDR?
 * TODO replace delay by an interrupt or something else more efficient?
 */
int main(void) {

	initUSART();
	setupADC();
	setupSleepMode();

	// disable digital input on ADC0
	// http://www.openmusiclabs.com/learning/digital/atmega-adc/
	// DIDR0 = 0b00000011;

	// main loop
	while (1) {
		measure();
		_delay_ms(1000);
	}

	// will never be reached
	return 0;
}
