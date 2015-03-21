/*
 * avrjunit.h
 *
 * Very basic unit test framework for AVR microcontroller code.
 *
 *  Created on: 06.03.2015
 *      Author: dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU Lesser General Public License Version 2.1 (the "LGPL License"), in which
 * case the provisions of the LGPL License are applicable instead of those
 * above.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU (Lesser) General Public License for more details.
 *
 * DISCLAIMER: I'm experienced with Java and I like C, but I'm new here.
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
