/*
 * sensors.c
 *
 * Functions to evaluate measurements from the sensors.
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "alert.h"
#include "sensors.h"
#include "integers.h"
#include "interrupts.h"
#include "pins.h"
#include "messages.h"

static HeaterState heaterState = heaterStateOff;
static uint32_t heaterOnTime = 0;

/**
 * Table used to look up the lambda value at 12 V heater voltage
 * and 220°C exhaust gas temperature. Most values are approximated
 * from the characteristic curve in the data sheet. The voltage
 * of the sensor is amplified by factor 11.
 * TODO real data?
 */
static TableEntry const lambdaTable[] = {
	{ 44, 2000 },
	{ 55, 1900 },
	{ 66, 1800 },
	{ 88, 1700 },
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
static TableEntry const tempOTable[] = {
	// { -57, -50 },
	{ 454, 0 },
	{ 1403, 100 },
	{ 2264, 200 },
	{ 3047, 300 },
	{ 3762, 400 }
};

/**
 * Table used to look up ADC measurement deviation due to non-linearity.
 */
static TableEntry const linADCTable[] = {
	{ 0, ADC_NONLIN_0 },
	{ AREF_MV, ADC_NONLIN_AREF }
};

/**
 * Variables holding averaged voltages*8.
 */
static uint32_t lambdaVoltageAvg = 44 << 3; // Lambda 2.00
static uint32_t tempIVoltageAvg = 100 << 3; // 20°C
static uint32_t tempOVoltageAvg = 644 << 3; // 20°C

/**
 * Measures the "input" and "output" temperatures and the lambda value,
 * calculates an exponential moving average and displays the translated values.
 */
Measurement measure(void) {
	uint32_t tempIVoltage = linADC(getVoltage(ADC_TEMPI));
	tempIVoltageAvg = tempIVoltage + tempIVoltageAvg -
			((tempIVoltageAvg - 4) >> 3);

	uint32_t tempOVoltage = linADC(getVoltage(ADC_TEMPO));
	tempOVoltageAvg = tempOVoltage + tempOVoltageAvg -
			((tempOVoltageAvg - 4) >> 3);

	if (heaterState == heaterStateReady) {
		uint32_t lambdaVoltage = linADC(getVoltage(ADC_LAMBDA));
		lambdaVoltageAvg = lambdaVoltage + lambdaVoltageAvg -
				((lambdaVoltageAvg - 4) >> 3);
	} else {
		lambdaVoltageAvg = 44 << 3; // Lambda 2.00
	}

	uint16_t heaterVoltage = linADC(getVoltage(ADC_HEATER));

	Measurement meas;
	meas.tempI = toTempI(tempIVoltageAvg >> 3);
	meas.tempO = toTempO(tempOVoltageAvg >> 3);
	meas.lambda = toLambda(lambdaVoltageAvg >> 3);
	meas.current = toCurrent(heaterVoltage);

	return meas;
}

Measurement readMeas(char* const fields[], size_t const size) {
	Measurement meas;
	if (size < 4) {
		return meas;
	}
	meas.tempI  = atoi(fields[0]);
	meas.tempO  = atoi(fields[1]);
	meas.lambda = atoi(fields[2]);
	meas.current = atoi(fields[3]);

	return meas;
}

int16_t toTempI(uint16_t const mV) {
	int16_t temp = divRoundNearest(mV, 5);

	return temp;
}

int16_t toTempO(uint16_t const mV) {
	int16_t temp = lookupLinInter(mV, tempOTable, ARRAY_LENGTH(tempOTable));

	return temp;
}

uint16_t toLambda(uint16_t const mV) {
	int16_t lambda = lookupLinInter(mV, lambdaTable, ARRAY_LENGTH(lambdaTable));

	return lambda;
}

uint16_t toCurrent(uint16_t const mV) {
	uint16_t current = mV * (1000 / SHUNT_MILLIOHMS);

	return current;
}

int16_t lookupLinInter(uint16_t const mV, TableEntry const table[],
		size_t const size) {
	if (mV < table[0].mV) {
		return table[0].value;
	} else if (mV > table[size - 1].mV) {
		return table[size - 1].value;
	}

	uint8_t i = 0;
	for (; i < size - 1; i++) {
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

int32_t linADC(uint16_t const mV) {
	int16_t dev = lookupLinInter(mV, linADCTable, ARRAY_LENGTH(linADCTable));

	return (int32_t)mV == 0 ? 0 : mV - dev;
}

char* toInfo(uint16_t const lambda) {
	if (lambda > 190) {
		return MSG_LEAN;
	} else if (lambda > 150 && lambda <= 190) {
		return MSG_OKAY;
	} else if (lambda >= 130 && lambda <= 150) {
		return MSG_IDEAL;
	} else {
		return MSG_RICH;
	}
}

void setHeaterState(HeaterState const state) {
	if (state == heaterStateOn) {
		PORTB |= (1 << PB2);
		heaterOnTime = getTime();
		alert_P(1, 1, 31, PSTR(MSG_HEATER_UP_0), PSTR(MSG_HEATER_UP_1), true);
	} else if (state == heaterStateOff || state == heaterStateFault) {
		PORTB &= ~(1 << PB2);
	}
	heaterState = state;
}

HeaterState getHeaterState(void) {
	// bit_is_set(PORTB, PB2)
	return heaterState;
}

uint32_t getHeaterUptime(void) {
	return getTime() - heaterOnTime;
}
