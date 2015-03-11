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
#include <util/delay.h>
#include "USART.h"
#include "adc.h"
#include "sensors.h"
#include "integers.h"

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
