/*
 * sensors-test.c
 *
 *  Created on: 04.03.2015
 *      Author: dode@luniks.net
 *
 */
#include <math.h>
#include <stdio.h>
// #include <stdlib.h>
#include "USART.h"
#include "sensors.h"
#include "avrjunit.h"

static const tableEntry testTable[] = {
		{0, 0},
		{10, 10}
};

int testToLambdaValue(void) {
	float lambda = toLambda(12);

	return lambda == 1.5;
}

int testToLambdaInter(void) {
	float lambda = toLambda(50);

	return round(lambda * 1000) == 1073;
}

int testToTempI(void) {
	int temp = toTempI(100);

	return temp == 20;
}

int testToTempOValue(void) {
	int temp = toTempO(454);

	return temp == 0;
}

int testToTempOInter(void) {
	int temp = toTempO(928);

	return temp == 50;
}

int testLookupLinInterBelow(void) {
	float value = lookupLinInter(-5, testTable, 2);

	return value == 0;
}

int testLookupLinInterAbove(void) {
	float value = lookupLinInter(15, testTable, 2);

	return value == 10;
}

int testLookupLinInterValue(void) {
	float value = lookupLinInter(10, testTable, 2);

	return value == 10;
}

int testLookupLinInterInter(void) {
	float value = lookupLinInter(3, testTable, 2);

	return value == 3;
}

test tests[] = {
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

	int count = sizeof(tests) / sizeof(tests[0]);
	runTests("lambda", "sensors", tests, count);

	return 0;
}
