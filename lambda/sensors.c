/*
 * sensors.c
 *
 * Functions to evaluate measurements from the sensors.
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "USART.h"
#include "adc.h"
#include "sensors.h"
#include "integers.h"
#include "lcdroutines.h"

/**
 * Table used to look up the lambda value at 12 V heater voltage
 * and 220°C exhaust gas temperature. Most values are approximated
 * from the characteristic curve in the data sheet.
 * TODO real data?
 */
static const tableEntry lambdaTable[] = {
	{ 4, 2000 },
	{ 5, 1900 },
	{ 6, 1800 },
	{ 8, 1700 },
	{ 10, 1600 },
	{ 12, 1500 },
	{ 15, 1400 },
	{ 20, 1300 },
	{ 28, 1200 },
	{ 40, 1100 },
	{ 68, 1025 },
	{ 400, 1000 },
	{ 800, 980 },
	{ 860, 900 },
    { 880, 800 }
};

/**
 * Table used to look up the temperature in °C at a given voltage
 * measured using a wheatstone bridge and amplified with a non-
 * inverting OP with an offset of 454 mV at 5000 mV supply voltage
 * and an amplification factor of 6.17.
 */
static const tableEntry tempOTable[] = {
	{ -57, -50 },
	{ 454, 0 },
	{ 1403, 100 },
	{ 2264, 200 },
	{ 3047, 300 },
	{ 3762, 400 }
};

/**
 * Global variables holding averaged voltages.
 */
int32_t lambdaVoltageAvg = 0;
int32_t tempIVoltageAvg = 0;
int32_t tempOVoltageAvg = 0;

/**
 * Measures the "input" and "output" temperatures and the lambda value
 * and displays the measured values.
 */
measurement measure(void) {
	int32_t tempIVoltage = getVoltage(PC5);
	tempIVoltageAvg = average((tempIVoltage << 4), tempIVoltageAvg, 4);

	int32_t tempOVoltage = getVoltage(PC0);
	tempOVoltageAvg = average((tempOVoltage << 4), tempOVoltageAvg, 4);

	// OP factor is 11
	int32_t lambdaVoltage = divRoundNearest(getVoltage(PC2), 11);
	lambdaVoltageAvg = average((lambdaVoltage << 4), lambdaVoltageAvg, 4);

	measurement meas;
	meas.tempIVoltage = divRoundNearest(tempIVoltageAvg, 16);
	meas.tempOVoltage = divRoundNearest(tempOVoltageAvg, 16);
	meas.lambdaVoltage = divRoundNearest(lambdaVoltageAvg, 16);

	meas.tempI = toTempI(meas.tempIVoltage);
	meas.tempO = toTempO(meas.tempOVoltage);
	meas.lambda = toLambda(meas.lambdaVoltage);

	return meas;
}

void display(measurement meas) {
	uint8_t lambdax10 = divRoundNearest(meas.lambda, 100);
	div_t lambdaT = div(lambdax10, 10);

	char log[64];
	snprintf(log, sizeof(log),
			"Ti %3d C %4d - To %3d C %4d - L %4d %4d\r\n",
			meas.tempI, meas.tempIVoltage, meas.tempO, meas.tempOVoltage,
			meas.lambda, meas.lambdaVoltage);
	printString(log);

	char line0[17];
	char line1[17];
	snprintf(line0, sizeof(line0), "Ti %3dC To %3dC ", meas.tempI, meas.tempO);
	snprintf(line1, sizeof(line1), "L   %d.%01d %s   ",
			lambdaT.quot, abs(lambdaT.rem), toInfo(meas.lambda));
	lcd_setcursor(0, 1);
	lcd_string(line0);
	lcd_setcursor(0, 2);
	lcd_string(line1);
}

int32_t average(int32_t value, int32_t average, uint8_t weight) {
	return divRoundNearest(value + (average * weight), weight + 1);
}

int16_t toTempI(int16_t mV) {
	int temp = divRoundNearest(mV, 5);

	return temp;
}

int16_t toTempO(int16_t mV) {
	uint8_t length = sizeof(tempOTable) / sizeof(tempOTable[0]);
	int16_t temp = lookupLinInter(mV, tempOTable, length);

	return temp;
}

int16_t toLambda(int16_t mV) {
	uint8_t length = sizeof(lambdaTable) / sizeof(lambdaTable[0]);
	int16_t lambda = lookupLinInter(mV, lambdaTable, length);

	return lambda;
}

int16_t lookupLinInter(int16_t mV, const tableEntry table[], uint8_t length) {
	if (mV < table[0].mV) {
		return table[0].value;
	} else if (mV > table[length - 1].mV) {
		return table[length - 1].value;
	}

	uint8_t i = 0;
	for (; i < length - 1; i++) {
		if (table[i + 1].mV > mV) {
			break;
		}
	}

	int16_t diffVoltage = table[i + 1].mV - table[i].mV;
	int16_t diffValue = table[i + 1].value - table[i].value;
	int16_t value = table[i].value + divRoundNearest(
			(int32_t)(mV - table[i].mV) * diffValue, diffVoltage);

	return value;
}

const char* toInfo(int16_t lambda) {
	if (lambda > 1900) {
		return LEAN;
	} else if (lambda > 1500 && lambda <= 1900) {
		return OKAY;
	} else if (lambda >= 1300 && lambda <= 1500) {
		return IDEAL;
	} else {
		return RICH;
	}
}


