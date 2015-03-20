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

#include <stdio.h>
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

uint8_t testSetupADC(void) {
	setupADC();

	// AVCC is set as AREF
	return (ADMUX & (1 << REFS0)) == (1 << REFS0);
	// ADC clock prescaler/16
	return (ADCSRA & (1 << ADPS2)) == (1 << ADPS2);
	// ADC enabled
	return (ADCSRA & (1 << ADEN)) == (1 << ADEN);
}

uint8_t testSetupSleepMode(void) {
	setupSleepMode();

	// don't know how to verify these
	// set_sleep_mode(SLEEP_MODE_ADC);
	// sei(); // enable global interrupts

	// ADC interrupt enabled
	return (ADCSRA & (1 << ADIE)) == (1 << ADIE);
}

uint8_t testGetVoltage(void) {
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

uint8_t testDivRoundNearest(void) {
	int32_t round = 0;

	round = divRoundNearest(4, 2);
	if (round != 2) return 0;

	round = divRoundNearest(5, 2);
	if (round != 3) return 0;

	round = divRoundNearest(10, 3);
	if (round != 3) return 0;

	return 1;
}

uint8_t testDivRoundNearestNumNeg(void) {
	int32_t round = 0;

	round = divRoundNearest(-4, 2);
	if (round != -2) return 0;

	round = divRoundNearest(-5, 2);
	if (round != -3) return 0;

	round = divRoundNearest(-10, 3);
	if (round != -3) return 0;

	return 1;
}

uint8_t testDivRoundNearestDenNeg(void) {
	int32_t round = 0;

	round = divRoundNearest(4, -2);
	if (round != -2) return 0;

	round = divRoundNearest(5, -2);
	if (round != -3) return 0;

	round = divRoundNearest(10, -3);
	if (round != -3) return 0;

	return 1;
}

uint8_t testDivRoundNearestBothNeg(void) {
	int32_t round = 0;

	round = divRoundNearest(-4, -2);
	if (round != 2) return 0;

	round = divRoundNearest(-5, -2);
	if (round != 3) return 0;

	round = divRoundNearest(-10, -3);
	if (round != 3) return 0;

	return 1;
}

uint8_t testDivRoundUp(void) {
	int32_t round = 0;

	round = divRoundUp(4, 2);
	if (round != 2) return 0;

	round = divRoundUp(5, 2);
	if (round != 3) return 0;

	round = divRoundUp(10, 3);
	if (round != 4) return 0;

	return 1;
}

uint8_t testDivRoundUpNumNeg(void) {
	int32_t round = 0;

	round = divRoundUp(-4, 2);
	if (round != -2) return 0;

	round = divRoundUp(-5, 2);
	if (round != -3) return 0;

	round = divRoundUp(-10, 3);
	if (round != -4) return 0;

	return 1;
}

uint8_t testDivRoundUpDenNeg(void) {
	int32_t round = 0;

	round = divRoundUp(4, -2);
	if (round != -2) return 0;

	round = divRoundUp(5, -2);
	if (round != -3) return 0;

	round = divRoundUp(10, -3);
	if (round != -4) return 0;

	return 1;
}

uint8_t testDivRoundUpBothNeg(void) {
	int32_t round = 0;

	round = divRoundUp(-4, -2);
	if (round != 2) return 0;

	round = divRoundUp(-5, -2);
	if (round != 3) return 0;

	round = divRoundUp(-10, -3);
	if (round != 4) return 0;

	return 1;
}

/* Module sensors */

uint8_t testAverageUp(void) {
	int32_t value = 10;
	int32_t avg = 0;
	for (uint8_t i = 0; i < 14; i++) {
		avg = average((value << 4), avg, 4);
	}

	return divRoundNearest(avg, 16) == value;
}

uint8_t testAverageDown(void) {
	int32_t value = 0;
	int32_t avg = (10 << 4);
	for (uint8_t i = 0; i < 14; i++) {
		avg = average((value << 4), avg, 4);
	}

	return divRoundNearest(avg, 16) == value;
}

uint8_t testAverageDoesNotWrap(void) {
	int32_t value = 5000;
	int32_t avg = (value << 4);
	avg = average((value << 4), avg, 8);

	return divRoundNearest(avg, 16) == value;
}

uint8_t testToLambdaValue(void) {
	int16_t lambda = toLambda(12);

	return lambda == 1500;
}

uint8_t testToLambdaInter(void) {
	int16_t lambda = toLambda(50);

	return lambda == 1073;
}

uint8_t testToTempI(void) {
	int16_t temp = toTempI(100);

	return temp == 20;
}

uint8_t testToTempOValue(void) {
	int16_t temp = toTempO(454);

	return temp == 0;
}

uint8_t testToTempOInter(void) {
	int16_t temp = toTempO(929);

	return temp == 50;
}

uint8_t testLookupLinInterBelow(void) {
	int16_t value = lookupLinInter(-5, testTable, 2);

	return value == 0;
}

uint8_t testLookupLinInterAbove(void) {
	int16_t value = lookupLinInter(15, testTable, 2);

	return value == 10;
}

uint8_t testLookupLinInterValue(void) {
	int16_t value = lookupLinInter(10, testTable, 2);

	return value == 10;
}

uint8_t testLookupLinInterInter(void) {
	int16_t value = lookupLinInter(3, testTable, 2);

	return value == 3;
}

// these long function names passed along as strings use a lot of memory
test tests[] = {
		{"adc", "testSetupADC", testSetupADC},
		{"adc", "testGetVoltage", testGetVoltage},
		{"adc", "testSetupSleepMode", testSetupSleepMode},
		{"integers", "testDivRoundNearest", testDivRoundNearest},
		{"integers", "testDivRoundNearestNumNeg", testDivRoundNearestNumNeg},
		{"integers", "testDivRoundNearestDenNeg", testDivRoundNearestDenNeg},
		{"integers", "testDivRoundNearestBothNeg", testDivRoundNearestBothNeg},
		{"integers", "testDivRoundUp", testDivRoundUp},
		{"integers", "testDivRoundUpNumNeg", testDivRoundUpNumNeg},
		{"integers", "testDivRoundUpDenNeg", testDivRoundUpDenNeg},
		{"integers", "testDivRoundUpBothNeg", testDivRoundUpBothNeg},
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
		{"sensors", "testLookupLinInterAbove", testLookupLinInterAbove}
};

int main(void) {
	initUSART();

	uint16_t count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", tests, count);

	return 0;
}
