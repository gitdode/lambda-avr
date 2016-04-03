/*
 * airgate-test.c
 *
 *  Created on: 28.03.2016
 *      Author: dode@luniks.net
 */

#include <stdlib.h>
#include <stdio.h>
#include "avrjunit.h"
#include "pins.h"
#include "airgate.h"
#include "integers.h"
#include "utils.h"

/* Module airgate */

static bool testSetAirgate(void) {

	setDriverFault(false);
	resetAirgate(0);
	setAirgate(100);

	assertTrue(isAirgateBusy());
	// full motor current is set
	assertTrue(bit_is_clear(PORT, PIN_CURRENT));
	// direction 0
	assertTrue(bit_is_clear(PORT, PIN_DIR));
	// min speed is set
	assertTrue(MIN_SPEED == OCR2A);
	// timer2 is running (prescaler is set)
	assertTrue(0 != TCCR2B);

	stepUntilDone();

	assertFalse(isAirgateBusy());
	// reduced motor current is set
	assertFalse(bit_is_clear(PORT, PIN_CURRENT));
	// timer2 is stopped
	assertTrue(0 == TCCR2B);
	// requested position reached
	assertTrue(100 == getAirgate());

	return true;
}

static bool testMakeSteps(void) {

	const uint16_t ramp = MIN(abs(MAX_SPEED - MIN_SPEED), (100 << STEPPING_MODE) >> 1);
	uint8_t speed = MIN_SPEED;

	setDriverFault(false);
	resetAirgate(0);
	setAirgate(100);

	// full motor current is set
	assertTrue(bit_is_clear(PORT, PIN_CURRENT));
	// step pin is low
	assertTrue(bit_is_clear(PORT, PIN_STEP));
	assertTrue(MIN_SPEED == OCR2A);

	makeSteps();

	// step pin is toggled high
	assertTrue(bit_is_set(PORT, PIN_STEP));
	// accelerated by 1
	assertTrue((SPEED_PROD / ++speed) == OCR2A);

	makeSteps();

	// step pin is toggled low
	assertTrue(bit_is_clear(PORT, PIN_STEP));
	// no acceleration
	assertTrue((SPEED_PROD / speed) == OCR2A);

	// step until end of acceleration ramp
	for (uint16_t i = 1; i < ramp; i++) {
		makeSteps();
		makeSteps();
	}

	// should have accelerated to MAX_SPEED
	speed = MAX_SPEED;
	assertTrue((SPEED_PROD / speed) == OCR2A);

	// make remaining steps
	stepUntilDone();

	// step pin is toggled low
	assertTrue(bit_is_clear(PORT, PIN_STEP));
	// decelerated to MIN_SPEED
	speed = MIN_SPEED;
	assertTrue((SPEED_PROD / speed) == OCR2A);
	// reached requested position
	assertTrue(100 == getAirgate());
	// reduced motor current is set
	assertTrue(bit_is_set(PORT, PIN_CURRENT));

	return true;
}

static bool testSetSleepMode(void) {

	setSleepMode(false);

	assertTrue(bit_is_set(PORT, PIN_SLEEP));

	setSleepMode(true);

	assertTrue(bit_is_clear(PORT, PIN_SLEEP));

	return true;
}

static bool testIsDriverFault(void) {

	setDriverFault(true);
	setSleepMode(true);

	assertFalse(isDriverFault());

	setSleepMode(false);

	assertTrue(isDriverFault());

	setDriverFault(false);

	assertFalse(isDriverFault());

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "airgate";

/* Test names */
static const char testSetAirgate_P[] PROGMEM = "testSetAirgate";
static const char testMakeSteps_P[] PROGMEM = "testMakeSteps";
static const char testSetSleepMode_P[] PROGMEM = "testSetSleepMode";
static const char testIsDriverFault_P[] PROGMEM = "testIsDriverFault";

/* Tests */
static TestCase const tests[] = {
		{class, testSetAirgate_P, testSetAirgate},
		{class, testMakeSteps_P, testMakeSteps},
		{class, testSetSleepMode_P, testSetSleepMode},
		{class, testIsDriverFault_P, testIsDriverFault}
};

TestClass airgateClass = {tests, ARRAY_LENGTH(tests)};
