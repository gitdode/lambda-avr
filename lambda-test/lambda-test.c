/*
 * lambda-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
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
#include "interrupts.h"
#include "adc.h"
#include "integers.h"
#include "sensors.h"
#include "display.h"
#include "avrjunit.h"

static const tableEntry testTable[] = {
		{10, 10},
		{20, 20}
};

/* Module interrupts */

bool testSetupPorts(void) {
	setupPorts();

	// test that the pull-up resistor for the mouton is enabled
	assertTrue(bit_is_set(PORTB, PB0));

	return true;
}

bool testSetupSleepMode(void) {
	setupSleepMode();

	// set_sleep_mode(SLEEP_MODE_ADC);
	assertTrue(bit_is_set(SMCR, SM0));

	return true;
}

bool testInitInterrupts(void) {
	initInterrupts();

	// ADC interrupt enabled
	assertTrue(bit_is_set(ADCSRA, ADIE));
	// PC interrupts enabled
	assertTrue(bit_is_set(PCICR, PCIE0));
	assertTrue(bit_is_set(PCMSK0, PB0));
	// sei(); // enable global interrupts
	assertTrue(bit_is_set(SREG, SREG_I));

	return true;
}

/* Module adc */

bool testSetupADC(void) {
	setupADC();

	// AVCC is set as AREF
	assertTrue(bit_is_set(ADMUX, REFS0));
	// digital inputs are disabled
	uint8_t adcPorts = (1 << ADC_TEMPI) | (1 << ADC_TEMPO) | (1 << ADC_LAMBDA);
	assertTrue((DIDR0 & adcPorts) == adcPorts);
	// ADC clock prescaler/8
	uint8_t prescalerBy8 = (1 << ADPS1) | (1 << ADPS0);
	assertTrue((ADCSRA & prescalerBy8) == prescalerBy8);
	// ADC enabled
	assertTrue(bit_is_set(ADCSRA, ADEN));

	return true;
}

bool testGetVoltage(void) {
	initInterrupts();
	setupADC();
	setupSleepMode();

	// enable pull-up resistor so the measured voltage
	// should be close to AREF
	PORTC |= (1 << PC1);
	// PORTC = 0xff;

	// it seems that sleep_mode() causes some interference when called
	// immediately after sending data over USART - some buffer not yet empty?
	// loop_until_bit_is_set(UCSR0A, UDRE0);
	_delay_ms(10);

	uint16_t mV = getVoltage(PC1);

	return mV > 4900;
}

/* Module integers */

bool testDivRoundNearest(void) {
	assertTrue(divRoundNearest(4, 2) == 2);
	assertTrue(divRoundNearest(5, 2) == 3);
	assertTrue(divRoundNearest(10, 3) == 3);

	return true;
}

bool testDivRoundNearestNumNeg(void) {
	assertTrue(divRoundNearest(-4, 2) == -2);
	assertTrue(divRoundNearest(-5, 2) == -3);
	assertTrue(divRoundNearest(-10, 3) == -3);

	return true;
}

bool testDivRoundNearestDenNeg(void) {
	assertTrue(divRoundNearest(4, -2) == -2);
	assertTrue(divRoundNearest(5, -2) == -3);
	assertTrue(divRoundNearest(10, -3) == -3);

	return true;
}

bool testDivRoundNearestBothNeg(void) {
	assertTrue(divRoundNearest(-4, -2) == 2);
	assertTrue(divRoundNearest(-5, -2) == 3);
	assertTrue(divRoundNearest(-10, -3) == 3);

	return true;
}

bool testDivRoundUp(void) {
	assertTrue(divRoundUp(4, 2) == 2);
	assertTrue(divRoundUp(5, 2) == 3);
	assertTrue(divRoundUp(10, 3) == 4);

	return true;
}

bool testDivRoundUpNumNeg(void) {
	assertTrue(divRoundUp(-4, 2) == -2);
	assertTrue(divRoundUp(-5, 2) == -3);
	assertTrue(divRoundUp(-10, 3) == -4);

	return true;
}

