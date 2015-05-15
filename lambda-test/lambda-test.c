/*
 * lambda-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 *
 * ATTRIBUTION: This project includes the module USART and the Makefile from
 * the code accompanying the book Make: AVR Programming by Elliot Williams,
 * a great book and a pleasant read, that helped me tremendously to get
 * started with AVR programming.
 * ATTRIBUTION: This project includes the module lcdroutines from
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
 */

#include <avr/io.h>
#include "USART.h"
#include "avrjunit.h"

int main(void) {
	initUSART();

	extern const TestCase adcTests[];
	extern const TestCase commandTests[];
	extern const TestCase displayTests[];
	extern const TestCase integersTests[];
	extern const TestCase interruptsTests[];
	extern const TestCase sensorsTests[];
	extern const TestCase stringsTests[];

	runTests("lambda", adcTests, 2);
	runTests("lambda", commandTests, 2);
	runTests("lambda", displayTests, 7);
	runTests("lambda", integersTests, 8);
	runTests("lambda", interruptsTests, 4);
	runTests("lambda", sensorsTests, 16);
	runTests("lambda", stringsTests, 2);

	return 0;
}
