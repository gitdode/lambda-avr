/*
 * adc-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "avrjunit.h"
#include "interrupts.h"
#include "alert.h"

/* Module alert */

bool testOscillateBeep(void) {
	extern uint8_t beepCount;
	extern uint8_t oscCount;

	beepCount = 0;
	oscCount = 0;

	// alert with two beep2 with a length of 2
	alert(2, 2, "a", "b");
	oscillateBeep();
	oscillateBeep();

	// beep on, alertActive true
	assertTrue(beepCount == 2);
	assertTrue(oscCount == 2);
	assertTrue(bit_is_set(TCCR1A, COM1A0));
	assertTrue(isAlertActive());

	oscillateBeep();
	oscillateBeep();

	// beep off, alertActive still true
	assertTrue(beepCount == 1);
	assertTrue(oscCount == 0);
	assertTrue(bit_is_clear(TCCR1A, COM1A0));
	assertTrue(isAlertActive());

	oscillateBeep();
	oscillateBeep();

	// beep on, alertActive still true
	assertTrue(beepCount == 1);
	assertTrue(oscCount == 2);
	assertTrue(bit_is_set(TCCR1A, COM1A0));
	assertTrue(isAlertActive());

	oscillateBeep();
	oscillateBeep();

	// beep off, alertActive false
	assertTrue(beepCount == 0);
	assertTrue(oscCount == 0);
	assertTrue(bit_is_clear(TCCR1A, COM1A0));
	assertFalse(isAlertActive());

	return true;
}

bool testBeep(void) {
	extern uint8_t beepCount;
	extern uint16_t beepLength;
	extern uint8_t oscCount;

	beepCount = 0;
	beepLength = 0;
	oscCount = 0;

	beep(1, 2);

	assertTrue(beepCount == 1);
	assertTrue(beepLength == 2);
	assertTrue(oscCount == 0);

	return true;
}

bool testAlert(void) {
	extern uint8_t beepCount;
	extern uint16_t beepLength;
	extern uint8_t oscCount;

	beepCount = 0;
	beepLength = 0;
	oscCount = 0;

	alert(1, 2, "a", "b");

	assertTrue(beepCount == 1);
	assertTrue(beepLength == 2);
	assertTrue(oscCount == 0);
	assertTrue(isAlertActive());

	return true;
}

bool testCancelAlert(void) {
	extern uint8_t beepCount;
	extern uint16_t beepLength;
	extern uint8_t oscCount;

	beepCount = 0;
	beepLength = 0;
	oscCount = 0;

	alert(1, 2, "a", "b");

	assertTrue(beepCount == 1);
	assertTrue(beepLength == 2);
	assertTrue(oscCount == 0);
	assertTrue(isAlertActive());

	cancelAlert();

	assertTrue(beepCount == 0);
	assertTrue(oscCount == 0);
	assertTrue(bit_is_clear(TCCR1A, COM1A0));
	assertFalse(isAlertActive());

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "alert";

/* Test names */
static const char testOscillateBeep_P[] PROGMEM = "testOscillateBeep";
static const char testBeep_P[] PROGMEM = "testBeep";
static const char testAlert_P[] PROGMEM = "testAlert";
static const char testCancelAlert_P[] PROGMEM = "testCancelAlert";

/* Tests */
static TestCase const tests[] = {
		{class, testOscillateBeep_P, testOscillateBeep},
		{class, testBeep_P, testBeep},
		{class, testAlert_P, testAlert},
		{class, testCancelAlert_P, testCancelAlert}
};

TestClass alertClass = {tests, sizeof(tests) / sizeof(tests[0])};