/*
 * sensors-test.c
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 */
#include <stdio.h>
#include "USART.h"
#include "sensors.h"
#include "avrjunit.h"

int testToTempI(void) {
	int temp = toTempI(100);

	return temp == 20;
}

int testToTempO0C(void) {
	int temp = toTempO(454);

	return temp == 0;
}

int testToTempO50C(void) {
	int temp = toTempO(928);

	return temp == 50;
}

int testToTempO100C(void) {
	int temp = toTempO(1403);

	return temp == 100;
}

test tests[] = {
	{"testToTempI", testToTempI},
	{"testToTempO0C", testToTempO0C},
	{"testToTempO50C", testToTempO50C},
	{"testToTempO100C", testToTempO100C}
};

int main(void) {
	initUSART();

	int count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", "sensors", tests, count);

	return 0;
}
