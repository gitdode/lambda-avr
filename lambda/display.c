/*
 * display.c
 *
 * Functions to format, display and log measurements.
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "lcdroutines.h"
#include "integers.h"
#include "interrupts.h"
#include "display.h"
#include "alert.h"
#include "messages.h"

#define MENU_OFF 0
#define MENU_MAX_VALUES 1
#define MENU_CURRENT 2
#define MENU_LAST_TEXT 3
#define MENU_TIME 4

uint8_t position = MENU_OFF;
bool updatePending = false;
Measurement measLatest = {0, 0, 2000, 0};
Measurement measMax = {0, 0, 2000, 0};

static char lastLine0[17];
static char lastLine1[17];

/**
 * Sets the given two lines of text on the display.
 */
static void setText(const char* const line0, const char* const line1) {
	lcd_setcursor(0, 1);
	lcd_string(line0);
	lcd_setcursor(0, 2);
	lcd_string(line1);
}

/**
 * Formats the given measurement values and displays them along with the
 * given hint.
 */
static void displayMeas(Measurement const meas, char* const hint) {
	uint16_t lambdax100 = divRoundNearest(meas.lambda, 10);
	div_t lambdaT = div(lambdax100, 100);

	char line0[17];
	char line1[17];
	snprintf(line0, sizeof(line0), "Ti %3dC To %3dC ", meas.tempI, meas.tempO);
	snprintf(line1, sizeof(line1), "L  %d.%02d %s %s",
			lambdaT.quot, abs(lambdaT.rem), toInfo(lambdax100), hint);
	setText(line0, line1);
}

/**
 * Formats the given heater current and displays it.
 */
static void displayCurrent(uint16_t const current) {
	div_t ampsT = div(current, 1000);

	char line1[17];
	snprintf(line1, sizeof(line1), "%d.%02dA",
			ampsT.quot, abs(ampsT.rem));
	setText(MSG_HEATING_CURRENT, line1);
}

/**
 * Formats and displays the time since last start/reset.
 */
static void displayTime(void) {
	char line1[12];
	formatTime(line1, sizeof(line1));
	setText(MSG_TIME_SINCE_START, line1);
}

void cycleDisplay(void) {
	if (isAlertActive()) {
		// button pressed during alert
		cancelAlert(false);
		lcd_clear();
		updatePending = true;
		return;
	}
	position++;
	if (position > MENU_TIME) {
		position = MENU_OFF;
	}
	lcd_clear();
	updatePending = true;
	beep(1, 2, 31);
}

void updateMeas(Measurement const meas) {
	measLatest = meas;

	measMax.tempI = MAX(measMax.tempI, meas.tempI);
	measMax.tempO = MAX(measMax.tempO, meas.tempO);
	measMax.lambda = MIN(measMax.lambda, meas.lambda);

	updatePending = true;
}

void resetDisplay(void) {
	measMax.tempI = 0;
	measMax.tempO = 0;
	measMax.lambda = 2000;

	updatePending = true;
}

void updateDisplayIfPending() {
	if (updatePending && ! isAlertActive()) {
		updatePending = false;

		if (position == MENU_MAX_VALUES) {
			displayMeas(measMax, "|>");
		} else if (position == MENU_LAST_TEXT) {
			setText(lastLine0, lastLine1);
		} else if (position == MENU_CURRENT) {
			displayCurrent(measLatest.current);
		} else if (position == MENU_TIME) {
			displayTime();
		} else {
			displayMeas(measLatest, "  ");
		}
	}
}

void logMeas(Measurement const meas) {
	char log[64];
	snprintf(log, sizeof(log),
			"Ti %3d C - To %3d C - L %u - C %4u\r\n",
			meas.tempI, meas.tempO, meas.lambda, meas.current);
	printString(log);
}

void displayText(const char* const line0, const char* const line1) {
	lastLine0[0] = '\0';
	lastLine1[0] = '\0';
	strncat(lastLine0, line0, sizeof(lastLine0) - 1);
	strncat(lastLine1, line1, sizeof(lastLine1) - 1);
	lcd_clear();
	setText(line0, line1);
}
