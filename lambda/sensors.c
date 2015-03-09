/*
 * sensors.c
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 *
 * TODO put defines in makefile
 */
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "sensors.h"
#include "integers.h"

/**
 * Table used to look up the lambda value at 12 V heater voltage
 * and 220°C exhaust gas temperature. Most values are approximated
 * from the characteristic curve in the datasheet.
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

int16_t toTempI(int16_t mV) {
	int temp = roundNearest(mV, 5);

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
	int16_t value = table[i].value +
			roundNearest((int32_t)(mV - table[i].mV) * diffValue, diffVoltage);

	return value;
}

const char* toInfo(int16_t lambda) {
	if (lambda > 1500) {
		return LEAN;
	} else if (lambda > 1300 && lambda <= 1500) {
		return IDEAL;
	} else {
		return RICH;
	}
}


