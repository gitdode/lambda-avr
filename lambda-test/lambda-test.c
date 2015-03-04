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

void testToTempI(void) {
	int temp = toTempI(100);

	if (temp == 20) {
		printString("testToTempI(): passed\r\n");
	} else {
		printString("testToTempI(): failed!\r\n");
	}
}

void testToTempO(void) {
	int temp = toTempO(454);

	if (temp == 0) {
		printString("testToTempO(): passed\r\n");
	} else {
		printString("testToTempO(): failed!\r\n");
	}
}

int main(void) {

	initUSART();
	testToTempI();
	testToTempO();

	return 0;
}
