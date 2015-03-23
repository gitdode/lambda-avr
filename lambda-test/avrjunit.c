/*
 * avrjunit.c
 *
 * Functions to run basic unit tests on an AVR micro controller and
 * send a minimal JUnit test report XML via USART.
 *
 *  Created on: 06.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 */

#include <stdio.h>
#include "USART.h"
#include "avrjunit.h"

void runTests(char* suite, test tests[], uint16_t count) {
	printString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	char tsbuf[128];
	snprintf(tsbuf, sizeof(tsbuf),
			"<testsuite name=\"%s\" tests=\"%d\">\n",
			suite, count);
	printString(tsbuf);

	for (uint16_t i = 0; i < count; i++) {
		int result = (*tests[i].test)();
		char tcbuf[128];
		snprintf(tcbuf, sizeof(tcbuf),
				"<testcase classname=\"%s\" name=\"%s\">\n",
				tests[i].class, tests[i].name);
		printString(tcbuf);
		if (! result) {
			// failure
			printString("<failure type=\"failure\">failed</failure>\n");
		}
		printString("</testcase>\n");
	}

	printString("</testsuite>\n");

	// send EOT
	printString("\n");
	transmitByte(4);
}
