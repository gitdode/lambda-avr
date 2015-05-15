/*
 * avrjunit.c
 *
 * Functions to run basic unit tests on an AVR micro controller and
 * send a minimal JUnit test report XML via USART.
 *
 *  Created on: 06.03.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdio.h>
#include <avr/pgmspace.h>
#include "USART.h"
#include "avrjunit.h"

void beginSuite(char* const suite) {
	printString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	char tsbuf[128];
	snprintf(tsbuf, sizeof(tsbuf),"<testsuite name=\"%s\">\n", suite);
	printString(tsbuf);
}

void endSuite() {
	printString("</testsuite>\n");

	// send EOT
	printString("\n");
	transmitByte(4);
}

void runClass(TestClass class) {
	for (size_t i = 0; i < class.size; i++) {
		char cbuf[24];
		char nbuf[64];
		char tcbuf[128];
		// TODO use strncat (macro?)
		strcpy_P(cbuf, (PGM_P)pgm_read_word(&(class.tests[i].class)));
		strcpy_P(nbuf, (PGM_P)pgm_read_word(&(class.tests[i].name)));
		snprintf(tcbuf, sizeof(tcbuf),
				"<testcase classname=\"%s\" name=\"%s\">\n", cbuf, nbuf);
		printString(tcbuf);
		FuncPtr test = (FuncPtr)pgm_read_word(&(class.tests[i].function));
		bool result = test();
		if (! result) {
			// failure
			printString("<failure type=\"failure\">failed</failure>\n");
		}
		printString("</testcase>\n");
	}
}
