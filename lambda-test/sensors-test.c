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
const char sensors_P[] PROGMEM = "sensors";

/* Test names */
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

/* Tests */
TestCase const sensorsTests[] = {
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
};
