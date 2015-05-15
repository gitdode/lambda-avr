/*
 * alert.c
 *
 * Functions to beep and show an alert on the display.
 *
 *  Created on: 24.04.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include "integers.h"
#include "alert.h"
#include "sensors.h"
#include "display.h"

uint8_t beepCount = 0;
uint16_t beepLength = 0;
uint8_t oscCount = 0;

static bool alertActive = false;

void oscillateBeep(void) {
	if (beepCount == 0) {
		oscCount = 0;
		return;
	}
	if (oscCount == 0) {
		// turn beep on
		TCCR1A |= (1 << COM1A0);
	}
	if (oscCount == beepLength) {
		// turn beep off
		TCCR1A &= ~(1 << COM1A0);
		beepCount--;
		if (beepCount == 0) {
			alertActive = false;
		}
	}
	oscCount == beepLength * 2 - 1 ? oscCount = 0 : oscCount++;
}

void beep(uint8_t const beeps, uint8_t const length, uint16_t const tone) {
	TCNT1 = 0;
	OCR1A = tone;
	oscCount = 0;
	beepCount = beeps;
	beepLength = length;
}

void alert(uint8_t const beeps, uint8_t const length, uint16_t tone,
		char* const line0, char* const line1) {
	TCNT1 = 0;
	OCR1A = tone;
	alertActive = true;
	oscCount = 0;
	beepCount = beeps;
	beepLength = length;
	displayText(line0, line1);
}

void cancelAlert(void) {
	beepCount = 0;
	oscCount = 0;
	// turn beep off
	TCCR1A &= ~(1 << COM1A0);
	alertActive = false;
}

bool isAlertActive(void) {
	return alertActive;
}