bool testDivRoundUpDenNeg(void) {
	assertTrue(divRoundUp(4, -2) == -2);
	assertTrue(divRoundUp(5, -2) == -3);
	assertTrue(divRoundUp(10, -3) == -4);

	return true;
}

bool testDivRoundUpBothNeg(void) {
	assertTrue(divRoundUp(-4, -2) == 2);
	assertTrue(divRoundUp(-5, -2) == 3);
	assertTrue(divRoundUp(-10, -3) == 4);

	return true;
}

/* Module sensors */

bool testMeasure(void) {
	setupADC();
	setupSleepMode();

	// enable pull-up resistor so the measured voltage
	// should be close to AREF
	PORTC |= ((1 << PC5) | (1 << PC0) | (1 << PC2));

	_delay_ms(10);

	// do many measurements so the averaged voltages are near the measured
	// voltages (close to AREF)
	measurement meas;
	for (uint8_t i = 0; i < 64; i++) {
		meas = measure();
	}

	assertTrue(meas.tempIVoltage > 4900);
	assertTrue(meas.tempOVoltage > 4900);
	assertTrue(meas.lambdaVoltage > 4900);

	// verify that temperatures and lambda are calculated correctly
	assertTrue(meas.tempI == toTempI(meas.tempIVoltage));
	assertTrue(meas.tempO == toTempO(meas.tempOVoltage));
	assertTrue(meas.lambda == toLambda(meas.lambdaVoltage));

	return true;
}

bool testToLambdaValue(void) {
	int16_t lambda = toLambda(132);

	return lambda == 1500;
}

bool testToLambdaInter(void) {
	int16_t lambda = toLambda(550);

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
	int16_t value = lookupLinInter(0, testTable, 2);

	return value == 10;
}

bool testLookupLinInterAbove(void) {
	int16_t value = lookupLinInter(30, testTable, 2);

	return value == 20;
}

bool testLookupLinInterValue(void) {
	int16_t value = lookupLinInter(10, testTable, 2);

	return value == 10;
}

bool testLookupLinInterInter(void) {
	int16_t value = lookupLinInter(15, testTable, 2);

	return value == 15;
}

bool testToInfoLean(void) {
	const char* info = toInfo(191);

	return ! strcmp(info, LEAN);
}

bool testToInfoOkay(void) {
	assertTrue(0 == strcmp(toInfo(190), OKAY));
	assertTrue(0 == strcmp(toInfo(170), OKAY));
	assertTrue(0 == strcmp(toInfo(151), OKAY));

	return true;
}

bool testToInfoIdeal(void) {
	assertTrue(0 == strcmp(toInfo(150), IDEAL));
	assertTrue(0 == strcmp(toInfo(140), IDEAL));
	assertTrue(0 == strcmp(toInfo(130), IDEAL));

	return true;
}

bool testToInfoRich(void) {
	const char* info = toInfo(129);

	return ! strcmp(info, RICH);
}

/* Module display */

// TODO assertions
bool testCycle(void) {

	return true;
}

// TODO assertions
bool testUpdateInitial(void) {

	return true;
}

// TODO assertions
bool testUpdate(void) {
	// set first display option so display() won't actually log something
	// via USART which would break the test result XML
	// TODO elegant solution for this and testing display()?

	// measurement meas = {0, 0, 0, 0, 0, 0};
	// update(meas);

	return true;
}

// these long function names passed along as strings use a lot of memory
test tests[] = {
		{"interrupts", "testSetupPorts", testSetupPorts},
		{"interrupts", "testSetupSleepMode", testSetupSleepMode},
		{"interrupts", "testInitInterrupts", testInitInterrupts},
		{"adc", "testSetupADC", testSetupADC},
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
		{"sensors", "testToInfoOkay", testToInfoOkay},
		{"sensors", "testToInfoIdeal", testToInfoIdeal},
		{"sensors", "testToInfoRich", testToInfoRich},
		{"display", "testCycle", testCycle},
		{"display", "testUpdateInitial", testUpdateInitial},
		{"display", "testUpdate", testUpdate}
};

int main(void) {
	initUSART();

	uint16_t count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", tests, count);

	return 0;
}
