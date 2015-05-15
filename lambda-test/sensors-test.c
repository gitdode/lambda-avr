/*
 * lambda-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include <string.h>
#include "avrjunit.h"
#include "interrupts.h"
#include "adc.h"
#include "sensors.h"
#include "pins.h"

static TableEntry const testTable[] = {
		{10, 10},
		{20, 20}
};

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

/* Test "class" */
static const char class[] PROGMEM = "sensors";

/* Test names */
static const char testMeasure_P[] PROGMEM = "testMeasure";
static const char testReadMeas_P[] PROGMEM = "testReadMeas";
static const char testReadMeasTooFewFields_P[] PROGMEM = "testReadMeasTooFewFields";
static const char testToLambdaValue_P[] PROGMEM = "testToLambdaValue";
static const char testToLambdaInter_P[] PROGMEM = "testToLambdaInter";
static const char testToTempI_P[] PROGMEM = "testToTempI";
static const char testToTempOValue_P[] PROGMEM = "testToTempOValue";
static const char testToTempOInter_P[] PROGMEM = "testToTempOInter";
static const char testLookupLinInterValue_P[] PROGMEM = "testLookupLinInterValue";
static const char testLookupLinInterInter_P[] PROGMEM = "testLookupLinInterInter";
static const char testLookupLinInterBelow_P[] PROGMEM = "testLookupLinInterBelow";
static const char testLookupLinInterAbove_P[] PROGMEM = "testLookupLinInterAbove";
static const char testToInfoLean_P[] PROGMEM = "testToInfoLean";
static const char testToInfoOkay_P[] PROGMEM = "testToInfoOkay";
static const char testToInfoIdeal_P[] PROGMEM = "testToInfoIdeal";
static const char testToInfoRich_P[] PROGMEM = "testToInfoRich";

/* Tests */
static TestCase const tests[] = {
		{class, testMeasure_P, testMeasure},
		{class, testReadMeas_P, testReadMeas},
		{class, testReadMeasTooFewFields_P, testReadMeasTooFewFields},
		{class, testToLambdaValue_P, testToLambdaValue},
		{class, testToLambdaInter_P, testToLambdaInter},
		{class, testToTempI_P, testToTempI},
		{class, testToTempOValue_P, testToTempOValue},
		{class, testToTempOInter_P, testToTempOInter},
		{class, testLookupLinInterValue_P, testLookupLinInterValue},
		{class, testLookupLinInterInter_P, testLookupLinInterInter},
		{class, testLookupLinInterBelow_P, testLookupLinInterBelow},
		{class, testLookupLinInterAbove_P, testLookupLinInterAbove},
		{class, testToInfoLean_P, testToInfoLean},
		{class, testToInfoOkay_P, testToInfoOkay},
		{class, testToInfoIdeal_P, testToInfoIdeal},
		{class, testToInfoRich_P, testToInfoRich},
};

TestClass sensorsClass = {tests, sizeof(tests) / sizeof(tests[0])};
