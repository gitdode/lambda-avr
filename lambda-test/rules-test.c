/*
 * rules-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 28.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "string.h"
#include "avrjunit.h"
#include "rules.h"
#include "alert.h"
#include "interrupts.h"

/* Module rules */

extern uint8_t measCount;
extern int8_t state;
extern uint8_t airgate;
extern Rule rules[];

static bool testAirgate50(void) {

	Measurement meas = {0, 0, 0, 0};

	resetRules(true);
	state = burning_down;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	state = undefined;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	state = firing_up;
	reason(meas);
	assertFalse(rules[0].fired);

	meas.tempI = TEMP_AIRGATE_50;
	meas.lambda = LAMBDA_TOO_LEAN;

	resetRules(true);
	state = burning_down;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	state = undefined;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	state = firing_up;
	reason(meas);
	assertTrue(rules[0].fired);
	assertTrue(50 == airgate);

	// should not fire if airgate == 50
	resetRules(false);
	state = firing_up;
	reason(meas);
	assertFalse(rules[0].fired);
	assertTrue(50 == airgate);

	cancelAlert();

	return true;
}

static bool testAirgate25(void) {

	Measurement meas = {999, 0, 0, 0};

	resetRules(true);
	state = firing_up;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	state = undefined;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	state = burning_down;
	reason(meas);
	assertFalse(rules[1].fired);

	meas.tempI = TEMP_AIRGATE_25 - 1;
	meas.lambda = LAMBDA_TOO_LEAN;

	resetRules(true);
	state = firing_up;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	state = undefined;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	state = burning_down;
	reason(meas);
	assertTrue(rules[1].fired);
	assertTrue(25 == airgate);

	cancelAlert();

	return true;
}

static bool testAirgateClose(void) {

	Measurement meas = {999, 0, 0, 0};

	resetRules(true);
	state = firing_up;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	state = undefined;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	state = burning_down;
	reason(meas);
	assertFalse(rules[2].fired);

	meas.tempI = TEMP_AIRGATE_0 - 1;
	meas.lambda = LAMBDA_MAX;

	resetRules(true);
	state = firing_up;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	state = undefined;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	state = burning_down;
	reason(meas);
	assertTrue(rules[2].fired);
	assertTrue(0 == airgate);

	cancelAlert();

	return true;
}

static bool testTooRich(void) {

	Measurement meas = {0, 0, 0, 1000};

	setHeaterState(heaterStateOn);

	meas.tempI = TEMP_FIRE_OUT;
	meas.lambda = LAMBDA_MAX;
	resetRules(true);
	reason(meas);
	assertFalse(rules[3].fired);

	meas.tempI = TEMP_FIRE_OUT + 1;
	meas.lambda = LAMBDA_MAX;
	resetRules(true);
	reason(meas);
	assertFalse(rules[3].fired);

	meas.tempI = TEMP_FIRE_OUT;
	meas.lambda = LAMBDA_TOO_RICH - 1;
	resetRules(true);
	reason(meas);
	assertFalse(rules[3].fired);

	meas.tempI = TEMP_FIRE_OUT + 1;
	meas.lambda = LAMBDA_TOO_RICH - 1;
	resetRules(true);
	airgate = 50;
	reason(meas);
	assertFalse(rules[3].fired);
	assertTrue(50 == airgate);

	meas.tempI = TEMP_FIRE_OUT + 1;
	meas.lambda = LAMBDA_TOO_RICH - 1;
	resetRules(true);
	airgate = 25;
	reason(meas);
	assertTrue(rules[3].fired);
	assertTrue(50 == airgate);

	cancelAlert();

	return true;
}

static bool testTooLean(void) {

	Measurement meas = {0, 0, 0, 1000};

	setHeaterState(heaterStateOn);

	meas.tempI = TEMP_AIRGATE_50;
	meas.lambda = LAMBDA_TOO_LEAN + 1;
	resetRules(true);
	reason(meas);
	assertFalse(rules[4].fired);

	meas.tempI = TEMP_AIRGATE_50 + 1;
	meas.lambda = 1300;
	resetRules(true);
	reason(meas);
	assertFalse(rules[4].fired);

	meas.tempI = TEMP_AIRGATE_50 + 1;
	meas.lambda = LAMBDA_TOO_LEAN + 1;
	resetRules(true);
	airgate = 50;
	reason(meas);
	assertFalse(rules[4].fired);
	assertTrue(50 == airgate);

	meas.tempI = TEMP_AIRGATE_50 + 1;
	meas.lambda = LAMBDA_TOO_LEAN + 1;
	resetRules(true);
	airgate = 100;
	reason(meas);
	assertTrue(rules[4].fired);
	assertTrue(50 == airgate);

	cancelAlert();

	return true;
}

