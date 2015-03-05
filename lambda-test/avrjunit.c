/*
 * avrjunit.c
 *
 *  Created on: 06.03.2015
 *      Author: dode@luniks.net
 *
 */
#include <stdio.h>
#include "USART.h"
#include "avrjunit.h"

void runTests(char* suite, char* class, test tests[], int count) {
	printString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	char tsbuf[64];
	snprintf(tsbuf, sizeof(tsbuf), "<testsuite name=\"%s\" tests=\"%d\">\n", suite, count);
	printString(tsbuf);

	for (int i = 0; i < count; i++) {
		int result = (*tests[i].test)();
		char tcbuf[64];
		snprintf(tcbuf, sizeof(tcbuf), "<testcase classname=\"%s\" name=\"%s\">\n", class, tests[i].name);
		printString(tcbuf);
		if (! result) {
			// failure
			printString("<failure type=\"failure\">Test failed</failure>\n");
		}
		printString("</testcase>\n");
	}

	printString("</testsuite>\n");

	// send EOT
	printString("\n");
	transmitByte(4);
}
