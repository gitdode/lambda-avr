/*
 * avrjunit.h
 *
 * Very basic unit test framework for AVR micro controller code.
 *
 *  Created on: 06.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 */

#include <stdbool.h>
#include <avr/pgmspace.h>

#ifndef AVRJUNIT_H_
#define AVRJUNIT_H_

/**
 * Returns false if the given expression is not true.
 */
#define assertTrue(exp) if (! (exp)) return false

/**
 * Returns false if the given expression is not false.
 */
#define assertFalse(exp) if (exp) return false

/**
 * Function pointer for test functions taking no parameters and returning
 * true on success and false on failure.
 */
typedef bool (*fptr)(void);

/**
 * A test case with its class, name and test function pointer,
 * which should return true on success and false on failure.
 */
typedef struct PROGMEM {
	const char* class;
	const char* name;
	fptr function;
} test;

/**
 * Runs the test cases in the given array and prints the results
 * via USART in JUnit XML format, using the given test suite name.
 * The size of the array needs to be passed along.
 * The printed JUnit XML can be read and written to a file
 * on the receiving side with a command like:
 * (stty sane; cat > tests.xml) < /dev/ttyUSB0
 */
void runTests(char* const suite, test const tests[], uint16_t const count);

#endif /* AVRJUNIT_H_ */
