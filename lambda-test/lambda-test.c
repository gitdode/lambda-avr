/*
 * sensors-test.c
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "USART.h"
#include "sensors.h"

int testToTempI(void) {
	int temp = toTempI(100);

	return temp == 20;
}

int testToTempO(void) {
	int temp = toTempO(454);

	return temp == 999;
}

typedef struct {
	char* name;
	int (*test)(void);
} test;

test tests[] = {
	{"testToTempI", testToTempI},
	{"testToTempO", testToTempO}
};

void runTests(void) {
	printString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	printString("<testsuite name=\"lambda-test\" tests=\"2\" failures=\"0\">\n");

	int testCount = sizeof(tests) / sizeof(tests[0]);
	for (int i = 0; i < testCount; i++) {
		int result = (*tests[i].test)();
		char buf[128];
		snprintf(buf, sizeof(buf), "<testcase classname=\"lambda-test\" name=\"%s\">\n", tests[i].name);
		printString(buf);
		if (result) {
			// pass
			printString("</testcase>\n");
		} else {
			printString("<failure type=\"failure\">Test failed</failure>\n");
			printString("</testcase>\n");
		}
	}

	printString("</testsuite>\n");
}

int main(void) {

	initUSART();

	runTests();

	// send EOT
	printString("\n");
	transmitByte(4);

	return 0;
}
