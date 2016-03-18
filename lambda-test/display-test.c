/*
 * display-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include <string.h>
#include "avrjunit.h"
#include "display.h"
#include "sensors.h"
#include "alert.h"

/* Module display */

extern uint8_t position;
extern bool updatePending;

extern uint8_t beepCount;
extern uint16_t beepLength;

extern Measurement measLatest;
extern Measurement measMax; // = {0, 0, 2000};

static bool testCycle(void) {
	cancelAlert();
	position = displayPosCurrent;
	updatePending = false;

	assertTrue(position == displayPosCurrent);
	assertFalse(updatePending);

	cycleDisplay();
	assertTrue(position == displayPosMax);
	assertTrue(updatePending);
	assertTrue(beepCount == 1);
	assertTrue(beepLength == 1);

	cycleDisplay();
	assertTrue(position == displayPosAirgate);
	cycleDisplay();
	assertTrue(position == displayPosHeater);
	cycleDisplay();
	assertTrue(position == displayPosLastText);
	cycleDisplay();
	assertTrue(position == displayPosTime);
	cycleDisplay(); // roll over to 0
	assertTrue(position == displayPosCurrent);

	return true;
}

static bool testCycleCancelAlert(void) {
	position = displayPosCurrent;;
	updatePending = false;

	alert(1, 1, 31, "", "", false);
	assertTrue(isAlertActive());

	cycleDisplay();
	assertFalse(isAlertActive());
	assertTrue(position == displayPosCurrent);
	assertTrue(updatePending);

	return true;
}

static bool testUpdateMeas(void) {
	// initial measurements
	assertTrue(measLatest.tempI == 0);
	assertTrue(measLatest.tempO == 0);
	assertTrue(measLatest.lambda == 2000);
	assertTrue(measLatest.current == 0);
	// initial max measurements
	assertTrue(measMax.tempI == 0);
	assertTrue(measMax.tempO == 0);
	assertTrue(measMax.lambda == 2000);

	Measurement meas1 = {1, 2, 3, 4};
	updateMeas(meas1);
	// updated measurements
	assertTrue(measLatest.tempI == 1);
	assertTrue(measLatest.tempO == 2);
	assertTrue(measLatest.lambda == 3);
	assertTrue(measLatest.current == 4);
	// updated max measurements
	assertTrue(measMax.tempI == 1);
	assertTrue(measMax.tempO == 2);
	assertTrue(measMax.lambda == 3);

	Measurement meas2 = {10, 20, 30, 40};
	updateMeas(meas2);
	// updated max measurements
	assertTrue(measMax.tempI == 10);
	assertTrue(measMax.tempO == 20);
	assertTrue(measMax.lambda == 3);

	return true;
}

static bool testResetMeas(void) {
	updatePending = false;

	resetDisplay();
	// reset max measurements
	assertTrue(measMax.tempI == 0);
	assertTrue(measMax.tempO == 0);
	assertTrue(measMax.lambda == 2000);
	assertTrue(updatePending);

	return true;
}

static bool testUpdateDisplayIfPending(void) {
	updatePending = true;

	updateDisplayIfPending();
	assertFalse(updatePending);

	return true;
}

static bool testUpdateDisplayIfPendingAlertActive(void) {
	updatePending = true;
	alert(1, 1, 31, "", "", false);
	assertTrue(isAlertActive());

	// update should be skipped if alert is active
	updateDisplayIfPending();
	assertTrue(updatePending);

	return true;
}

static bool testDisplayText(void) {
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
