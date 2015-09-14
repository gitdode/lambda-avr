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

	Measurement meas = {0, 0, 0};

	resetRules();
	dir = burning_down;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules();
	dir = none;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules();
	dir = firing_up;
	reason(meas);
	assertFalse(rules[0].fired);

	meas.tempI = 500;

	resetRules();
	dir = burning_down;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules();
	dir = none;
	reason(meas);
	assertFalse(rules[0].fired);

	resetRules();
	dir = firing_up;
	reason(meas);
	assertTrue(rules[0].fired);
	assertTrue(50 == airgate);

	cancelAlert(false);

	return true;
}

static bool testAirgate25(void) {

	Measurement meas = {999, 0, 0};

	resetRules();
	dir = firing_up;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules();
	dir = none;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules();
	dir = burning_down;
	reason(meas);
	assertFalse(rules[1].fired);

	meas.tempI = 699;

	resetRules();
	dir = firing_up;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules();
	dir = none;
	reason(meas);
	assertFalse(rules[1].fired);

	resetRules();
	dir = burning_down;
	reason(meas);
	assertTrue(rules[1].fired);
	assertTrue(25 == airgate);

	cancelAlert(false);

	return true;
}

static bool testAirgateClose(void) {

	Measurement meas = {999, 0, 0};

	resetRules();
	dir = firing_up;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules();
	dir = none;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules();
	dir = burning_down;
	reason(meas);
	assertFalse(rules[2].fired);

	meas.tempI = 399;

	resetRules();
	dir = firing_up;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules();
	dir = none;
	reason(meas);
	assertFalse(rules[2].fired);

	resetRules();
	dir = burning_down;
	reason(meas);
	assertTrue(rules[2].fired);
	assertTrue(0 == airgate);

	cancelAlert(false);

	return true;
}

static bool testTooRich(void) {

	Measurement meas = {0, 0, 0};
	dir = none;

	setHeaterState(heaterStateReady);

	meas.tempI = 100;
	meas.lambda = 2000;
	resetRules();
	reason(meas);
	assertFalse(rules[3].fired);

	meas.tempI = 101;
	meas.lambda = 2000;
	resetRules();
	reason(meas);
	assertFalse(rules[3].fired);

	meas.tempI = 100;
	meas.lambda = 1199;
	resetRules();
	reason(meas);
	assertFalse(rules[3].fired);

	meas.tempI = 101;
	meas.lambda = 1199;
	resetRules();
	airgate = 100;
	reason(meas);
	assertFalse(rules[3].fired);
	assertTrue(100 == airgate);

	meas.tempI = 101;
	meas.lambda = 1199;
	resetRules();
	airgate = 50;
	reason(meas);
	assertTrue(rules[3].fired);
	assertTrue(100 == airgate);

	meas.lambda = 1300;
	age = 0;
	reason(meas);
	assertFalse(rules[3].fired);

	cancelAlert(false);

	return true;
}

static bool testTooLean(void) {

	Measurement meas = {0, 0, 0};
	dir = none;

	setHeaterState(heaterStateReady);

	meas.tempI = 500;
	meas.lambda = 1601;
	resetRules();
	reason(meas);
	assertFalse(rules[4].fired);

	meas.tempI = 501;
	meas.lambda = 1300;
	resetRules();
	reason(meas);
	assertFalse(rules[4].fired);

	meas.tempI = 501;
	meas.lambda = 1601;
	resetRules();
	airgate = 50;
	reason(meas);
	assertFalse(rules[4].fired);
	assertTrue(50 == airgate);

	meas.tempI = 501;
	meas.lambda = 1601;
	resetRules();
	airgate = 100;
	reason(meas);
	assertTrue(rules[4].fired);
	assertTrue(50 == airgate);

	meas.lambda = 1500;
	age = 0;
	reason(meas);
	assertFalse(rules[4].fired);

	cancelAlert(false);

	return true;
}

static bool testFireOut(void) {

	resetRules();
	Measurement meas = {0, 0, 0};
	dir = firing_up;

	meas.tempI = 50;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = 100;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = 125;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	meas.tempI = 99;
	age = 0;
	reason(meas);
	assertTrue(rules[5].fired);

	meas.tempI = 125;
	age = 0;
	reason(meas);
	assertFalse(rules[5].fired);

	cancelAlert(false);

	return true;
}

