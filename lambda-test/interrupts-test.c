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
#include "interrupts.h"

/* Module interrupts */

bool testSetupPorts(void) {
	setupPorts();

	// test that the pull-up resistor for the mouton is enabled
	assertTrue(bit_is_set(PORTB, PB0));

	// test that the beep output pin is enabled
	assertTrue(bit_is_set(DDRB, PB1));

	return true;
}

bool testSetupSleepMode(void) {
	setupSleepMode();

	// set_sleep_mode(SLEEP_MODE_IDLE);
	assertFalse(bit_is_set(SMCR, SM2));
	assertFalse(bit_is_set(SMCR, SM1));
	assertFalse(bit_is_set(SMCR, SM0));

	return true;
}

bool testInitInterrupts(void) {
	initInterrupts();

	// ADC interrupt enabled
	assertTrue(bit_is_set(ADCSRA, ADIE));
	// PC interrupts enabled
	// assertTrue(bit_is_set(PCICR, PCIE0));
	// assertTrue(bit_is_set(PCMSK0, PB0));
	// enable timer 0 overflow interrupt
	assertTrue(bit_is_set(TIMSK0, TOIE0));
	// USART RX complete interrupt 0 enabled
	assertTrue(bit_is_set(UCSR0B, RXCIE0));

	// sei(); // enable global interrupts
	assertTrue(bit_is_set(SREG, SREG_I));

	return true;
}

bool testInitTimers(void) {
	initTimers();

	// timer0 clock prescaler /64 = 15.625 kHz overflowing every 16.2 ms
	uint8_t prescalerBy64 = (1 << CS00) | (1 << CS01);
	assertTrue((TCCR0B & prescalerBy64) == prescalerBy64);

	// timer1 Clear Timer on Compare Match mode, TOP OCR1A
	assertTrue(bit_is_set(TCCR1B, WGM12));
	// timer1 clock prescaler/8
	assertTrue(bit_is_set(TCCR1B, CS11));
	// toggles PB1 at 7.8 kHz generating a 3.9 kHz beep
	// assertTrue(OCR1A == 15);
	// 2 kHz is less noisy on the small piezo beeper
	assertTrue(OCR1A == 31);

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "interrupts";

/* Test names */
static const char testSetupPorts_P[] PROGMEM = "testSetupPorts";
static const char testSetupSleepMode_P[] PROGMEM = "testSetupSleepMode";
static const char testInitInterrupts_P[] PROGMEM = "testInitInterrupts";
static const char testInitTimers_P[] PROGMEM = "testInitTimers";

/* Tests */
static TestCase const tests[] = {
		{class, testSetupPorts_P, testSetupPorts},
		{class, testSetupSleepMode_P, testSetupSleepMode},
		{class, testInitInterrupts_P, testInitInterrupts},
		{class, testInitTimers_P, testInitTimers},
};

TestClass interruptsClass = {tests, sizeof(tests) / sizeof(tests[0])};
