/*
 * avrjunit.h
 *
 *  Created on: 06.03.2015
 *      Author: dode@luniks.net
 */
typedef struct {
	char* name;
	int (*test)(void);
} test;

void runTests(char* suite, char* class, test tests[], int count);
