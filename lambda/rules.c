/*
 * rules.c
 *
 *  Created on: 22.05.2015
 *      Author: dode@luniks.net
 */

#include <string.h>
#include <avr/pgmspace.h>
#include "alert.h"
#include "integers.h"
#include "interrupts.h"
#include "rules.h"
#include "messages.h"

uint8_t age = 0;
FireDir dir = none;
uint8_t airgate = 100;

static Measurement rulesMeasMax = {0, 0, 2000, 0};
static Measurement rulesMeasPrev = {0, 0, 2000, 0};
static bool prevInit = false;

/**
 * Reminds to set the air gate to 50% when the fire is still firing up
 * and the temperature has reached TEMP_AIRGATE_50.
 */
static void airgate50(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == firing_up &&
			meas.tempI >= TEMP_AIRGATE_50) {
		airgate = 50;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Reminds to set the air gate to 25% when the fire is burning down and the
 * temperature went below TEMP_AIRGATE_25.
 */
static void airgate25(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == burning_down &&
			meas.tempI < TEMP_AIRGATE_25) {
		airgate = 25;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_25_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Reminds to close the air gate when the fire is burning down and the
 * temperature went below TEMP_AIRGATE_0 (no more flames).
 */
static void airgateClose(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == burning_down &&
			meas.tempI < TEMP_AIRGATE_0) {
		setHeaterOn(false);
		airgate = 0;
		alert_P(BEEPS, LENGTH, TONE,
				PSTR(MSG_AIRGATE_CLOSE_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Notifies that the combustion is too rich and to open the air gate.
 */
static void tooRich(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && meas.tempI > TEMP_FIRE_OUT &&
			meas.lambda < LAMBDA_TOO_RICH &&
			getHeaterState() == heaterStateReady && airgate < 100) {
		airgate = 100;
		alert_P(BEEPS, LENGTH, TONE,
				PSTR(MSG_TOO_RICH_0), PSTR(MSG_TOO_RICH_1), false);
		*fired = true;
	}
	if (meas.lambda >= LAMBDA_TOO_RICH_RESET) {
		*fired = false;
	}
}

/**
 * Notifies that the combustion is lean (again) and to set the air gate to 50%.
 */
static void tooLean(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && meas.tempI > TEMP_AIRGATE_50 &&
			meas.lambda > LAMBDA_TOO_LEAN &&
			getHeaterState() == heaterStateReady && airgate > 50) {
		airgate = 50;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
	if (meas.lambda <= LAMBDA_TOO_LEAN_RESET) {
		*fired = false;
	}
}

/**
 * Notifies that the fire might have gone out at the beginning of firing up.
 */
static void fireOut(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == firing_up && meas.tempI < TEMP_FIRE_OUT &&
			rulesMeasMax.tempI - meas.tempI > (TEMP_FIRE_OUT_RESET - TEMP_FIRE_OUT)) {
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_FIRE_OUT_0), PSTR(""), false);
		*fired = true;
	}
	if (meas.tempI >= TEMP_FIRE_OUT_RESET) {
		*fired = false;
	}
}

/**
 * Notifies that the heater is ready and sets the corresponding state.
 */
static void heaterReady(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! isHeaterOn() || getHeaterState() == heaterStateReady) {
		return;
	}
	if (meas.current <= milliAmpsReady && meas.current > milliAmpsDisconn) {
		setHeaterState(heaterStateReady);
		alert_P(3, 5, TONE, PSTR(MSG_HEATER_READY_0),
				PSTR(MSG_HEATER_READY_1), false);
	}
}

/**
 * Notifies that the heater or its connection is faulty and sets the
 * corresponding state.
 */
static void heaterFault(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! isHeaterOn() || getHeaterState() == heaterStateFault) {
		return;
	}
	if (meas.current > milliAmpsShort || meas.current < milliAmpsDisconn ||
			(getHeaterUptime() >= 300 && meas.current > milliAmpsReady)) {
		// short circuit or disconnected or did not warm up within 5 minutes
		setHeaterOn(false);
		setHeaterState(heaterStateFault);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_HEATER_FAULT_0),
				PSTR(MSG_HEATER_FAULT_1), true);
	}
}

