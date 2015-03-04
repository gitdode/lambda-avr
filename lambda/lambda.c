/*
 * lambda.c
 *
 *  Created on: 22.02.2015
 *      Author: dode@luniks.net
 *
 * TODO try starting ADC sample manually and wait for it to finish
 * TODO try to remove floating points
 * TODO refactoring - module?
 * TODO comments, attribution
 * TODO DIDR?
 * TODO unit tests, Jenkins?
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "USART.h"
#include "sensors.h"

float lambdaVoltageAvg = 0.0;
float tempIVoltageAvg = 0.0;
float tempOVoltageAvg = 0.0;

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

void display(
		int tempIVoltage, int tempI,
		int tempOVoltage, int tempO,
		float lambdaVoltage, float lambda) {
	char lambdaStr[13];
	dtostrf(lambda, 5, 3, lambdaStr);
	char lambdaVoltageStr[13];
	dtostrf(lambdaVoltage, 5, 3, lambdaVoltageStr);

	char line0[40];
	char line1[40];
	snprintf(line0, sizeof(line0), "Ti %3d C %d   To %3d C %d\r\n", tempI, tempIVoltage, tempO, tempOVoltage);
	snprintf(line1, sizeof(line1), "L %s  (%s)\r\n", lambdaStr, lambdaVoltageStr);
	printString(line0);
	printString(line1);
}

void measure(void) {
	float lambdaVoltage = getVoltage(PC2) / 11.0;
	lambdaVoltageAvg = (lambdaVoltage + lambdaVoltageAvg * 8) / 9;

	int tempIVoltage = getVoltage(PC5);
	tempIVoltageAvg = (tempIVoltage + tempIVoltageAvg * 2) / 3;

	int tempOVoltage = getVoltage(PC0);
	tempOVoltageAvg = (tempOVoltage + tempOVoltageAvg * 2) / 3;

	int tempI = toTempI(tempIVoltage);
	int tempO = toTempO(tempOVoltage);
	float lambda = toLambda(lambdaVoltage);
	// round(lambda * 10) / 10.0;

	display(tempIVoltage, tempI, tempOVoltage, tempO, lambdaVoltage, lambda);
}

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

	return 0;
}
