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

#ifndef AVRJUNIT_H_
#define AVRJUNIT_H_

/**
 * A test case with its class, name and pointer to the test function,
 * which should return true on success and false on failure.
 */
typedef struct {
	char* class;
	char* name;
	bool (*test)(void);
} test;

/**
 * Runs the test cases in the given array and prints the results
 * via USART in JUnit XML format, using the given test suite name.
 * The size of the array needs to be passed along.
 * The printed JUnit XML can be read and written to a file
 * on the receiving side with a command like:
 * (stty sane; cat > tests.xml) < /dev/ttyUSB0
 */
void runTests(char* suite, test tests[], uint16_t count);

#endif /* AVRJUNIT_H_ */
