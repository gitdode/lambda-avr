/*
 * lambda-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU Lesser General Public License Version 2.1 (the "LGPL License"), in which
 * case the provisions of the LGPL License are applicable instead of those
 * above.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU (Lesser) General Public License for more details.
 *
 * DISCLAIMER: I'm experienced with Java, but new to C.
 *
 * ATTRIBUTION: This project includes the module USART and the Makefile from
 * the code accompanying the book Make: AVR Programming by Elliot Williams,
 * a great book and a pleasant read, that helped me tremendously to get
 * started with AVR programming.
 * ATTRIBUTION: This project includes the module lcdroutines from
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "USART.h"
#include "adc.h"
#include "integers.h"
#include "sensors.h"
#include "avrjunit.h"

static const tableEntry testTable[] = {
		{0, 0},
		{10, 10}
};

/* Module adc */

bool testSetupADC(void) {
	setupADC();

	// AVCC is set as AREF
	if ((ADMUX & (1 << REFS0)) != (1 << REFS0)) return false;
	// ADC clock prescaler/8
	uint8_t prescalerBy8 = (1 << ADPS1) | (1 << ADPS0);
	if ((ADCSRA & prescalerBy8) != prescalerBy8) return false;
	// ADC enabled
	if ((ADCSRA & (1 << ADEN)) != (1 << ADEN)) return false;

	return true;
}

bool testSetupSleepMode(void) {
	setupSleepMode();

	// set_sleep_mode(SLEEP_MODE_ADC);
	if ((SMCR & (1 << SM0)) != (1 << SM0)) return false;
	// sei(); // enable global interrupts
	if ((SREG & (1 << SREG_I)) != (1 << SREG_I)) return false;
	// ADC interrupt enabled
	if ((ADCSRA & (1 << ADIE)) != (1 << ADIE)) return false;

	return true;
}

bool testGetVoltage(void) {
	setupADC();
	setupSleepMode();

	// enable pull-up resistor so the measured voltage
	// should be (close to?) AREF
	PORTC |= (1 << PC1);
	// PORTC = 0xff;

	// it seems that sleep_mode() causes some interference when called
	// immediately after sending data over USART - some buffer not yet empty?
	// loop_until_bit_is_set(UCSR0A, UDRE0);
	_delay_ms(10);

	int16_t mV = getVoltage(PC1);

	return mV > 4900;
}

/* Module integers */

bool testDivRoundNearest(void) {
	int32_t round = 0;

	round = divRoundNearest(4, 2);
	if (round != 2) return false;

	round = divRoundNearest(5, 2);
	if (round != 3) return false;

	round = divRoundNearest(10, 3);
	if (round != 3) return false;

	return true;
}

bool testDivRoundNearestNumNeg(void) {
	int32_t round = 0;

	round = divRoundNearest(-4, 2);
	if (round != -2) return false;

	round = divRoundNearest(-5, 2);
	if (round != -3) return false;

	round = divRoundNearest(-10, 3);
	if (round != -3) return false;

	return true;
}

bool testDivRoundNearestDenNeg(void) {
	int32_t round = 0;

	round = divRoundNearest(4, -2);
	if (round != -2) return false;

	round = divRoundNearest(5, -2);
	if (round != -3) return false;

	round = divRoundNearest(10, -3);
	if (round != -3) return false;

	return true;
}

bool testDivRoundNearestBothNeg(void) {
	int32_t round = 0;

	round = divRoundNearest(-4, -2);
	if (round != 2) return false;

	round = divRoundNearest(-5, -2);
	if (round != 3) return false;

	round = divRoundNearest(-10, -3);
	if (round != 3) return false;

	return true;
}

bool testDivRoundUp(void) {
	int32_t round = 0;

	round = divRoundUp(4, 2);
	if (round != 2) return false;

	round = divRoundUp(5, 2);
	if (round != 3) return false;

	round = divRoundUp(10, 3);
	if (round != 4) return false;

	return true;
}

bool testDivRoundUpNumNeg(void) {
	int32_t round = 0;

	round = divRoundUp(-4, 2);
	if (round != -2) return false;

	round = divRoundUp(-5, 2);
	if (round != -3) return false;

	round = divRoundUp(-10, 3);
	if (round != -4) return false;

	return true;
}

bool testDivRoundUpDenNeg(void) {
	int32_t round = 0;

	round = divRoundUp(4, -2);
	if (round != -2) return false;

	round = divRoundUp(5, -2);
	if (round != -3) return false;

	round = divRoundUp(10, -3);
	if (round != -4) return false;

	return true;
}

bool testDivRoundUpBothNeg(void) {
	int32_t round = 0;

	round = divRoundUp(-4, -2);
	if (round != 2) return false;

	round = divRoundUp(-5, -2);
	if (round != 3) return false;

	round = divRoundUp(-10, -3);
	if (round != 4) return false;

	return true;
}

/* Module sensors */