static bool testFireOut(void) {

	Measurement meas = {0, 0, 0, 0};

	resetRules(true);
	state = firing_up;

	meas.tempI = 50;
	measCount = 10;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT;
	measCount = 10;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT_RESET;
	measCount = 10;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT - 1;
	measCount = 10;
	reason(meas);
	assertTrue(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT_RESET;
	measCount = 10;
	reason(meas);
	assertFalse(rules[5].fired);

	cancelAlert();

	return true;
}

static bool testWarmStart(void) {

	Measurement meas = {TEMP_AIRGATE_50, 0, 0, 0};

	setHeaterState(heaterStateOff);

	resetRules(true);
	rules[6].fired = false;
	airgate = 50;
	reason(meas);
	assertTrue(50 == airgate);
	assertTrue(heaterStateOff == getHeaterState());
	assertFalse(rules[6].fired);

	resetRules(true);
	rules[6].fired = false;
	state = firing_up;
	reason(meas);
	assertTrue(100 == airgate);
	assertTrue(heaterStateOn == getHeaterState());
	assertTrue(rules[6].fired);

	cancelAlert();

	return true;
}

static bool testHeaterReady(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};

	setHeaterState(heaterStateOff);
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	setHeaterState(heaterStateOn);
	meas.current = 5000;
	reason(meas);
	assertTrue(heaterStateOn == getHeaterState());

	setHeaterState(heaterStateOn);
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateReady == getHeaterState());

	cancelAlert();

	return true;
}

static bool testHeaterFaultNoconn(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};
	state = firing_up;

	setHeaterState(heaterStateOn);
	meas.current = 0;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert();

	return true;
}

static bool testHeaterFaultShort(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};
	state = firing_up;

	setHeaterState(heaterStateOn);
	meas.current = 8000;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert();

	return true;
}

static bool testHeaterFaultNoheat(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};
	state = firing_up;

	setHeaterState(heaterStateOn);

	// more than 5 mins
	addTime(301);

	meas.current = 5000;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert();

	return true;
}

static bool testHeaterTimeout0(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};

	setHeaterState(heaterStateOn);

	// equal or more than 30 mins below TEMP_FIRE_OUT
	addTime(1800);

	meas.tempI = TEMP_FIRE_OUT - 1;
	meas.lambda = LAMBDA_MAX;
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	cancelAlert();

	return true;
}

static bool testHeaterTimeout1(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};

	setHeaterState(heaterStateOn);

	// equal or more than 3 hours below TEMP_AIRGATE_0
	addTime(10800UL);

	meas.tempI = TEMP_AIRGATE_0 - 1;
	meas.lambda = LAMBDA_MAX;
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	cancelAlert();

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "rules";

/* Test names */
static const char testAirgate50_P[] PROGMEM = "testAirgate50";
static const char testAirgate25_P[] PROGMEM = "testAirgate25";
static const char testAirgateClose_P[] PROGMEM = "testAirgateClose";
static const char testTooRich_P[] PROGMEM = "testTooRich";
static const char testTooLean_P[] PROGMEM = "testTooLean";
static const char testFireOut_P[] PROGMEM = "testFireOut";
static const char testWarmStart_P[] PROGMEM = "testWarmStart";
static const char testHeaterReady_P[] PROGMEM = "testHeaterReady";
static const char testHeaterFaultNoconn_P[] PROGMEM = "testHeaterFaultNoconn";
static const char testHeaterFaultShort_P[] PROGMEM = "testHeaterFaultShort";
static const char testHeaterFaultNoheat_P[] PROGMEM = "testHeaterFaultNoheat";
static const char testHeaterTimeout0_P[] PROGMEM = "testHeaterTimeout0";
static const char testHeaterTimeout1_P[] PROGMEM = "testHeaterTimeout1";

/* Tests */
static TestCase const tests[] = {
 		{class, testAirgate50_P, testAirgate50},
		{class, testAirgate25_P, testAirgate25},
		{class, testAirgateClose_P, testAirgateClose},
		{class, testTooRich_P, testTooRich},
		{class, testTooLean_P, testTooLean},
 		{class, testFireOut_P, testFireOut},
		{class, testWarmStart_P, testWarmStart},
		{class, testHeaterReady_P, testHeaterReady},
		{class, testHeaterFaultNoconn_P, testHeaterFaultNoconn},
		{class, testHeaterFaultShort_P, testHeaterFaultShort},
		{class, testHeaterFaultNoheat_P, testHeaterFaultNoheat},
		{class, testHeaterTimeout0_P, testHeaterTimeout0},
		{class, testHeaterTimeout1_P, testHeaterTimeout1}
};

TestClass rulesClass = {tests, sizeof(tests) / sizeof(tests[0])};
