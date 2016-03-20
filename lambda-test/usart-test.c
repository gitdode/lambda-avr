/*
 * usart-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "usart.h"
#include <util/setbaud.h>
#include "avrjunit.h"
#include "integers.h"

/* Module usart */

static bool testInitUSART(void) {
	assertTrue(BAUD);

	initUSART();

	assertTrue(UBRR0H == UBRRH_VALUE);
	assertTrue(UBRR0L == UBRRL_VALUE);

#if USE_2X
	assertTrue(bit_is_set(UCSR0A, U2X0));
#else
	assertTrue(bit_is_clear(UCSR0A, U2X0));
#endif

	// transmission enabled
	assertTrue(bit_is_set(UCSR0B, TXEN0));
	// receive enabled
	assertTrue(bit_is_set(UCSR0B, RXEN0));

	// 8 data bits
	assertTrue(bit_is_set(UCSR0C, UCSZ01));
	assertTrue(bit_is_set(UCSR0C, UCSZ00));

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "usart";

/* Test names */
static const char testInitUSART_P[] PROGMEM = "testInitUSART";

/* Tests */
static TestCase const tests[] = {
		{class, testInitUSART_P, testInitUSART}
};

TestClass usartClass = {tests, ARRAY_LENGTH(tests)};
