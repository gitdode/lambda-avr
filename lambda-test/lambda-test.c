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
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "USART.h"
#include "avrjunit.h"
#include "interrupts.h"
#include "adc.h"
#include "alert.h"
#include "integers.h"
#include "sensors.h"
#include "display.h"
#include "pins.h"
#include "command.h"
#include "strings.h"

static TableEntry const testTable[] = {
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

bool testCycle(void) {
	extern uint8_t position;
	extern bool updatePending;

	extern uint8_t beepCount;
	extern uint16_t beepLength;

	updatePending = false;

	assertTrue(position == 0);
	assertFalse(updatePending);

	cycleDisplay();
	assertTrue(position == 1);
	assertTrue(updatePending);
	assertTrue(beepCount == 1);
	assertTrue(beepLength == 2);

	cycleDisplay();
	assertTrue(position == 0);

	return true;
}

bool testCycleCancelAlert(void) {
	extern uint8_t position;
	extern bool updatePending;

	updatePending = false;

	alert(1, 1, "", "");
	assertTrue(isAlertActive());

	cycleDisplay();
	assertFalse(isAlertActive());
	assertTrue(position == 0);
	assertTrue(updatePending);

	return true;
}

bool testUpdateMeas(void) {
	extern bool updatePending;
	extern Measurement measLatest;
	extern Measurement measMax; // = {0, 0, 2000};

	updatePending = false;

	// initial measurements
	assertTrue(measLatest.tempI == 0);
	assertTrue(measLatest.tempO == 0);
	assertTrue(measLatest.lambda == 0);
	// initial max measurements
	assertTrue(measMax.tempI == 0);
	assertTrue(measMax.tempO == 0);
	assertTrue(measMax.lambda == 2000);

	Measurement meas1 = {1, 2, 3};
	updateMeas(meas1);
	// updated measurements
	assertTrue(measLatest.tempI == 1);
	assertTrue(measLatest.tempO == 2);
	assertTrue(measLatest.lambda == 3);
	// updated max measurements
	assertTrue(measMax.tempI == 1);
	assertTrue(measMax.tempO == 2);
	assertTrue(measMax.lambda == 3);
	assertTrue(updatePending);

	Measurement meas2 = {0, 0, 10};
	updateMeas(meas2);
	// updated max measurements
	assertTrue(measMax.tempI == 1);
	assertTrue(measMax.tempO == 2);
	assertTrue(measMax.lambda == 3);

	return true;
}

bool testResetMeas(void) {
	extern bool updatePending;
	extern Measurement measLatest;
	extern Measurement measMax;

	updatePending = false;

	resetMeas();
	// reset max measurements
	assertTrue(measMax.tempI == 0);
	assertTrue(measMax.tempO == 0);
	assertTrue(measMax.lambda == 2000);
	assertTrue(updatePending);

	return true;
}

bool testUpdateDisplayIfPending(void) {
	extern bool updatePending;

	updatePending = true;

	updateDisplayIfPending();
	assertFalse(updatePending);

	return true;
}

bool testUpdateDisplayIfPendingAlertActive(void) {
	extern bool updatePending;

	updatePending = true;
	alert(1, 1, "", "");
	assertTrue(isAlertActive());

	// update should be skipped if alert is active
	updateDisplayIfPending();
	assertTrue(updatePending);

	return true;
}

bool testDisplayText(void) {
	// won't actually display anything
	displayText("testDisplayText", "testDisplayTextLineTooLong");

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
	initTimers();

	// timer0 clock prescaler /64 = 15.625 kHz overflowing every 16.2 ms
	uint8_t prescalerBy64 = (1 << CS00) | (1 << CS01);
	assertTrue((TCCR0B & prescalerBy64) == prescalerBy64);

	// timer1 Clear Timer on Compare Match mode, TOP OCR1A
	assertTrue(bit_is_set(TCCR1B, WGM12));
	// timer1 clock prescaler/8
	assertTrue(bit_is_set(TCCR1B, CS11));
	// toggles PB1 at 7.8 kHz generating a 3.9 kHz beep
	// assertTrue(OCR1A == 15);
	// 1.8 kHz is less noisy on the small piezo beeper
	assertTrue(OCR1A == 31);

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
	Measurement meas;
	for (uint8_t i = 0; i < 64; i++) {
		meas = measure();
	}

	// verify that temperatures and lambda are calculated correctly
	// for voltages > 4950 and <= 5000 mV
	assertTrue(meas.tempI >= 990 && meas.tempI <= 1000);
	assertTrue(meas.tempO == 400);
	assertTrue(meas.lambda == 997);

	return true;
}

bool testReadMeas(void) {
	char* fields[] = {"1", "2", "3"};

	Measurement meas = readMeas(fields, 3);
	assertTrue(meas.tempI == 1);
	assertTrue(meas.tempO == 2);
	assertTrue(meas.lambda == 3);

	return true;
}

bool testReadMeasTooFewFields(void) {
	char* fields[] = {"1"};

	Measurement meas = readMeas(fields, 1);
	assertTrue(meas.tempI == 0);
	assertTrue(meas.tempO == 0);
	assertTrue(meas.lambda == 0);

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
	char* info = toInfo(191);

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
	char* info = toInfo(129);

	return ! strcmp(info, RICH);
}

/* Module strings */

bool testSplit(void) {
	char string[] = "f1 f2  f3 ";
	char* fields[4];
	split(string, " ", fields, 4);

	assertTrue(strcmp("f1", fields[0]) == 0);
	assertTrue(strcmp("f2", fields[1]) == 0);
	assertTrue(strcmp("f3", fields[2]) == 0);
	assertTrue(strcmp("", fields[3]) == 0);

	return true;
}

bool testSplitSizeTooSmall(void) {
	char string[] = "f1 f2";
	char* fields[1];
	split(string, " ", fields, 1);

	assertTrue(strcmp("f1", fields[0]) == 0);

	return true;
}

/* Test "classes" */
const char adc_P[] PROGMEM = "adc";
const char command_P[] PROGMEM = "command";
const char display_P[] PROGMEM = "display";
const char integers_P[] PROGMEM = "integers";
const char interrupts_P[] PROGMEM = "interrupts";
const char sensors_P[] PROGMEM = "sensors";
const char strings_P[] PROGMEM = "strings";

/* Test names */
const char testSetupADC_P[] PROGMEM = "testSetupADC";
const char testGetVoltage_P[] PROGMEM = "testGetVoltage";
const char testIsSimulation_P[] PROGMEM = "testIsSimulation";
const char testIsLogging_P[] PROGMEM = "testIsLogging";
const char testCycle_P[] PROGMEM = "testCycle";
const char testCycleCancelAlert_P[] PROGMEM = "testCycleCancelAlert";
const char testUpdateMeas_P[] PROGMEM = "testUpdateMeas";
const char testResetMeas_P[] PROGMEM = "testResetMeas";
const char testUpdateDisplayIfPending_P[] PROGMEM = "testUpdateDisplayIfPending";
const char testUpdateDisplayIfPendingAlertActive_P[] PROGMEM = "testUpdateDisplayIfPendingAlertActive";
const char testDisplayText_P[] PROGMEM = "testDisplayText";
const char testDivRoundNearest_P[] PROGMEM = "testDivRoundNearest";
const char testDivRoundNearestNumNeg_P[] PROGMEM = "testDivRoundNearestNumNeg";
const char testDivRoundNearestDenNeg_P[] PROGMEM = "testDivRoundNearestDenNeg";
const char testDivRoundNearestBothNeg_P[] PROGMEM = "testDivRoundNearestBothNeg";
const char testDivRoundUp_P[] PROGMEM = "testDivRoundUp";
const char testDivRoundUpNumNeg_P[] PROGMEM = "testDivRoundUpNumNeg";
const char testDivRoundUpDenNeg_P[] PROGMEM = "testDivRoundUpDenNeg";
const char testDivRoundUpBothNeg_P[] PROGMEM = "testDivRoundUpBothNeg";
const char testSetupPorts_P[] PROGMEM = "testSetupPorts";
const char testSetupSleepMode_P[] PROGMEM = "testSetupSleepMode";
const char testInitInterrupts_P[] PROGMEM = "testInitInterrupts";
const char testInitTimers_P[] PROGMEM = "testInitTimers";
const char testMeasure_P[] PROGMEM = "testMeasure";
const char testReadMeas_P[] PROGMEM = "testReadMeas";
const char testReadMeasTooFewFields_P[] PROGMEM = "testReadMeasTooFewFields";
const char testToLambdaValue_P[] PROGMEM = "testToLambdaValue";
const char testToLambdaInter_P[] PROGMEM = "testToLambdaInter";
const char testToTempI_P[] PROGMEM = "testToTempI";
const char testToTempOValue_P[] PROGMEM = "testToTempOValue";
const char testToTempOInter_P[] PROGMEM = "testToTempOInter";
const char testLookupLinInterValue_P[] PROGMEM = "testLookupLinInterValue";
const char testLookupLinInterInter_P[] PROGMEM = "testLookupLinInterInter";
const char testLookupLinInterBelow_P[] PROGMEM = "testLookupLinInterBelow";
const char testLookupLinInterAbove_P[] PROGMEM = "testLookupLinInterAbove";
const char testToInfoLean_P[] PROGMEM = "testToInfoLean";
const char testToInfoOkay_P[] PROGMEM = "testToInfoOkay";
const char testToInfoIdeal_P[] PROGMEM = "testToInfoIdeal";
const char testToInfoRich_P[] PROGMEM = "testToInfoRich";
const char testSplit_P[] PROGMEM = "testSplit";
const char testSplitSizeTooSmall_P[] PROGMEM = "testSplitSizeTooSmall";

/* Tests */
TestCase const tests[] = {
		{adc_P, testSetupADC_P, testSetupADC},
		{adc_P, testGetVoltage_P, testGetVoltage},
		{command_P, testIsSimulation_P, testIsSimulation},
		{command_P, testIsLogging_P, testIsLogging},
		{display_P, testCycle_P, testCycle},
		{display_P, testCycleCancelAlert_P, testCycleCancelAlert},
		{display_P, testUpdateMeas_P, testUpdateMeas},
		{display_P, testResetMeas_P, testResetMeas},
		{display_P, testUpdateDisplayIfPending_P, testUpdateDisplayIfPending},
		{display_P, testUpdateDisplayIfPendingAlertActive_P, testUpdateDisplayIfPendingAlertActive},
		{display_P, testDisplayText_P, testDisplayText},
		{integers_P, testDivRoundNearest_P, testDivRoundNearest},
		{integers_P, testDivRoundNearestNumNeg_P, testDivRoundNearestNumNeg},
		{integers_P, testDivRoundNearestDenNeg_P, testDivRoundNearestDenNeg},
		{integers_P, testDivRoundNearestBothNeg_P, testDivRoundNearestBothNeg},
		{integers_P, testDivRoundUp_P, testDivRoundUp},
		{integers_P, testDivRoundUpNumNeg_P, testDivRoundUpNumNeg},
		{integers_P, testDivRoundUpDenNeg_P, testDivRoundUpDenNeg},
		{integers_P, testDivRoundUpBothNeg_P, testDivRoundUpBothNeg},
		{interrupts_P, testSetupPorts_P, testSetupPorts},
		{interrupts_P, testSetupSleepMode_P, testSetupSleepMode},
		{interrupts_P, testInitInterrupts_P, testInitInterrupts},
		{interrupts_P, testInitTimers_P, testInitTimers},
		{sensors_P, testMeasure_P, testMeasure},
		{sensors_P, testReadMeas_P, testReadMeas},
		{sensors_P, testReadMeasTooFewFields_P, testReadMeasTooFewFields},
		{sensors_P, testToLambdaValue_P, testToLambdaValue},
		{sensors_P, testToLambdaInter_P, testToLambdaInter},
		{sensors_P, testToTempI_P, testToTempI},
		{sensors_P, testToTempOValue_P, testToTempOValue},
		{sensors_P, testToTempOInter_P, testToTempOInter},
		{sensors_P, testLookupLinInterValue_P, testLookupLinInterValue},
		{sensors_P, testLookupLinInterInter_P, testLookupLinInterInter},
		{sensors_P, testLookupLinInterBelow_P, testLookupLinInterBelow},
		{sensors_P, testLookupLinInterAbove_P, testLookupLinInterAbove},
		{sensors_P, testToInfoLean_P, testToInfoLean},
		{sensors_P, testToInfoOkay_P, testToInfoOkay},
		{sensors_P, testToInfoIdeal_P, testToInfoIdeal},
		{sensors_P, testToInfoRich_P, testToInfoRich},
		{strings_P, testSplit_P, testSplit},
		{strings_P, testSplitSizeTooSmall_P, testSplitSizeTooSmall}
};

int main(void) {
	initUSART();

	size_t count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", tests, count);

	return 0;
}
