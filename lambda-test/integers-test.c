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

bool testDivRoundNearest(void) {
	assertTrue(divRoundNearest(4, 2) == 2);
	assertTrue(divRoundNearest(5, 2) == 3);
	assertTrue(divRoundNearest(10, 3) == 3);

	return true;
}

bool testDivRoundNearestNumNeg(void) {
	assertTrue(divRoundNearest(-4, 2) == -2);
	assertTrue(divRoundNearest(-5, 2) == -3);
	assertTrue(divRoundNearest(-10, 3) == -3);

	return true;
}

bool testDivRoundNearestDenNeg(void) {
	assertTrue(divRoundNearest(4, -2) == -2);
	assertTrue(divRoundNearest(5, -2) == -3);
	assertTrue(divRoundNearest(10, -3) == -3);

	return true;
}

bool testDivRoundNearestBothNeg(void) {
	assertTrue(divRoundNearest(-4, -2) == 2);
	assertTrue(divRoundNearest(-5, -2) == 3);
	assertTrue(divRoundNearest(-10, -3) == 3);

	return true;
}

bool testDivRoundUp(void) {
	assertTrue(divRoundUp(4, 2) == 2);
	assertTrue(divRoundUp(5, 2) == 3);
	assertTrue(divRoundUp(10, 3) == 4);

	return true;
}

bool testDivRoundUpNumNeg(void) {
	assertTrue(divRoundUp(-4, 2) == -2);
	assertTrue(divRoundUp(-5, 2) == -3);
	assertTrue(divRoundUp(-10, 3) == -4);

	return true;
}

bool testDivRoundUpDenNeg(void) {
	assertTrue(divRoundUp(4, -2) == -2);
	assertTrue(divRoundUp(5, -2) == -3);
	assertTrue(divRoundUp(10, -3) == -4);

	return true;
}

bool testDivRoundUpBothNeg(void) {
	assertTrue(divRoundUp(-4, -2) == 2);
	assertTrue(divRoundUp(-5, -2) == 3);
	assertTrue(divRoundUp(-10, -3) == 4);

	return true;
}

/* Test "class" */
const char integers_P[] PROGMEM = "integers";

/* Test names */
const char testDivRoundNearest_P[] PROGMEM = "testDivRoundNearest";
const char testDivRoundNearestNumNeg_P[] PROGMEM = "testDivRoundNearestNumNeg";
const char testDivRoundNearestDenNeg_P[] PROGMEM = "testDivRoundNearestDenNeg";
const char testDivRoundNearestBothNeg_P[] PROGMEM = "testDivRoundNearestBothNeg";
const char testDivRoundUp_P[] PROGMEM = "testDivRoundUp";
const char testDivRoundUpNumNeg_P[] PROGMEM = "testDivRoundUpNumNeg";
const char testDivRoundUpDenNeg_P[] PROGMEM = "testDivRoundUpDenNeg";
const char testDivRoundUpBothNeg_P[] PROGMEM = "testDivRoundUpBothNeg";

/* Tests */
TestCase const integersTests[] = {
		{integers_P, testDivRoundNearest_P, testDivRoundNearest},
		{integers_P, testDivRoundNearestNumNeg_P, testDivRoundNearestNumNeg},
		{integers_P, testDivRoundNearestDenNeg_P, testDivRoundNearestDenNeg},
		{integers_P, testDivRoundNearestBothNeg_P, testDivRoundNearestBothNeg},
		{integers_P, testDivRoundUp_P, testDivRoundUp},
		{integers_P, testDivRoundUpNumNeg_P, testDivRoundUpNumNeg},
		{integers_P, testDivRoundUpDenNeg_P, testDivRoundUpDenNeg},
		{integers_P, testDivRoundUpBothNeg_P, testDivRoundUpBothNeg},
};
