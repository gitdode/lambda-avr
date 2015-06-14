/*
 * command-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "avrjunit.h"
#include "alert.h"
#include "command.h"
#include "display.h"
#include "interrupts.h"
#include "sensors.h"

/* Module command */

bool testIsSimulation(void) {
	extern bool updatePending;

	setupPorts();
	setHeatingOn(false);

	assertFalse(isSimulation());
	runCommand("se");
	assertTrue(isSimulation());
	assertTrue(getTime() == 0);
	assertTrue(isHeatingOn());
	runCommand("sd");
	assertFalse(isSimulation());
	assertTrue(getTime() == 0);
	// assertFalse(isHeatingOn());

	assertTrue(updatePending);

	return true;
}

bool testIsLogging(void) {
	extern bool updatePending;

	assertFalse(isLogging());
	runCommand("le");
	assertTrue(isLogging());
	runCommand("ld");
	assertFalse(isLogging());

	assertTrue(updatePending);

	return true;
}

bool testHeating(void) {
	extern bool updatePending;

	setupPorts();
	setHeatingOn(false);

	assertFalse(isHeatingOn());
	runCommand("he");
	assertTrue(isHeatingOn());
	runCommand("hd");
	assertFalse(isHeatingOn());

	assertTrue(updatePending);

	return true;
}

bool testCycleDisplay(void) {
	extern uint8_t position;
	extern bool updatePending;

	cancelAlert(true);
	assertTrue(position == 0);

	cycleDisplay();
	assertTrue(position == 1);

	assertTrue(updatePending);

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "command";

/* Test names */
static const char testIsSimulation_P[] PROGMEM = "testIsSimulation";
static const char testIsLogging_P[] PROGMEM = "testIsLogging";
static const char testHeating_P[] PROGMEM = "testHeating";
static const char testCycleDisplay_P[] PROGMEM = "testCycleDisplay";

/* Tests */
static TestCase const tests[] = {
		{class, testIsSimulation_P, testIsSimulation},
		{class, testIsLogging_P, testIsLogging},
		{class, testHeating_P, testHeating},
		{class, testCycleDisplay_P, testCycleDisplay}
};

TestClass commandClass = {tests, sizeof(tests) / sizeof(tests[0])};
