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
 * from the characteristic curve in the data sheet. The voltage
 * of the sensor is amplified by factor 11.
 * TODO real data?
 */
static const tableEntry lambdaTable[] = {
	{ 44, 2000 },
	{ 55, 1900 },
	{ 66, 1800 },
	{ 80, 1700 },
	{ 110, 1600 },
	{ 132, 1500 },
	{ 165, 1400 },
	{ 220, 1300 },
	{ 308, 1200 },
	{ 440, 1100 },
	{ 748, 1025 },
	{ 4400, 1000 },
	{ 8800, 980 },
	{ 9460, 900 },
	{ 9680, 800 }
};

/**
 * Table used to look up the temperature in °C at a given voltage
 * measured using a wheatstone bridge and amplified with a non-
 * inverting OP with an offset of 454 mV at 5000 mV supply voltage
 * and an amplification factor of 6.17.
 */
static const tableEntry tempOTable[] = {
	// { -57, -50 },
	{ 454, 0 },
	{ 1403, 100 },
	{ 2264, 200 },
	{ 3047, 300 },
	{ 3762, 400 }
};

/**
 * Global variables holding averaged voltages.
 */
uint32_t lambdaVoltageAvg = 4;
uint32_t tempIVoltageAvg = 4;
uint32_t tempOVoltageAvg = 4;

/**
 * Measures the "input" and "output" temperatures and the lambda value,
 * calculates an exponential moving average and displays the translated values.
 */
measurement measure(void) {
	uint32_t tempIVoltage = getVoltage(PC5);
	tempIVoltageAvg = tempIVoltage + tempIVoltageAvg -
			((tempIVoltageAvg - 4) >> 3);

	uint32_t tempOVoltage = getVoltage(PC0);
	tempOVoltageAvg = tempOVoltage + tempOVoltageAvg -
			((tempOVoltageAvg - 4) >> 3);

	// OP factor is 11
	uint32_t lambdaVoltage = getVoltage(PC2);
	lambdaVoltageAvg = lambdaVoltage + lambdaVoltageAvg -
			((lambdaVoltageAvg - 4) >> 3);

	measurement meas;
	meas.tempIVoltage = (tempIVoltageAvg >> 3);
	meas.tempOVoltage = (tempOVoltageAvg >> 3);
	meas.lambdaVoltage = (lambdaVoltageAvg >> 3);

	meas.tempI = toTempI(meas.tempIVoltage);
	meas.tempO = toTempO(meas.tempOVoltage);
	meas.lambda = toLambda(meas.lambdaVoltage);

	return meas;
}

void display(measurement meas) {
	uint16_t lambdax100 = divRoundNearest(meas.lambda, 10);
	div_t lambdaT = div(lambdax100, 100);

	char log[64];
	snprintf(log, sizeof(log),
			"Ti %3d C %4u - To %3d C %4u - L %4u %4u\r\n",
			meas.tempI, meas.tempIVoltage, meas.tempO, meas.tempOVoltage,
			meas.lambda, meas.lambdaVoltage);
	printString(log);

	char line0[17];
	char line1[17];
	snprintf(line0, sizeof(line0), "Ti %3dC To %3dC ", meas.tempI, meas.tempO);
	snprintf(line1, sizeof(line1), "L  %d.%02d %s   ",
			lambdaT.quot, abs(lambdaT.rem), toInfo(lambdax100));
	lcd_setcursor(0, 1);
	lcd_string(line0);
	lcd_setcursor(0, 2);
	lcd_string(line1);
}

int16_t toTempI(uint16_t mV) {
	int temp = divRoundNearest(mV, 5);

	return temp;
}

int16_t toTempO(uint16_t mV) {
	uint8_t length = sizeof(tempOTable) / sizeof(tempOTable[0]);
	int16_t temp = lookupLinInter(mV, tempOTable, length);

	return temp;
}

int16_t toLambda(uint16_t mV) {
	uint8_t length = sizeof(lambdaTable) / sizeof(lambdaTable[0]);
	int16_t lambda = lookupLinInter(mV, lambdaTable, length);

	return lambda;
}

int16_t lookupLinInter(uint16_t mV, const tableEntry table[], uint8_t length) {
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

	uint16_t diffVoltage = table[i + 1].mV - table[i].mV;
	int16_t diffValue = table[i + 1].value - table[i].value;
	int16_t value = table[i].value + divRoundNearest(
			(uint32_t)(mV - table[i].mV) * diffValue, diffVoltage);

	return value;
}

const char* toInfo(uint16_t lambda) {
	if (lambda > 190) {
		return LEAN;
	} else if (lambda > 150 && lambda <= 190) {
		return OKAY;
	} else if (lambda >= 130 && lambda <= 150) {
		return IDEAL;
	} else {
		return RICH;
	}
}
