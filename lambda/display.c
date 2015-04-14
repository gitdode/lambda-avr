/*
 * display.c
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "USART.h"
#include "lcdroutines.h"
#include "integers.h"
#include "sensors.h"
#include "display.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define MENU_OFF 0
#define MENU_MIN_VALUES 1
#define MENU_MAX_VALUES 2

uint8_t position = MENU_OFF;
char* hint = "  ";
measurement measCur;
measurement measMin = {0, 20, 0, 20, 0, 2000};
measurement measMax = {0, 0, 0, 0, 0, 0};

/**
 * Cycles through the "menu" (display options) when the menu button is pressed.
 */
ISR(PCINT0_vect) {
	if (bit_is_clear(PINB, PB0)) {
		_delay_ms(30);
		if (bit_is_clear(PINB, PB0)) {
			cycle();
		}
	}
}

void cycle(void) {
	position++;
	if (position == MENU_MIN_VALUES) {
		hint = " <";
		display(measMin);
	} else if (position == MENU_MAX_VALUES) {
		hint = " >";
		display(measMax);
	} else {
		position = MENU_OFF;
		hint = "  ";
		display(measCur);
	}
}

void update(measurement meas) {
	measCur = meas;

	measMin.tempI = MIN(measMin.tempI, meas.tempI);
	measMin.tempO = MIN(measMin.tempO, meas.tempO);
	measMin.lambda = MIN(measMin.lambda, meas.lambda);

	measMax.tempI = MAX(measMax.tempI, meas.tempI);
	measMax.tempO = MAX(measMax.tempO, meas.tempO);
	measMax.lambda = MAX(measMax.lambda, meas.lambda);

	if (position == MENU_OFF) {
		display(meas);
	}
}

void display(measurement meas) {
	uint16_t lambdax100 = divRoundNearest(meas.lambda, 10);
	div_t lambdaT = div(lambdax100, 100);

	char log[64];
	snprintf(log, sizeof(log),
			"Ti %3d C %4u - To %3d C %4u - L %4u %4u %s\r\n",
			meas.tempI, meas.tempIVoltage, meas.tempO, meas.tempOVoltage,
			meas.lambda, meas.lambdaVoltage, hint);
	printString(log);

	char line0[17];
	char line1[17];
	snprintf(line0, sizeof(line0), "Ti %3dC To %3dC ", meas.tempI, meas.tempO);
	snprintf(line1, sizeof(line1), "L  %d.%02d %s %s",
			lambdaT.quot, abs(lambdaT.rem), toInfo(lambdax100), hint);
	lcd_setcursor(0, 1);
	lcd_string(line0);
	lcd_setcursor(0, 2);
	lcd_string(line1);
}
