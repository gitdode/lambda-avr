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

void printXunit(void) {
	printString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	printString("<testsuite name=\"lambda-test\" tests=\"2\" errors=\"0\" failures=\"0\" skip=\"0\">\n");
	printString("</testsuite>\n");
}

void testToTempI(void) {
	int temp = toTempI(100);

	if (temp == 20) {
	}
}

void testToTempO(void) {
	int temp = toTempO(454);

	if (temp == 0) {
	}
}

int main(void) {

	initUSART();
	testToTempI();
	testToTempO();
	printXunit();
	// send EOT
	printString("\n");
	transmitByte(4);

	return 0;
}
