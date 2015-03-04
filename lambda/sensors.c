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

// #define AREF_MV 4850
#define AREF_MV 5000
#define ADC_OFFSET_MV 7
// #define TMP_OP_OFFSET_MV 441
#define TMP_OP_OFFSET_MV 454

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
	{ -57, -50 },
	{ 454, 0 },
	{ 1403, 100 },
	{ 2264, 200 },
	{ 3047, 300 },
	{ 3762, 400 }
};

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

float toLambda(float mV) {
	int length = sizeof(lambdaTable) / sizeof(lambdaTable[0]);
	float lambda = lookupLinInter(mV, lambdaTable, length);

	return lambda;
}

int toTempI(float mV) {
	int temp = round(mV / 5);

	return temp;
}

int toTempO(float mV) {
	int length = sizeof(tempOTable) / sizeof(tempOTable[0]);
	float c = lookupLinInter(mV, tempOTable, length);
	int temp = round(c);

	return temp;
}

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


