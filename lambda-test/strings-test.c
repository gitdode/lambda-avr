/*
 * lambda-test.c
 *
 * Unit tests for the lambda project.
 *
 *  Created on: 15.05.2015
 *      Author: dode@luniks.net
 *
 */

#include <string.h>
#include "avrjunit.h"
#include "strings.h"

/* Module strings */

bool testSplit(void) {
	char string[] = "f1 f2  f3 ";
	char* fields[4];
	split(string, " ", fields, 4);

	assertTrue(strcmp("f1", fields[0]) == 0);
	assertTrue(strcmp("f2", fields[1]) == 0);
	assertTrue(strcmp("f3", fields[2]) == 0);
	assertTrue(strcmp("", fields[3]) == 0);

	return true;
}

bool testSplitSizeTooSmall(void) {
	char string[] = "f1 f2";
	char* fields[1];
	split(string, " ", fields, 1);

	assertTrue(strcmp("f1", fields[0]) == 0);

	return true;
}

/* Test "class" */
const char strings_P[] PROGMEM = "strings";

/* Test names */
const char testSplit_P[] PROGMEM = "testSplit";
const char testSplitSizeTooSmall_P[] PROGMEM = "testSplitSizeTooSmall";

/* Tests */
TestCase const stringsTests[] = {
		{strings_P, testSplit_P, testSplit},
		{strings_P, testSplitSizeTooSmall_P, testSplitSizeTooSmall}
};
