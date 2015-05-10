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
#include "avrjunit.h"
#include "interrupts.h"
#include "adc.h"
#include "integers.h"
#include "sensors.h"
#include "display.h"
#include "pins.h"
#include "command.h"
#include "strings.h"

static const tableEntry testTable[] = {
		{10, 10},
		{20, 20}
};

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

	uint16_t mV = getVoltage(PC1);

	return mV > 4900;
}

/* Module command */

bool testIsSimulation(void) {
	assertFalse(isSimulation());
	runCommand("se");
	assertTrue(isSimulation());
	runCommand("sd");
	assertFalse(isSimulation());

	return true;
}

bool testIsLogging(void) {
	assertFalse(isLogging());
	runCommand("le");
	assertTrue(isLogging());
	runCommand("ld");
	assertFalse(isLogging());

	return true;
}

/* Module display */

// TODO assertions
bool testCycle(void) {
	cycleDisplay();

	return true;
}

// TODO assertions
bool testUpdateInitial(void) {

	return true;
}

// TODO assertions
bool testUpdate(void) {
	measurement meas = {0, 0, 0};
	updateMeas(meas);

	return true;
}

// TODO test display() with no display connected?
bool testDisplay(void) {
	measurement meas = {0, 0, 0};
	displayMeas(meas, "  ");

	return true;
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

/* Module interrupts */

bool testSetupPorts(void) {
	setupPorts();

	// test that the pull-up resistor for the mouton is enabled
	assertTrue(bit_is_set(PORTB, PB0));

	// test that the beep output pin is enabled
	assertTrue(bit_is_set(DDRB, PB1));

	return true;
}

bool testSetupSleepMode(void) {
	setupSleepMode();

	// set_sleep_mode(SLEEP_MODE_IDLE);
	assertFalse(bit_is_set(SMCR, SM2));
	assertFalse(bit_is_set(SMCR, SM1));
	assertFalse(bit_is_set(SMCR, SM0));

	return true;
}

bool testInitInterrupts(void) {
	initInterrupts();

	// ADC interrupt enabled
	assertTrue(bit_is_set(ADCSRA, ADIE));
	// PC interrupts enabled
	// assertTrue(bit_is_set(PCICR, PCIE0));
	// assertTrue(bit_is_set(PCMSK0, PB0));
	// enable timer 0 overflow interrupt
	assertTrue(bit_is_set(TIMSK0, TOIE0));
	// USART RX complete interrupt 0 enabled
	assertTrue(bit_is_set(UCSR0B, RXCIE0));

	// sei(); // enable global interrupts
	assertTrue(bit_is_set(SREG, SREG_I));

	return true;
}

bool testInitTimers(void) {
	initInterrupts();

	// timer0 clock prescaler /64 = 15.625 kHz overflowing every 16.2 ms
	uint8_t prescalerBy64 = (1 << CS00) | (1 << CS01);
	assertTrue((TCCR0B & prescalerBy64) == prescalerBy64);

	// timer1 Clear Timer on Compare Match mode, TOP OCR1A
	assertTrue(bit_is_set(TCCR1B, WGM12));
	// timer1 clock prescaler/8
	assertTrue(bit_is_set(TCCR1B, CS11));
	// toggles PB1 at 7.8 kHz generating a 3.9 kHz beep
	// assertTrue(OCR1A == 16);
	// 1.8 kHz is less noisy on the small piezo beeper
	assertTrue(OCR1A == 32);

	return true;
}

/* Module sensors */

bool testMeasure(void) {
	setupADC();
	setupSleepMode();

	// enable pull-up resistor so the measured voltage
	// should be close to AREF
	PORTC |= ((1 << ADC_TEMPI) | (1 << ADC_TEMPO) | (1 << ADC_LAMBDA));

	// do many measurements so the averaged voltages are near the measured
	// voltages (close to AREF)
	measurement meas;
	for (uint8_t i = 0; i < 64; i++) {
		meas = measure();
	}

	// verify that temperatures and lambda are calculated correctly
	// for voltages > 4950 and <= 5000 mV
	assertTrue(meas.tempI > 990 && meas.tempI <= 1000);
	assertTrue(meas.tempO == 400);
	assertTrue(meas.lambda == 997);

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

/* Module strings */

bool testSplit(void) {
	char* string = "f1 f2  f3 ";
	char* fields[4];
	split(string, " ", fields, 4);

	assertTrue(strcmp("f1", fields[0]) == 0);
	assertTrue(strcmp("f2", fields[1]) == 0);
	assertTrue(strcmp("f3", fields[2]) == 0);
	assertTrue(strcmp("", fields[3]) == 0);

	return true;
}

/*
 * Whoa. Trying to write more elements than its size in the fields array
 * seems to cause the AVR to reset and rerun the tests in an infinite loop.
 */
bool testSplitSizeTooSmall(void) {
	char* string = "f1 f2";
	char* fields[1];
	split(string, " ", fields, 1);

	assertTrue(strcmp("f1", fields[0]) == 0);

	return true;
}


// TODO these long function names passed along as strings use a lot of memory
// use PROGMEM?
test tests[] = {
		{"adc", "testSetupADC", testSetupADC},
		{"adc", "testGetVoltage", testGetVoltage},
		{"command", "testIsSimulation", testIsSimulation},
		{"command", "testIsLogging", testIsLogging},
		{"display", "testCycle", testCycle},
		{"display", "testUpdateInitial", testUpdateInitial},
		{"display", "testUpdate", testUpdate},
		{"display", "testDisplay", testDisplay},
		{"integers", "testDivRoundNearest", testDivRoundNearest},
		{"integers", "testDivRoundNearestNumNeg", testDivRoundNearestNumNeg},
		{"integers", "testDivRoundNearestDenNeg", testDivRoundNearestDenNeg},
		{"integers", "testDivRoundNearestBothNeg", testDivRoundNearestBothNeg},
		{"integers", "testDivRoundUp", testDivRoundUp},
		{"integers", "testDivRoundUpNumNeg", testDivRoundUpNumNeg},
		{"integers", "testDivRoundUpDenNeg", testDivRoundUpDenNeg},
		{"integers", "testDivRoundUpBothNeg", testDivRoundUpBothNeg},
		{"interrupts", "testSetupPorts", testSetupPorts},
		{"interrupts", "testSetupSleepMode", testSetupSleepMode},
		{"interrupts", "testInitInterrupts", testInitInterrupts},
		{"interrupts", "testInitTimers", testInitInterrupts},
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
		{"strings", "testSplit", testSplit},
		{"strings", "testSplitSizeTooSmall", testSplitSizeTooSmall}
};

int main(void) {
	initUSART();

	uint16_t count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", tests, count);

	return 0;
}
