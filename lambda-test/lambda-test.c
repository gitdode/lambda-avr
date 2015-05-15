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

	extern TestClass adcClass;
	extern TestClass commandClass;

	beginSuite("lambda");
	runClass(adcClass);
	runClass(commandClass);
	endSuite();

	return 0;
}
