/*
 * lambda.c
 *
 *  Created on: 22.02.2015
 *      Author: dode@luniks.net
 *
 * TODO comments, attribution
 * TODO DIDR?
 * TODO How to round integer divisions half up?
 * TODO string.h?
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

int16_t lambdaVoltageAvg = 0;
int16_t tempIVoltageAvg = 0;
int16_t tempOVoltageAvg = 0;

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
		int16_t tempIVoltage, int16_t tempI,
		int16_t tempOVoltage, int16_t tempO,
		int16_t lambdaVoltage, int16_t lambda) {
	div_t lambdaT = div(lambda, 1000);

	// TODO chars per line 16
	char line0[40];
	char line1[40];
	snprintf(line0, sizeof(line0), "Ti %3d C %d   To %3d C %d\r\n", tempI, tempIVoltage, tempO, tempOVoltage);
	snprintf(line1, sizeof(line1), "L %d.%03d %d\r\n", lambdaT.quot, lambdaT.rem, lambdaVoltage);
	printString(line0);
	printString(line1);
}

int16_t average(int16_t value, int16_t average, uint8_t weight) {
	return (value + (average * weight) + weight) / (weight + 1);
}

void measure(void) {
	int16_t tempIVoltage = getVoltage(PC5);
	tempIVoltageAvg = average(tempIVoltage, tempIVoltageAvg, 4);

	int16_t tempOVoltage = getVoltage(PC0);
	tempOVoltageAvg = average(tempOVoltage, tempOVoltageAvg, 4);

	// OP factor is 11
	int16_t lambdaVoltage = (getVoltage(PC2) + 5) / 11;
	lambdaVoltageAvg = average(lambdaVoltage, lambdaVoltageAvg, 4);

	int16_t tempI = toTempI(tempIVoltageAvg);
	int16_t tempO = toTempO(tempOVoltageAvg);
	int16_t lambda = toLambda(lambdaVoltageAvg);

	display(tempIVoltageAvg, tempI, tempOVoltageAvg, tempO, lambdaVoltageAvg, lambda);
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
