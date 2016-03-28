/*
 * release-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "avrjunit.h"
#include "interrupts.h"
#include "messages.h"
#include "pins.h"
#include "sensors.h"

/* Validate release */

/**
 * TODO Makefile, improve this, how?
 */
static bool testRelease(void) {
	// language set to en
	assertTrue(0 == LANG);

	// pins according to diagram
	assertTrue(PD7 == LCD_RS);
	assertTrue(PD6 == LCD_EN);
	assertTrue(PD5 == LCD_DB4);
	assertTrue(PD4 == LCD_DB5);
	assertTrue(PD3 == LCD_DB6);
	assertTrue(PD2 == LCD_DB7);

	// nominal value
	assertTrue(100 == SHUNT_MILLIOHMS);

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "release";

/* Test names */
static const char testRelease_P[] PROGMEM = "testRelease";

/* Tests */
static TestCase const tests[] = {
		{class, testRelease_P, testRelease}
};

TestClass releaseClass = {tests, ARRAY_LENGTH(tests)};
