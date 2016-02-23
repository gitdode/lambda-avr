/*
 * alert.c
 *
 * Functions to beep and show an alert on the display.
 *
 *  Created on: 24.04.2015
 *      Author: dode@luniks.net
 *
 */

#include <avr/io.h>
#include <util/atomic.h>
#include "integers.h"
#include "alert.h"
#include "sensors.h"
#include "display.h"
#include "pins.h"

volatile uint8_t beepCount = 0;
volatile uint16_t beepLength = 0;
volatile uint8_t oscCount = 0;

volatile static bool alertActive = false;
volatile static bool keepActive = false;

void oscillateBeep(void) {
	if (beepCount == 0) {
		oscCount = 0;
		return;
	}
	if (oscCount == 0) {
		// turn beep on
		TCCR1A |= (1 << PIN_BEEPER_TOGGLE);
	}
	if (oscCount == beepLength) {
		// turn beep off
		TCCR1A &= ~(1 << PIN_BEEPER_TOGGLE);
		beepCount--;
		if (beepCount == 0 && ! keepActive) {
			alertActive = false;
		}
	}
	oscCount == beepLength * 2 - 1 ? oscCount = 0 : oscCount++;
}

void beep(uint8_t const beeps, uint8_t const length, uint16_t const tone) {
	OCR1A = tone;
	if (TCNT1 >= tone) TCNT1 = 0;
	oscCount = 0;
	beepCount = beeps;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		beepLength = length;
	}
}

void alert(uint8_t const beeps, uint8_t const length, uint16_t const tone,
		const char* const line0, const char* const line1,
		bool const keep) {
	OCR1A = tone;
	if (TCNT1 >= tone) TCNT1 = 0;
	alertActive = true;
	keepActive = keep;
	oscCount = 0;
	beepCount = beeps;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		beepLength = length;
	}
	displayText(line0, line1);
}

void alert_P(uint8_t const beeps, uint8_t const length, uint16_t const tone,
		PGM_P const line0_P, PGM_P const line1_P, bool const keep) {
	char line0[17];
	char line1[17];
	strncpy_P(line0, line0_P, sizeof(line0));
	strncpy_P(line1, line1_P, sizeof(line1));

	alert(beeps, length, tone, line0, line1, keep);
}

void cancelAlert() {
	beepCount = 0;
	oscCount = 0;
	// turn beep off
	TCCR1A &= ~(1 << PIN_BEEPER_TOGGLE);
	alertActive = false;
}

bool isAlertActive(void) {
	return alertActive;
}

bool isBeeping(void) {
	return beepCount > 0;
}
