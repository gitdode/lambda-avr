/*
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
#include "lambda.h"

#define AREF_MV 4850
#define ADC_OFFSET_MV 7
#define TMP_OP_OFFSET 441

static const char* lean = "Mager";
static const char* ideal = "Ideal";
static const char* rich = "Fett!";

/**
 * Table used to look up the lambda value at 12 V heater voltage
 * and 220°C exhaust gas temperature. Most values are approximated
 * from the characteristic curve in the datasheet.
 * TODO equation? real table?
 */
static const tableEntry lambdaTable[] = {
	{ 4, 2.0 },
	{ 5, 1.9 },
	{ 6, 1.8 },
	{ 8, 1.7 },
	{ 10, 1.6 },
	{ 12, 1.5 },
	{ 15, 1.4 },
	{ 20, 1.3 },
	{ 28, 1.2 },
	{ 40, 1.1 },
	{ 68, 1.025 },
	{ 400, 1.0 },
	{ 800, 0.98 },
	{ 860, 0.9 },
    { 880, 0.8 }
};

static const tableEntry tempOTable[] = {
	{ -57, 10.22 }, // -50 C
	{ 455, 9.97 },  // 0 C
	{ 1403, 9.49 }, // 100 C
	{ 2264, 9.05 }, // 200 C
	{ 3047, 8.64 }, // 300 C
	{ 3762, 8.27 }  // 400 C
};

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

int main(void) {

	initUSART();
	setupADC();
	setupSleepMode();

	// disable digital input on ADC0
	// http://www.openmusiclabs.com/learning/digital/atmega-adc/
	// DIDR0 = 0b00000011;

	// initial update
	update(0.0, 0.0, 0.0);

	// main loop
	while (1) {
		run();
		_delay_ms(1000);
	}

	return 0;
}

void run(void) {
	float lambdaVoltage = getVoltage(PC2) / 11.0;
	lambdaVoltageAvg = (lambdaVoltage + lambdaVoltageAvg * 8) / 9;

	int tempIVoltage = getVoltage(PC5);
	tempIVoltageAvg = (tempIVoltage + tempIVoltageAvg * 2) / 3;

	int tempOVoltage = getVoltage(PC0);
	tempOVoltageAvg = (tempOVoltage + tempOVoltageAvg * 2) / 3;

	update(tempIVoltageAvg, tempOVoltageAvg, lambdaVoltageAvg);
}

void update(float tempIVoltage, float tempOVoltage, float lambdaVoltage) {
	int tempI = toTempI(tempIVoltage);
	int tempO = toTempO(tempOVoltage);
	int length = sizeof(lambdaTable) / sizeof(lambdaTable[0]);
	float lambda = lookupLinInter(lambdaVoltage, lambdaTable, length);
	// round(lambda * 10) / 10.0;

	display(tempIVoltage, tempI, tempOVoltage, tempO, lambdaVoltage, lambda);
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

int getVoltage(int port) {

	ADMUX = (0b11110000 & ADMUX) | port;

	unsigned long overValue = 0;
	for (int i = 0; i < 16; i++) {
		sleep_mode();
		overValue += ADC;
	}
	float mV = ((overValue >> 2) * AREF_MV / 4096) + ADC_OFFSET_MV;

	return mV;
}

/**
 * Returns the temperature for the given voltage of a type K thermocouple
 * amplified with an AD8495 (5 mV/°C). Type K thermocouple voltages are
 * about linear between 0 and 800°C. Returns 0 for negative voltages.
 */
int toTempI(float mV) {
	if (mV < 0) {
		return 0;
	}

	int temp = round(mV / 5);

	return temp;
}

int toTempO(float mV) {
	if (mV < 0) {
		return 0;
	}

	int length = sizeof(tempOTable) / sizeof(tempOTable[0]);
	float mVPerC = lookupLinInter(mV, tempOTable, length) / 5000 * AREF_MV;
	int temp = round((mV - TMP_OP_OFFSET) / mVPerC);

	char buf[13];
	dtostrf(mVPerC, 5, 2, buf);
	char str[14];
	snprintf(str, sizeof(str), "mV/C %s\r\n", buf);
	printString(str);

	return temp;
}

/**
 * Returns the value corresponding to the given voltage
 * from the lookup table using linear interpolation.
 * Thanks to http://stackoverflow.com/a/7091629/709426 and
 * http://en.wikipedia.org/wiki/Linear_interpolation
 */
float lookupLinInter(float mV, const tableEntry table[], int length) {
	if (mV < table[0].mV) {
		return table[0].value;
	} else if (mV > table[length - 1].mV) {
		return table[length - 1].value;
	}

	int i = 0;
	for (; i < length - 1; i++) {
		if (table[i + 1].mV > mV) {
			break;
		}
	}

	float diffVoltage = table[i + 1].mV - table[i].mV;
	float diffValue = table[i + 1].value - table[i].value;
	float value = table[i].value +
			(mV - table[i].mV) * diffValue / diffVoltage;

	return value;
}

const char* toInfo(float lambda) {
	if (lambda > 1.5) {
		return lean;
	} else if (lambda > 1.3 && lambda <= 1.5) {
		return ideal;
	} else {
		return rich;
	}
}