static bool testHeaterReady(void) {

	resetRules();
	resetTime();
	Measurement meas = {0, 0, 0};
	dir = firing_up;

	setHeaterOn(false);
	meas.current = 1300;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	setHeaterOn(true);
	meas.current = 5000;
	reason(meas);
	assertTrue(heaterStateUp == getHeaterState());

	setHeaterOn(true);
	meas.current = 1300;
	reason(meas);
	assertTrue(heaterStateReady == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterFaultNoconn(void) {

	resetRules();
	resetTime();
	Measurement meas = {0, 0, 0};
	dir = firing_up;

	setHeaterOn(true);
	meas.current = 0;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterFaultShort(void) {

	resetRules();
	resetTime();
	Measurement meas = {0, 0, 0};
	dir = firing_up;

	setHeaterOn(true);
	meas.current = 8000;
	reason(meas);
	assertTrue(heaterStateFault == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testHeaterFaultNoheat(void) {

	resetRules();
	resetTime();
	Measurement meas = {0, 0, 0};
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

static bool testHeaterTimeout(void) {

	resetRules();
	resetTime();
	Measurement meas = {0, 0, 0};
	dir = firing_up;

	setHeaterOn(true);

	// more than 3 hours below 400°C
	addTime(10800UL);

	meas.tempI = 300;
	meas.current = 1300;
	reason(meas);
	assertTrue(heaterStateOff == getHeaterState());

	cancelAlert(false);

	return true;
}

static bool testReasonDirBurnUp(void) {

	resetRules();
	Measurement meas = {0, 0, 2000};

	age = 0;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 9;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 100;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == firing_up);

	meas.tempI = 690;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == firing_up);

	meas.tempI = 690;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 690;
	meas.lambda = 1900;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = 700;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = 800;
	meas.lambda = 1999;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	cancelAlert(false);

	return true;
}

static bool testReasonDirBurnDown(void) {

	resetRules();
	Measurement meas = {999, 0, 1999};

	age = 0;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 900;
	meas.lambda = 1999;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = 800;
	meas.lambda = 1999;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = 750;
	meas.lambda = 1999;
	age = 180;
	reason(meas);
	assertTrue(dir == burning);

	meas.tempI = 699;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == burning_down);

	meas.tempI = 699;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == none);

	meas.tempI = 100;
	meas.lambda = 2000;
	age = 180;
	reason(meas);
	assertTrue(dir == burning_down);

	cancelAlert(false);

	return true;
}

// TODO add tests for "burning" and "warm start"

/* Test "class" */
static const char class[] PROGMEM = "rules";

/* Test names */
static const char testAirgate50_P[] PROGMEM = "testAirgate50";
static const char testAirgate25_P[] PROGMEM = "testAirgate25";
static const char testAirgateClose_P[] PROGMEM = "testAirgateClose";
static const char testTooRich_P[] PROGMEM = "testTooRich";
static const char testTooLean_P[] PROGMEM = "testTooLean";
static const char testFireOut_P[] PROGMEM = "testFireOut";
static const char testHeaterReady_P[] PROGMEM = "testHeaterReady";
static const char testHeaterFaultNoconn_P[] PROGMEM = "testHeaterFaultNoconn";
static const char testHeaterFaultShort_P[] PROGMEM = "testHeaterFaultShort";
static const char testHeaterFaultNoheat_P[] PROGMEM = "testHeaterFaultNoheat";
static const char testHeaterTimeout_P[] PROGMEM = "testHeaterTimeout";
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
		{class, testHeaterReady_P, testHeaterReady},
		{class, testHeaterFaultNoconn_P, testHeaterFaultNoconn},
		{class, testHeaterFaultShort_P, testHeaterFaultShort},
		{class, testHeaterFaultNoheat_P, testHeaterFaultNoheat},
		{class, testHeaterTimeout_P, testHeaterTimeout},
		{class, testReasonDirBurnUp_P, testReasonDirBurnUp},
		{class, testReasonDirBurnDown_P, testReasonDirBurnDown}
};

TestClass rulesClass = {tests, sizeof(tests) / sizeof(tests[0])};
