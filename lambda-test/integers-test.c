/*
 * integers-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include "avrjunit.h"
#include "integers.h"

/* Module integers */

static bool testDivRoundNearest(void) {
	assertTrue(divRoundNearest(4, 2) == 2);
	assertTrue(divRoundNearest(5, 2) == 3);
	assertTrue(divRoundNearest(10, 3) == 3);

	return true;
}

static bool testDivRoundNearestNumNeg(void) {
	assertTrue(divRoundNearest(-4, 2) == -2);
	assertTrue(divRoundNearest(-5, 2) == -3);
	assertTrue(divRoundNearest(-10, 3) == -3);

	return true;
}

static bool testDivRoundNearestDenNeg(void) {
	assertTrue(divRoundNearest(4, -2) == -2);
	assertTrue(divRoundNearest(5, -2) == -3);
	assertTrue(divRoundNearest(10, -3) == -3);

	return true;
}

static bool testDivRoundNearestBothNeg(void) {
	assertTrue(divRoundNearest(-4, -2) == 2);
	assertTrue(divRoundNearest(-5, -2) == 3);
	assertTrue(divRoundNearest(-10, -3) == 3);

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "integers";

/* Test names */
static const char testDivRoundNearest_P[] PROGMEM = "testDivRoundNearest";
static const char testDivRoundNearestNumNeg_P[] PROGMEM = "testDivRoundNearestNumNeg";
static const char testDivRoundNearestDenNeg_P[] PROGMEM = "testDivRoundNearestDenNeg";
static const char testDivRoundNearestBothNeg_P[] PROGMEM = "testDivRoundNearestBothNeg";

/* Tests */
static TestCase const tests[] = {
		{class, testDivRoundNearest_P, testDivRoundNearest},
		{class, testDivRoundNearestNumNeg_P, testDivRoundNearestNumNeg},
		{class, testDivRoundNearestDenNeg_P, testDivRoundNearestDenNeg},
		{class, testDivRoundNearestBothNeg_P, testDivRoundNearestBothNeg}
};

TestClass integersClass = {tests, ARRAY_LENGTH(tests)};
