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

extern uint16_t age;
extern int8_t dir;
extern uint8_t airgate;
extern Rule rules[];

static bool testAirgate50(void) {

	Measurement meas = {0, 0, 0, 0};

	resetRules(true);
	dir = burning_down;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	dir = none;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	dir = firing_up;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	dir = burning;
	reason(meas);
	assertFalse(rules[0].fired);

	meas.tempI = TEMP_AIRGATE_50;
	meas.lambda = LAMBDA_TOO_LEAN;

	resetRules(true);
	dir = burning_down;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	dir = none;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules(true);
	dir = firing_up;
	reason(meas);
	assertTrue(rules[0].fired);
	assertTrue(50 == airgate);

	// should not fire if airgate == 50
	resetRules(false);
	dir = firing_up;
	reason(meas);
	assertFalse(rules[0].fired);
	assertTrue(50 == airgate);

	resetRules(true);
	dir = burning;
	reason(meas);
	assertTrue(rules[0].fired);
	assertTrue(50 == airgate);

	cancelAlert(false);

	return true;
}

static bool testAirgate25(void) {

	Measurement meas = {999, 0, 0, 0};

	resetRules(true);
	dir = firing_up;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	dir = none;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	dir = burning_down;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	dir = burning;
	reason(meas);
	assertFalse(rules[1].fired);

	meas.tempI = TEMP_AIRGATE_25 - 1;
	meas.lambda = LAMBDA_TOO_LEAN;

	resetRules(true);
	dir = firing_up;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	dir = none;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	dir = burning;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules(true);
	dir = burning_down;
	reason(meas);
	assertTrue(rules[1].fired);
	assertTrue(25 == airgate);

	cancelAlert(false);

	return true;
}

static bool testAirgateClose(void) {

	Measurement meas = {999, 0, 0, 0};

	resetRules(true);
	dir = firing_up;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	dir = none;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	dir = burning_down;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	dir = burning;
	reason(meas);
	assertFalse(rules[2].fired);

	meas.tempI = TEMP_AIRGATE_0 - 1;
	meas.lambda = LAMBDA_MAX;

	resetRules(true);
	dir = firing_up;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	dir = none;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	dir = burning;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules(true);
	dir = burning_down;
	reason(meas);
	assertTrue(rules[2].fired);
	assertTrue(0 == airgate);

	cancelAlert(false);

	return true;
}

static bool testTooRich(void) {

	Measurement meas = {0, 0, 0, 0};
	dir = none;

	setHeaterState(heaterStateReady);

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

	cancelAlert(false);

	return true;
}

static bool testTooLean(void) {

	Measurement meas = {0, 0, 0, 0};
	dir = none;

	setHeaterState(heaterStateReady);

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

	cancelAlert(false);

	return true;
}

static bool testFireOut(void) {

	resetRules(true);
	Measurement meas = {0, 0, 0, 0};
	dir = firing_up;

	meas.tempI = 50;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT_RESET;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT - 1;
	age = 0;
	reason(meas);
	assertTrue(rules[5].fired);

	meas.tempI = TEMP_FIRE_OUT_RESET;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	cancelAlert(false);

	return true;
}

static bool testWarmStart(void) {

	resetRules(true);
	Measurement meas = {0, 0, 0, 0};

	age = 0;
	airgate = 50;

	setHeaterOn(false);
	reason(meas);
	assertTrue(50 == airgate);
	assertTrue(heaterStateOff == getHeaterState());
	assertFalse(rules[6].fired);

	age = 0;
	dir = firing_up;

	setHeaterOn(false);
	reason(meas);
	assertTrue(100 == airgate);
	assertTrue(heaterStateUp == getHeaterState());
	assertTrue(rules[6].fired);

	cancelAlert(false);

	return true;
}

static bool testHeaterReady(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};

	setHeaterOn(false);
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	setHeaterOn(true);
	meas.current = 5000;
	reason(meas);
	assertTrue(heaterStateUp == getHeaterState());

	setHeaterOn(true);
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateReady == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterFaultNoconn(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};
	dir = firing_up;

	setHeaterOn(true);
	meas.current = 0;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterFaultShort(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};
	dir = firing_up;

	setHeaterOn(true);
	meas.current = 8000;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterFaultNoheat(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};
	dir = firing_up;

	setHeaterOn(true);

	// more than 5 mins
	addTime(301);

	meas.current = 5000;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterTimeout0(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};

	setHeaterOn(true);

	// equal or more than 30 mins below TEMP_FIRE_OUT
	addTime(1800);

	meas.tempI = TEMP_FIRE_OUT - 1;
	meas.lambda = LAMBDA_MAX;
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterTimeout1(void) {

	resetRules(true);
	resetTime();
	Measurement meas = {0, 0, 0, 0};

	setHeaterOn(true);

	// equal or more than 3 hours below TEMP_AIRGATE_0
	addTime(10800UL);

	meas.tempI = TEMP_AIRGATE_0 - 1;
	meas.lambda = LAMBDA_MAX;
	meas.current = milliAmpsReady;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testReasonDirBurnUp(void) {

	resetRules(true);
	Measurement meas = {0, 0, 2000, 0};

	age = 0;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 9;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = TEMP_FIRE_OUT;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == firing_up);

	meas.tempI = TEMP_AIRGATE_25 - 1;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == firing_up);

	meas.tempI = TEMP_AIRGATE_25 - 1;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = TEMP_AIRGATE_25;
	meas.lambda = LAMBDA_BURNING - 1;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = TEMP_AIRGATE_25 + 1;
	meas.lambda = LAMBDA_BURNING;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	cancelAlert(false);

	return true;
}

static bool testReasonDirBurnDown(void) {

	resetRules(true);
	Measurement meas = {999, 0, 1999, 0};

	age = 0;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 800;
	meas.lambda = LAMBDA_BURNING - 1;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = TEMP_AIRGATE_25;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = TEMP_AIRGATE_25 - 1;
	meas.lambda = LAMBDA_BURNING - 1;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = TEMP_AIRGATE_25 - 1;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = TEMP_FIRE_OUT;
	meas.lambda = LAMBDA_MAX;
	age = 180;
	reason(meas);
	assertTrue(dir == burning_down);

	cancelAlert(false);

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
static const char testReasonDirBurnUp_P[] PROGMEM = "testReasonDirBurnUp";
static const char testReasonDirBurnDown_P[] PROGMEM = "testReasonDirBurnDown";

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
		{class, testHeaterTimeout1_P, testHeaterTimeout1},
		{class, testReasonDirBurnUp_P, testReasonDirBurnUp},
		{class, testReasonDirBurnDown_P, testReasonDirBurnDown}
};

TestClass rulesClass = {tests, sizeof(tests) / sizeof(tests[0])};
