/*
 * avrjunit.c
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
 * DISCLAIMER: I'm experienced with Java, but new to C.
 *
 * ATTRIBUTION: This project includes the module USART and the Makefile from
 * the code accompanying the book Make: AVR Programming by Elliot Williams,
 * a great book and a pleasant read, that helped me tremendously to get
 * started with AVR programming.
 */
#include <stdio.h>
#include "USART.h"
#include "avrjunit.h"

void runTests(char* suite, test tests[], uint16_t count) {
	printString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	char tsbuf[128];
	snprintf(tsbuf, sizeof(tsbuf), "<testsuite name=\"%s\" tests=\"%d\">\n",
			suite, count);
	printString(tsbuf);

	for (uint16_t i = 0; i < count; i++) {
		int result = (*tests[i].test)();
		char tcbuf[128];
		snprintf(tcbuf, sizeof(tcbuf), "<testcase classname=\"%s\" name=\"%s\">\n",
				tests[i].class, tests[i].name);
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
