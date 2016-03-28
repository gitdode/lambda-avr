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
 * ATTRIBUTION: This project includes the the Makefile from the code
 * accompanying the book Make: AVR Programming by Elliot Williams, a great book
 * and a pleasant read that helped me tremendously to get started with AVR
 * programming.
 * ATTRIBUTION: This project includes the module lcdroutines from
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
 */

#include <avr/io.h>
#include "avrjunit.h"
#include "usart.h"

int main(void) {
	initUSART();

	extern TestClass releaseClass;
	extern TestClass adcClass;
	extern TestClass airgateClass;
	extern TestClass alertClass;
	extern TestClass commandClass;
	extern TestClass displayClass;
	extern TestClass integersClass;
	extern TestClass interruptsClass;
	extern TestClass rulesClass;
	extern TestClass schedulerClass;
	extern TestClass sensorsClass;
	extern TestClass stringsClass;
	extern TestClass usartClass;

	beginSuite("lambda");
	runClass(releaseClass);
	// run rules tests before interrupts are initialized to avoid interference
	runClass(rulesClass);
	runClass(adcClass);
	runClass(airgateClass);
	runClass(alertClass);
	runClass(commandClass);
	runClass(displayClass);
	runClass(integersClass);
	runClass(interruptsClass);
	runClass(schedulerClass);
	runClass(sensorsClass);
	runClass(stringsClass);
	runClass(usartClass);
	endSuite();

	while (1) {
	}

	return 0;
}
