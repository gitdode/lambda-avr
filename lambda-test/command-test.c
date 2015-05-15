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
#include "command.h"

/* Module command */

bool testIsSimulation(void) {
	assertFalse(isSimulation());
	runCommand("se");
	assertTrue(isSimulation());
	runCommand("sd");
	assertFalse(isSimulation());

	return true;
}

bool testIsLogging(void) {
	assertFalse(isLogging());
	runCommand("le");
	assertTrue(isLogging());
	runCommand("ld");
	assertFalse(isLogging());

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "command";

/* Test names */
static const char testIsSimulation_P[] PROGMEM = "testIsSimulation";
static const char testIsLogging_P[] PROGMEM = "testIsLogging";

/* Tests */
static TestCase const tests[] = {
		{class, testIsSimulation_P, testIsSimulation},
		{class, testIsLogging_P, testIsLogging},
};

TestClass commandClass = {tests, sizeof(tests) / sizeof(tests[0])};
