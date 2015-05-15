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
const char command_P[] PROGMEM = "command";

/* Test names */
const char testIsSimulation_P[] PROGMEM = "testIsSimulation";
const char testIsLogging_P[] PROGMEM = "testIsLogging";

/* Tests */
TestCase const commandTests[] = {
		{command_P, testIsSimulation_P, testIsSimulation},
		{command_P, testIsLogging_P, testIsLogging},
};
