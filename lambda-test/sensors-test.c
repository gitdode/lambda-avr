/*
 * sensors-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdio.h>
#include <string.h>
#include "avrjunit.h"
#include "interrupts.h"
#include "adc.h"
#include "sensors.h"
#include "pins.h"
#include "messages.h"

#include "usart.h"

static TableEntry const testTable[] = {
		{10, 10},
		{20, 20}
};

/* Module sensors */

static bool testMeasure(void) {
	setupADC();
	setupSleepMode();
	initInterrupts();

	// enable pull-up resistor so the measured voltage
	// should be close to AREF
	PORTC |= ((1 << ADC_TEMPI) | (1 << ADC_TEMPO) | (1 << ADC_LAMBDA) |
			(1 << ADC_HEATER));

	setHeaterState(heaterStateReady);

	// do many measurements so the averaged voltages are near the measured
	// voltages (close to AREF)
	Measurement meas;
	for (uint8_t i = 0; i < 64; i++) {
		meas = measure();
	}

	// verify that temperatures, lambda and current are calculated correctly
	// for voltages > 4900 and <= 5000 mV
	assertTrue(meas.tempI >= 980 && meas.tempI <= 1000);
	assertTrue(meas.tempO == 400);
	assertTrue(meas.lambda >= 995 && meas.lambda <= 999);
	assertTrue(meas.current >= 49500 && meas.current <= 50000);

	return true;
}

static bool testReadMeas(void) {
	char* fields[] = {"1", "2", "3", "4"};

	Measurement meas = readMeas(fields, 4);
	assertTrue(meas.tempI == 1);
	assertTrue(meas.tempO == 2);
	assertTrue(meas.lambda == 3);
	assertTrue(meas.current == 4);

	return true;
}

static bool testReadMeasTooFewFields(void) {
	char* fields[] = {"1"};

	Measurement meas = readMeas(fields, 1);
	assertTrue(meas.tempI == 0);
	assertTrue(meas.tempO == 0);
	assertTrue(meas.lambda == 0);
	assertTrue(meas.current == 0);

	return true;
}

static bool testToLambdaValue(void) {
	uint16_t lambda = toLambda(132);

	return lambda == 1500;
}

static bool testToLambdaInter(void) {
	uint16_t lambda = toLambda(550);

	return lambda == 1073;
}

static bool testToTempI(void) {
	int16_t temp = toTempI(100);

	return temp == 20;
}

static bool testToTempOValue(void) {
	int16_t temp = toTempO(454);

	return temp == 0;
}

static bool testToTempOInter(void) {
	int16_t temp = toTempO(929);

	return temp == 50;
}

static bool testToCurrent(void) {
	uint16_t current = toCurrent(150);

	return current == 1500 / SHUNT_MILLIOHMS * 100;
}

static bool testLookupLinInterBelow(void) {
	int16_t value = lookupLinInter(0, testTable, 2);

	return value == 10;
}

static bool testLookupLinInterAbove(void) {
	int16_t value = lookupLinInter(30, testTable, 2);

	return value == 20;
}

static bool testLookupLinInterValue(void) {
	int16_t value = lookupLinInter(10, testTable, 2);

	return value == 10;
}

static bool testLookupLinInterInter(void) {
	int16_t value = lookupLinInter(15, testTable, 2);

	return value == 15;
}

static bool testLinADCLow(void) {
	int32_t lin = linADC(1000);

	return lin == 1004;
}

static bool testLinADCMid(void) {
	int32_t lin = linADC(2000);

	return lin == 2000;
}

static bool testLinADCHigh(void) {
	int32_t lin = linADC(4000);

	return lin == 3992;
}

static bool testToInfoLean(void) {
	char* info = toInfo(191);

	return ! strcmp(info, MSG_LEAN);
}

static bool testToInfoOkay(void) {
	assertTrue(0 == strcmp(toInfo(190), MSG_OKAY));
	assertTrue(0 == strcmp(toInfo(170), MSG_OKAY));
	assertTrue(0 == strcmp(toInfo(151), MSG_OKAY));

	return true;
}

static bool testToInfoIdeal(void) {
	assertTrue(0 == strcmp(toInfo(150), MSG_IDEAL));
	assertTrue(0 == strcmp(toInfo(140), MSG_IDEAL));
	assertTrue(0 == strcmp(toInfo(130), MSG_IDEAL));

	return true;
}

static bool testToInfoRich(void) {
	char* info = toInfo(129);

	return ! strcmp(info, MSG_RICH);
}

static bool testSetHeaterOn(void) {
	setHeaterOn(true);
	assertTrue(isHeaterOn());
	assertTrue(heaterStateUp == getHeaterState());

	setHeaterOn(false);
	assertFalse(isHeaterOn());
	assertTrue(heaterStateOff == getHeaterState());

	return true;
}

static bool testSetHeaterState(void) {
	setHeaterOn(false);
	assertFalse(isHeaterOn());
	assertTrue(heaterStateOff == getHeaterState());

	setHeaterState(heaterStateFault);
	assertTrue(heaterStateFault == getHeaterState());

	return true;
}

static bool testGetHeaterUptime(void) {
	addTime(10);

	setHeaterOn(true);
	assertTrue(0 == getHeaterUptime());

	addTime(10);
	assertTrue(10 == getHeaterUptime());

	return true;
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
static const char testToCurrent_P[] PROGMEM = "testToCurrent";
static const char testLookupLinInterValue_P[] PROGMEM = "testLookupLinInterValue";
static const char testLookupLinInterInter_P[] PROGMEM = "testLookupLinInterInter";
static const char testLookupLinInterBelow_P[] PROGMEM = "testLookupLinInterBelow";
static const char testLookupLinInterAbove_P[] PROGMEM = "testLookupLinInterAbove";
static const char testLinADCLow_P[] PROGMEM = "testLinADCLow";
static const char testLinADCMid_P[] PROGMEM = "testLinADCMid";
static const char testLinADCHigh_P[] PROGMEM = "testLinADCHigh";
static const char testToInfoLean_P[] PROGMEM = "testToInfoLean";
static const char testToInfoOkay_P[] PROGMEM = "testToInfoOkay";
static const char testToInfoIdeal_P[] PROGMEM = "testToInfoIdeal";
static const char testToInfoRich_P[] PROGMEM = "testToInfoRich";
static const char testSetHeaterOn_P[] PROGMEM = "testSetHeaterOn";
static const char testSetHeaterState_P[] PROGMEM = "testSetHeaterState";
static const char testGetHeaterUptime_P[] PROGMEM = "testGetHeaterUptime";

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
		{class, testToCurrent_P, testToCurrent},
		{class, testLookupLinInterValue_P, testLookupLinInterValue},
		{class, testLookupLinInterInter_P, testLookupLinInterInter},
		{class, testLookupLinInterBelow_P, testLookupLinInterBelow},
		{class, testLookupLinInterAbove_P, testLookupLinInterAbove},
		{class, testLinADCLow_P, testLinADCLow},
		{class, testLinADCMid_P, testLinADCMid},
		{class, testLinADCHigh_P, testLinADCHigh},
		{class, testToInfoLean_P, testToInfoLean},
		{class, testToInfoOkay_P, testToInfoOkay},
		{class, testToInfoIdeal_P, testToInfoIdeal},
		{class, testToInfoRich_P, testToInfoRich},
		{class, testSetHeaterOn_P, testSetHeaterOn},
		{class, testSetHeaterState_P, testSetHeaterState},
		{class, testGetHeaterUptime_P, testGetHeaterUptime}
};

TestClass sensorsClass = {tests, sizeof(tests) / sizeof(tests[0])};
