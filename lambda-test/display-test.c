/*
 * lambda-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "avrjunit.h"
#include "display.h"
#include "sensors.h"
#include "alert.h"

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

	alert(1, 1, 31, "", "");
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
	alert(1, 1, 31, "", "");
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

/* Test "class" */
static const char class[] PROGMEM = "display";

/* Test names */
static const char testCycle_P[] PROGMEM = "testCycle";
static const char testCycleCancelAlert_P[] PROGMEM = "testCycleCancelAlert";
static const char testUpdateMeas_P[] PROGMEM = "testUpdateMeas";
static const char testResetMeas_P[] PROGMEM = "testResetMeas";
static const char testUpdateDisplayIfPending_P[] PROGMEM = "testUpdateDisplayIfPending";
static const char testUpdateDisplayIfPendingAlertActive_P[] PROGMEM = "testUpdateDisplayIfPendingAlertActive";
static const char testDisplayText_P[] PROGMEM = "testDisplayText";

/* Tests */
static TestCase const tests[] = {
		{class, testCycle_P, testCycle},
		{class, testCycleCancelAlert_P, testCycleCancelAlert},
		{class, testUpdateMeas_P, testUpdateMeas},
		{class, testResetMeas_P, testResetMeas},
		{class, testUpdateDisplayIfPending_P, testUpdateDisplayIfPending},
		{class, testUpdateDisplayIfPendingAlertActive_P, testUpdateDisplayIfPendingAlertActive},
		{class, testDisplayText_P, testDisplayText}
};

TestClass displayClass = {tests, sizeof(tests) / sizeof(tests[0])};
