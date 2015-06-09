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
#include "adc.h"
#include "pins.h"

/* Module adc */

bool testSetupADC(void) {
	setupADC();

	// AVCC is set as AREF
	assertTrue(bit_is_set(ADMUX, REFS0));
	// digital inputs are disabled
	assertTrue(DIDR0 == 0b00111111);
	// ADC clock prescaler/8
	uint8_t prescalerBy8 = (1 << ADPS1) | (1 << ADPS0);
	assertTrue((ADCSRA & prescalerBy8) == prescalerBy8);
	// ADC enabled
	assertTrue(bit_is_set(ADCSRA, ADEN));

	return true;
}

bool testGetVoltage(void) {
	initInterrupts();
	setupADC();
	setupSleepMode();

	// enable pull-up resistor so the measured voltage
	// should be close to AREF
	PORTC |= (1 << PC0);

	uint16_t mV = getVoltage(PC0);

	return mV > 4900;
}

/* Test "class" */
static const char class[] PROGMEM = "adc";

/* Test names */
static const char testSetupADC_P[] PROGMEM = "testSetupADC";
static const char testGetVoltage_P[] PROGMEM = "testGetVoltage";

/* Tests */
static TestCase const tests[] = {
		{class, testSetupADC_P, testSetupADC},
		{class, testGetVoltage_P, testGetVoltage}
};

TestClass adcClass = {tests, sizeof(tests) / sizeof(tests[0])};