/**
 * Switches the heater off if it is on for 15 mins or more and there does
 * not seem to be a fire, and notifies that the fire is out.
 */
static void heaterTimeout(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! isHeaterOn() || getHeaterState() == heaterStateFault) {
		return;
	}
	uint32_t heaterUptime = getHeaterUptime();
	if (heaterUptime >= 900 && meas.tempI < TEMP_FIRE_OUT) {
		setHeaterOn(false);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_FIRE_OUT_0), PSTR(""), false);
	}
	if (heaterUptime >= 1800 && meas.tempI < TEMP_AIRGATE_0) {
		setHeaterOn(false);
	}
}

/**
 * Rules applied to every nth averaged measurement.
 */
Rule rules[] = {
		{false, airgate50},
		{false, airgate25},
		{false, airgateClose},
		{false, tooRich},
		{false, tooLean},
		{false, fireOut}
};

/**
 * Heater rules applied to each not averaged measured heater current value.
 */
Rule heaterRules[] = {
		{false, heaterReady},
		{false, heaterFault},
		{false, heaterTimeout}
};

int8_t getDir(void) {
	return dir;
}

// called about every second
void reason(Measurement const meas) {

	// rules applied to every 10th measurement
	if (age % 10 == 0) {
		size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
		for (size_t i = 0; i < rulesSize; i++) {
			rules[i].cond(&(rules[i].fired), dir, meas);
		}
	}

	// rules applied to each measurement
	size_t heaterRulesSize = sizeof(heaterRules) / sizeof(heaterRules[0]);
	for (size_t i = 0; i < heaterRulesSize; i++) {
		heaterRules[i].cond(&(heaterRules[i].fired), dir, meas);
	}

	age++;

	// init previous measurements with current measurements
	if (! prevInit) {
		rulesMeasPrev = meas;
		prevInit = true;
	}

	// try to figure out if the fire is building up, burning or burning down
	// by comparing current measurements with ones that are 3 minutes old.
	if (age >= AGE_MEAS_PREV) {
		dir = none;
		if ((meas.tempI - rulesMeasPrev.tempI) >= TEMP_DELTA_UP &&
				rulesMeasMax.tempI < TEMP_MIN && meas.lambda >= LAMBDA_MAX) {
			dir = firing_up;
		} else if (meas.tempI > TEMP_MIN || meas.lambda < LAMBDA_MAX) {
			dir = burning;
		} else if ((rulesMeasPrev.tempI - meas.tempI) >= TEMP_DELTA_DOWN &&
				rulesMeasMax.tempI > TEMP_MIN && meas.lambda >= LAMBDA_MAX) {
			dir = burning_down;
		} else if ((meas.tempI - rulesMeasPrev.tempI) >= TEMP_DELTA_UP) {
			// it seems wood has been added - reset some measurements and rules
			// dir = warm_start;
			rulesMeasMax.tempI = meas.tempI;
			size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
			for (size_t i = 0; i < rulesSize; i++) {
				rules[i].fired = false;
			}
			if (! isHeaterOn() && getHeaterState() != heaterStateFault) {
				setHeaterOn(true);
			}
		}

		rulesMeasPrev = meas;
		age = 0;
	}

	rulesMeasMax.tempI = MAX(rulesMeasMax.tempI, meas.tempI);
}

void resetRules(void) {
	prevInit = false;
	rulesMeasMax.tempI = 0;

	age = 0;
	dir = none;
	airgate = 100;

	size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
	for (size_t i = 0; i < rulesSize; i++) {
		rules[i].fired = false;
	}

	size_t heaterRulesSize = sizeof(heaterRules) / sizeof(heaterRules[0]);
	for (size_t i = 0; i < heaterRulesSize; i++) {
		heaterRules[i].fired = false;
	}
}