bool testMeasure(void) {
	setupADC();
	setupSleepMode();

	// enable pull-up resistor so the measured voltage
	// should be (close to?) AREF
	PORTC |= ((1 << PC5) | (1 << PC0) | (1 << PC2));

	_delay_ms(10);

	// do many measurements so the averaged voltages are near the measured
	// voltages (close to AREF)
	measurement meas;
	for (uint8_t i = 0; i < 64; i++) {
		meas = measure();
	}

	if (meas.tempIVoltage < 4900) return false;
	if (meas.tempOVoltage < 4900) return false;
	// lambdaVoltage is divided by the OP amplification factor 11
	if (meas.lambdaVoltage < (4900 / 11)) return false;

	// verify that temperatures and lambda are calculated correctly
	if (meas.tempI != toTempI(meas.tempIVoltage)) return false;
	if (meas.tempO != toTempO(meas.tempOVoltage)) return false;
	if (meas.lambda != toLambda(meas.lambdaVoltage)) return false;

	return true;
}

bool testAverageUp(void) {
	int32_t value = 10;
	int32_t avg = 0;
	for (uint8_t i = 0; i < 14; i++) {
		avg = average((value << 4), avg, 4);
	}

	return divRoundNearest(avg, 16) == value;
}

bool testAverageDown(void) {
	int32_t value = 0;
	int32_t avg = (10 << 4);
	for (uint8_t i = 0; i < 14; i++) {
		avg = average((value << 4), avg, 4);
	}

	return divRoundNearest(avg, 16) == value;
}

bool testAverageDoesNotWrap(void) {
	int32_t value = 5000;
	int32_t avg = (value << 4);
	avg = average((value << 4), avg, 16);

	return divRoundNearest(avg, 16) == value;
}

bool testToLambdaValue(void) {
	int16_t lambda = toLambda(12);

	return lambda == 1500;
}

bool testToLambdaInter(void) {
	int16_t lambda = toLambda(50);

	return lambda == 1073;
}

bool testToTempI(void) {
	int16_t temp = toTempI(100);

	return temp == 20;
}

bool testToTempOValue(void) {
	int16_t temp = toTempO(454);

	return temp == 0;
}

bool testToTempOInter(void) {
	int16_t temp = toTempO(929);

	return temp == 50;
}

bool testLookupLinInterBelow(void) {
	int16_t value = lookupLinInter(-5, testTable, 2);

	return value == 0;
}

bool testLookupLinInterAbove(void) {
	int16_t value = lookupLinInter(15, testTable, 2);

	return value == 10;
}

bool testLookupLinInterValue(void) {
	int16_t value = lookupLinInter(10, testTable, 2);

	return value == 10;
}

bool testLookupLinInterInter(void) {
	int16_t value = lookupLinInter(3, testTable, 2);

	return value == 3;
}

bool testToInfoLean(void) {
	const char* info = toInfo(1600);

	return ! strcmp(info, LEAN);
}

bool testToInfoIdeal(void) {
	const char* info = toInfo(1400);

	return ! strcmp(info, IDEAL);
}

bool testToInfoRich(void) {
	const char* info = toInfo(1200);

	return ! strcmp(info, RICH);
}

// these long function names passed along as strings use a lot of memory
test tests[] = {
		{"adc", "testSetupADC", testSetupADC},
		{"adc", "testSetupSleepMode", testSetupSleepMode},
		{"adc", "testGetVoltage", testGetVoltage},
		{"integers", "testDivRoundNearest", testDivRoundNearest},
		{"integers", "testDivRoundNearestNumNeg", testDivRoundNearestNumNeg},
		{"integers", "testDivRoundNearestDenNeg", testDivRoundNearestDenNeg},
		{"integers", "testDivRoundNearestBothNeg", testDivRoundNearestBothNeg},
		{"integers", "testDivRoundUp", testDivRoundUp},
		{"integers", "testDivRoundUpNumNeg", testDivRoundUpNumNeg},
		{"integers", "testDivRoundUpDenNeg", testDivRoundUpDenNeg},
		{"integers", "testDivRoundUpBothNeg", testDivRoundUpBothNeg},
		{"sensors", "testMeasure", testMeasure},
		{"sensors", "testAverageUp", testAverageUp},
		{"sensors", "testAverageDown", testAverageDown},
		{"sensors", "testAverageDoesNotWrap", testAverageDoesNotWrap},
		{"sensors", "testToLambdaValue", testToLambdaValue},
		{"sensors", "testToLambdaInter", testToLambdaInter},
		{"sensors", "testToTempI", testToTempI},
		{"sensors", "testToTempOValue", testToTempOValue},
		{"sensors", "testToTempOInter", testToTempOInter},
		{"sensors", "testLookupLinInterValue", testLookupLinInterValue},
		{"sensors", "testLookupLinInterInter", testLookupLinInterInter},
		{"sensors", "testLookupLinInterBelow", testLookupLinInterBelow},
		{"sensors", "testLookupLinInterAbove", testLookupLinInterAbove},
		{"sensors", "testToInfoLean", testToInfoLean},
		{"sensors", "testToInfoIdeal", testToInfoIdeal},
		{"sensors", "testToInfoRich", testToInfoRich}
};

int main(void) {
	initUSART();

	uint16_t count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", tests, count);

	return 0;
}
