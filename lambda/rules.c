/*
 * rules.c
 *
 *  Created on: 22.05.2015
 *      Author: dode@luniks.net
 */

#include <avr/pgmspace.h>
#include "alert.h"
#include "integers.h"
#include "rules.h"
#include "messages.h"

#define BEEPS 60
#define LENGTH 20
#define TONE 31

#define DIR_NONE 0
#define DIR_BURN_UP 1
#define DIR_BURN_DOWN -1

Measurement rulesMeasMax = {0, 0, 2000};
Measurement rulesMeasPrev = {0, 0, 2000};
uint16_t age = 0;
int8_t dir = 0;

/**
 * Reminds to set the air gate to 50% when the fire is still building up
 * and the temperature has reached 500°C.
 */
static void airgate50(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == DIR_BURN_UP && meas.tempI >= 500) {
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""));
		*fired = true;
	}
}

/**
 * Reminds to set the air gate to 25% when the fire is burning down and the
 * temperature went below 800°C.
 */
static void airgate25(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == DIR_BURN_DOWN && meas.tempI < 800) {
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""));
		*fired = true;
	}
}

/**
 * Reminds to close the air gate when the fire is burning down and the
 * temperature went below 400°C.
 */
static void airgateClose(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == DIR_BURN_DOWN && meas.tempI < 400) {
		alert_P(BEEPS, LENGTH, TONE,
				PSTR(MSG_AIRGATE_CLOSE_0), PSTR(MSG_AIRGATE_CLOSE_1));
		*fired = true;
	}
}

/**
 * Notifies that the combustion is too rich and to open the air gate
 * if possible.
 */
static void tooRich(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && meas.tempI >= 100 && meas.lambda < 1200) {
		alert_P(BEEPS, LENGTH, TONE,
				PSTR(MSG_TOO_RICH_0), PSTR(MSG_TOO_RICH_1));
		*fired = true;
	}
	if (meas.lambda >= 1300) {
		*fired = false;
	}
}

/**
 * Notifies that the might have gone out at the beginning of building up.
 */
static void fireOut(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == DIR_BURN_UP && meas.tempI < 100 &&
			meas.tempI < rulesMeasMax.tempI) {
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_FIRE_OUT_0), PSTR(""));
		*fired = true;
	}
	if (meas.tempI >= 100) {
		*fired = false;
	}
}

/**
 * Array of rules.
 */
Rule rules[] = {
		{false, airgate50},
		{false, airgate25},
		{false, airgateClose},
		{false, tooRich},
		{false, fireOut}
};

// called about every second
void reason(Measurement const meas) {

	// apply the rules about every 10 seconds
	if (age % 10 == 0) {
		size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
		for (size_t i = 0; i < rulesSize; i++) {
			rules[i].cond(&(rules[i].fired), dir, meas);
		}
	}

	// try to figure out if the fire is building up or burning down by
	// comparing current measurements with ones that are 3 minutes old.
	if (age >= 180) {
		dir = DIR_NONE;
		if ((meas.tempI - rulesMeasPrev.tempI) >= 10 &&
				rulesMeasMax.tempI < 800 && meas.lambda >= 2000) {
			dir = DIR_BURN_UP;
		} else if ((rulesMeasPrev.tempI - meas.tempI) >= 1 &&
				rulesMeasMax.tempI > 800 && meas.lambda >= 2000) {
			dir = DIR_BURN_DOWN;
		}

		rulesMeasPrev = meas;
		age = 0;
	}

	rulesMeasMax.tempI = MAX(rulesMeasMax.tempI, meas.tempI);
	rulesMeasMax.tempO = MAX(rulesMeasMax.tempO, meas.tempO);
	rulesMeasMax.lambda = MIN(rulesMeasMax.lambda, meas.lambda);

	age++;
}

void resetRules(void) {
	rulesMeasPrev.tempI = 0;
	rulesMeasPrev.tempO = 0;
	rulesMeasPrev.lambda = 2000;

	rulesMeasMax.tempI = 0;
	rulesMeasMax.tempO = 0;
	rulesMeasMax.lambda = 2000;

	age = 0;
	dir = DIR_NONE;

	size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
	for (size_t i = 0; i < rulesSize; i++) {
		rules[i].fired = false;
	}
}
