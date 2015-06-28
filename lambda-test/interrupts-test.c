/*
 * interrupts-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include <string.h>
#include <util/delay.h>
#include "avrjunit.h"
#include "interrupts.h"

/* Module interrupts */

static bool testSetupPorts(void) {
	setupPorts();

	// test that the pull-up resistor for the mouton is enabled
	assertTrue(bit_is_set(PORTB, PB0));

	// test that the beep output pin is enabled
	assertTrue(bit_is_set(DDRB, PB1));

	// test that the oxygen sensor heater control output pin is enabled
	assertTrue(bit_is_set(DDRB, PB1));

	return true;
}

static bool testSetupSleepMode(void) {
	setupSleepMode();

	// set_sleep_mode(SLEEP_MODE_IDLE);
	assertFalse(bit_is_set(SMCR, SM2));
	assertFalse(bit_is_set(SMCR, SM1));
	assertFalse(bit_is_set(SMCR, SM0));

	return true;
}

static bool testInitInterrupts(void) {
	initInterrupts();

	// ADC interrupt enabled
	assertTrue(bit_is_set(ADCSRA, ADIE));

	// timer0 compare match A interrupt enabled
	assertTrue(bit_is_set(TIMSK0, OCIE0A));

	// USART RX complete interrupt 0 enabled
	assertTrue(bit_is_set(UCSR0B, RXCIE0));

	// sei(); // enable global interrupts
	assertTrue(bit_is_set(SREG, SREG_I));

	return true;
}

static bool testInitTimers(void) {
	initTimers();

	// timer0 clear timer on compare match mode, TOP OCR0A
	assertTrue(bit_is_set(TCCR0A, WGM01));
	// timer0 clock prescaler/256
	uint8_t timer0Prescaler = (1 << CS02);
	assertTrue((TCCR0B & timer0Prescaler) == timer0Prescaler);
	// timer0 compare match
	assertTrue(OCR0A == 122);

	// timer1 clear timer on compare match mode, TOP OCR1A
	assertTrue(bit_is_set(TCCR1B, WGM12));
	// timer1 clock prescaler/8
	uint8_t timer1Prescaler = (1 << CS11);
	assertTrue((TCCR1B & timer1Prescaler) == timer1Prescaler);
	// timer1 compare match
	assertTrue(OCR1A == 31);

	return true;
}

static bool testTime(void) {
	resetTime();
	assertTrue(getTime() == 0);

	_delay_ms(1000);

	assertTrue(getTime() == 1);

	// add 1:11:10
	addTime(4270UL);

	// HHHHH:MM:SS
	char str[12];
	formatTime(str, sizeof(str));
	assertTrue(strcmp("1:11:11", str));

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "interrupts";

/* Test names */
static const char testSetupPorts_P[] PROGMEM = "testSetupPorts";
static const char testSetupSleepMode_P[] PROGMEM = "testSetupSleepMode";
static const char testInitInterrupts_P[] PROGMEM = "testInitInterrupts";
static const char testInitTimers_P[] PROGMEM = "testInitTimers";
static const char testTime_P[] PROGMEM = "testTime";

/* Tests */
static TestCase const tests[] = {
		{class, testSetupPorts_P, testSetupPorts},
		{class, testSetupSleepMode_P, testSetupSleepMode},
		{class, testInitInterrupts_P, testInitInterrupts},
		{class, testInitTimers_P, testInitTimers},
		{class, testTime_P, testTime}
};

TestClass interruptsClass = {tests, sizeof(tests) / sizeof(tests[0])};
