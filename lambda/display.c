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
#include "alert.h"

#define MENU_OFF 0
#define MENU_MAX_VALUES 1

uint8_t position = MENU_OFF;
bool updatePending = false;

measurement measLatest;
measurement measMax = {0, 0, 2000};

void cycleDisplay(void) {
	if (isAlertActive()) {
		// button pressed during alert
		cancelAlert();
		updatePending = true;
		return;
	}
	position++;
	if (position > MENU_MAX_VALUES) {
		position = MENU_OFF;
	}
	updatePending = true;
	beep(1, 2);
}

void updateMeas(measurement meas) {
	measLatest = meas;

	measMax.tempI = MAX(measMax.tempI, meas.tempI);
	measMax.tempO = MAX(measMax.tempO, meas.tempO);
	measMax.lambda = MIN(measMax.lambda, meas.lambda);

	updatePending = true;
}

void resetMeas(void) {
	measLatest.tempI = 0;
	measLatest.tempO = 0;
	measLatest.lambda = 2000;

	measMax.tempI = 0;
	measMax.tempO = 0;
	measMax.lambda = 2000;

	updatePending = true;
}

void updateDisplay(void) {
	if (isAlertActive()) {
		return;
	}

	if (position == MENU_MAX_VALUES) {
		displayMeas(measMax, "|>");
	} else {
		displayMeas(measLatest, "  ");
	}

	updatePending = false;
}

void updateDisplayIfRequested() {
	if (updatePending) {
		updateDisplay();
	}
}

void printMeas(measurement meas) {
	char log[64];
	snprintf(log, sizeof(log),
			"Ti %3d C - To %3d C - L %4u \r\n",
			meas.tempI, meas.tempO, meas.lambda);
	printString(log);
}

void displayMeas(measurement meas, char* hint) {
	uint16_t lambdax100 = divRoundNearest(meas.lambda, 10);
	div_t lambdaT = div(lambdax100, 100);

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

void displayText(char* line0, char* line1) {
	lcd_clear();
	lcd_setcursor(0, 1);
	lcd_string(line0);
	lcd_setcursor(0, 2);
	lcd_string(line1);
}
