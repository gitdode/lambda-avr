/*
 * sensors-test.c
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdio.h>
// #include <stdlib.h>
#include "USART.h"
#include "sensors.h"
#include "avrjunit.h"

static const tableEntry testTable[] = {
		{0, 0},
		{10, 10}
};

uint8_t testAverage(void) {
	int16_t avg = 0;
	avg = average(8, avg, 4);
	if (avg != 2) { return 0; }
	avg = average(8, avg, 4);
	if (avg != 4) { return 0; }
	avg = average(8, avg, 4);
	if (avg != 5) { return 0; }
	avg = average(8, avg, 4);
	if (avg != 6) { return 0; }
	avg = average(8, avg, 4);
	if (avg != 7) { return 0; }
	avg = average(8, avg, 4);
	if (avg != 8) { return 0; }
	avg = average(8, avg, 4);
	if (avg != 8) { return 0; }

	return 1;
}

uint8_t testToLambdaValue(void) {
	int16_t lambda = toLambda(12);

	return lambda == 1500;
}

uint8_t testToLambdaInter(void) {
	int16_t lambda = toLambda(50);

	return lambda == 1073;
}

uint8_t testToTempI(void) {
	int16_t temp = toTempI(100);

	return temp == 20;
}

uint8_t testToTempOValue(void) {
	int16_t temp = toTempO(454);

	return temp == 0;
}

uint8_t testToTempOInter(void) {
	int16_t temp = toTempO(929);

	return temp == 50;
}

uint8_t testLookupLinInterBelow(void) {
	int16_t value = lookupLinInter(-5, testTable, 2);

	return value == 0;
}

uint8_t testLookupLinInterAbove(void) {
	int16_t value = lookupLinInter(15, testTable, 2);

	return value == 10;
}

uint8_t testLookupLinInterValue(void) {
	int16_t value = lookupLinInter(10, testTable, 2);

	return value == 10;
}

uint8_t testLookupLinInterInter(void) {
	int16_t value = lookupLinInter(3, testTable, 2);

	return value == 3;
}

test tests[] = {
		{"testAverage", testAverage},
		{"testToLambdaValue", testToLambdaValue},
		{"testToLambdaInter", testToLambdaInter},
		{"testToTempI", testToTempI},
		{"testToTempOValue", testToTempOValue},
		{"testToTempOInter", testToTempOInter},
		{"testLookupLinInterValue", testLookupLinInterValue},
		{"testLookupLinInterInter", testLookupLinInterInter},
		{"testLookupLinInterBelow", testLookupLinInterBelow},
		{"testLookupLinInterAbove", testLookupLinInterAbove}
};

int main(void) {
	initUSART();

	uint16_t count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", "sensors", tests, count);

	return 0;
}
