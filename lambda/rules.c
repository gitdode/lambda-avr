/*
 * rules.c
 *
 *  Created on: 22.05.2015
 *      Author: dode@luniks.net
 */

#include <avr/pgmspace.h>
#include "alert.h"
#include "integers.h"
#include "interrupts.h"
#include "rules.h"
#include "messages.h"

#define BEEPS 30
#define LENGTH 10
#define TONE 31

uint8_t age = 0;
FireDir dir = none;
uint8_t airgate = 100;

static Measurement rulesMeasMax = {0, 0, 2000, 0};
static Measurement rulesMeasPrev = {0, 0, 2000, 0};

int8_t getDir(void) {
	return dir;
}

/**
 * Reminds to set the air gate to 50% when the fire is still building up
 * and the temperature has reached 500°C.
 */
static void airgate50(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == firing_up && meas.tempI >= 500) {
		airgate = 50;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Reminds to set the air gate to 25% when the fire is burning down and the
 * temperature went below 800°C.
 */
static void airgate25(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == burning_down && meas.tempI < 800) {
		airgate = 25;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_25_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Reminds to close the air gate when the fire is burning down and the
 * temperature went below 400°C (no more flames).
 */
static void airgateClose(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == burning_down && meas.tempI < 450) {
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
	if (! *fired && meas.tempI > 100 && meas.lambda < 1200 &&
			getHeaterState() == heaterStateReady && airgate < 100) {
		airgate = 100;
		alert_P(BEEPS, LENGTH, TONE,
				PSTR(MSG_TOO_RICH_0), PSTR(MSG_TOO_RICH_1), false);
		*fired = true;
	}
	if (meas.lambda >= 1300) {
		*fired = false;
	}
}

/**
 * Notifies that the combustion is lean (again) and to set the air gate to 50%.
 */
static void tooLean(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && meas.tempI > 500 && meas.lambda > 1600 &&
			getHeaterState() == heaterStateReady && airgate > 50) {
		airgate = 50;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
	if (meas.lambda <= 1500) {
		*fired = false;
	}
}

/**
 * Notifies that the fire might have gone out at the beginning of building up.
 */
static void fireOut(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == firing_up && meas.tempI < 100 &&
			rulesMeasMax.tempI - meas.tempI > 25) {
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_FIRE_OUT_0), PSTR(""), false);
		*fired = true;
	}
	if (meas.tempI >= 125) {
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
			(getTime() >= 180 && meas.current > milliAmpsReady)) {
		// short circuit or disconnected or did not warm up within 3 minutes
		setHeaterOn(false);
		setHeaterState(heaterStateFault);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_HEATER_FAULT_0),
				PSTR(MSG_HEATER_FAULT_1), true);
	}
}

/**
 * Switches the heater off if it is still on after 3 hours and there does
 * not seem to be a fire.
 */
static void heaterTimeout(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (isHeaterOn() && getTime() >= 10800 && meas.tempI < 400) {
		setHeaterOn(false);
		alert_P(3, 5, TONE, PSTR(MSG_HEATER_OFF_0),
				PSTR(MSG_HEATER_OFF_1), false);
	}
}

// TODO what if fired up again without reset?

/**
 * Rules applied to every nth averaged measurement
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
 * Heater rules applied to each not averaged measured heater current value
 */
Rule heaterRules[] = {
		{false, heaterReady},
		{false, heaterFault},
		{false, heaterTimeout}
};


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

	// try to figure out if the fire is building up or burning down by
	// comparing current measurements with ones that are 3 minutes old.
	if (age >= 180) {
		dir = none;
		if ((meas.tempI - rulesMeasPrev.tempI) >= 10 &&
				rulesMeasMax.tempI < 800 && meas.lambda >= 2000) {
			dir = firing_up;
		} else if ((rulesMeasPrev.tempI - meas.tempI) >= 1 &&
				rulesMeasMax.tempI > 800 && meas.lambda >= 2000) {
			dir = burning_down;
		}

		rulesMeasPrev = meas;
		age = 0;
	}

	rulesMeasMax.tempI = MAX(rulesMeasMax.tempI, meas.tempI);
}

void resetRules(void) {
	rulesMeasPrev.tempI = 0;
	rulesMeasPrev.tempO = 0;
	rulesMeasPrev.lambda = 2000;
	rulesMeasPrev.current = 0;

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
