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

#include "usart.h"

FireDir dir = none;
uint8_t airgate = 100;
uint8_t measCount = 10;

static int32_t tempIAvg = 0;
static int16_t tempIMax = 0;
static int16_t tempIOldQueue[QUEUE_SIZE];

/**
 * Pushes the given new value in the given fixed-size queue of old temperature
 * measurements and returns the oldest value being pushed off the queue.
 */
static int16_t push(int16_t queue[], const size_t size, const int16_t value) {
	int16_t last = queue[size - 1];
	for (size_t i = size - 1; i > 0; i--) {
		queue[i] = queue[i - 1];
	}
	queue[0] = value;

	return last;
}

/**
 * Reminds to set the air gate to 50% when the fire is still firing up
 * and the temperature has reached TEMP_AIRGATE_50.
 */
static void airgate50(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if ((dir == firing_up || dir == burning) &&
			meas.tempI >= TEMP_AIRGATE_50 && meas.lambda >= LAMBDA_TOO_LEAN &&
			airgate != 50) {

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
	if (dir == burning_down && meas.tempI < TEMP_AIRGATE_25 &&
			meas.lambda >= LAMBDA_TOO_LEAN && airgate > 25) {
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
	if (dir == burning_down && meas.tempI < TEMP_AIRGATE_0 &&
			meas.lambda >= LAMBDA_MAX && airgate > 0) {
		setHeaterState(heaterStateOff);
		airgate = 0;
		alert_P(BEEPS, LENGTH, TONE,
				PSTR(MSG_AIRGATE_CLOSE_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Notifies that combustion is (too) rich and suggests to set the air gate
 * to 50%.
 * It does however not seem to be a good idea to fully open the air gate as
 * this doesn't	make it noticeably leaner, but since the temperature of the
 * exhaust gas going into the chimney rises, more heat appears to be thrown
 * out of the chimney.
 */
static void tooRich(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (meas.tempI > TEMP_FIRE_OUT &&
			meas.lambda < LAMBDA_TOO_RICH &&
			getHeaterState() == heaterStateReady && airgate < 50) {
		airgate = 50;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Notifies that the combustion is lean (again) and suggests to set the air
 * gate to 50%.
 */
static void tooLean(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (meas.tempI > TEMP_AIRGATE_50 &&
			meas.lambda > LAMBDA_TOO_LEAN &&
			getHeaterState() == heaterStateReady && airgate > 50) {
		airgate = 50;
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Notifies that the fire might have gone out at the beginning of firing up.
 */
static void fireOut(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! *fired && dir == firing_up && meas.tempI < TEMP_FIRE_OUT &&
			tempIMax - meas.tempI > (TEMP_FIRE_OUT_RESET - TEMP_FIRE_OUT)) {
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_FIRE_OUT_0), PSTR(""), false);
		*fired = true;
	}
	if (meas.tempI >= TEMP_FIRE_OUT_RESET) {
		*fired = false;
	}
}

/**
 * Resets rules and some state and switches on the heating if it seems that
 * wood was added or the oven was fired up without resetting.
 * TODO make a complete reset including time?
 * TODO come up with something better than using the state of the heater
 */
static void warmStart(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (meas.tempI > TEMP_FIRE_OUT && (dir == firing_up || dir == burning) &&
			! isHeaterOn() && (getHeaterState() != heaterStateFault)) {
		resetRules(false);
		airgate = 100;
		tempIMax = meas.tempI;
		setHeaterState(heaterStateOn);
		*fired = true;
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
			(getHeaterUptime() >= 300 && getHeaterState() != heaterStateReady)) {
		// short circuit or disconnected or did not warm up within 5 minutes
		setHeaterState(heaterStateFault);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_HEATER_FAULT_0),
				PSTR(MSG_HEATER_FAULT_1), true);
	}
}

/**
 * Switches the heater off if it is on for 30 mins or more and there does
 * not seem to be a fire, and notifies that the fire is out.
 */
static void heaterTimeout(bool* const fired, int8_t const dir,
		Measurement const meas) {
	if (! isHeaterOn() || getHeaterState() == heaterStateFault) {
		return;
	}
	uint32_t heaterUptime = getHeaterUptime();
	if (heaterUptime >= 1800 && meas.tempI < TEMP_FIRE_OUT &&
			meas.lambda >= LAMBDA_MAX) {
		setHeaterState(heaterStateOff);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_FIRE_OUT_0), PSTR(""), false);
	}
	if (heaterUptime >= 10800 && meas.tempI < TEMP_AIRGATE_0 &&
			meas.lambda >= LAMBDA_MAX) {
		setHeaterState(heaterStateOff);
		if (airgate > 0) {
			alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_CLOSE_0), PSTR(""),
					false);
		} else {
			alert_P(3, 5, TONE, PSTR(MSG_HEATER_OFF_0), PSTR(MSG_HEATER_OFF_1),
					false);
		}
	}
}

/**
 * Heater rules applied to each not averaged measured heater current value.
 */
Rule heaterRules[] = {
		{false, heaterReady},
		{false, heaterFault},
		{false, heaterTimeout}
};

/**
 * Rules applied to every nth averaged measurement.
 */
Rule rules[] = {
		{false, airgate50},
		{false, airgate25},
		{false, airgateClose},
		{false, tooRich},
		{false, tooLean},
		{false, fireOut},
		{false, warmStart}
};

int8_t getDir(void) {
	return dir;
}

// called about every second
void reason(Measurement const meas) {

	// rules applied to each measurement
	size_t heaterRulesSize = sizeof(heaterRules) / sizeof(heaterRules[0]);
	for (size_t i = 0; i < heaterRulesSize; i++) {
		heaterRules[i].cond(&(heaterRules[i].fired), dir, meas);
	}

	// rules applied to every 10th measurement
	if (measCount == 10) {
		measCount = 0;

		size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
		for (size_t i = 0; i < rulesSize; i++) {
			rules[i].cond(&(rules[i].fired), dir, meas);
		}

		tempIAvg = meas.tempI + tempIAvg - ((tempIAvg - 4) >> 3);
		int16_t tempICur = tempIAvg >> 3;
		int16_t tempIOld = push(tempIOldQueue, QUEUE_SIZE, tempICur);

		// simply skip if old temperature was 0°C - in practice it never goes
		// below 3°C anyway.
		if (tempIOld > 0) {
			// try to figure out if the fire is building up, burning or burning
			// down by comparing the current temperature value with one that is
			// 3 minutes old
			dir = none;
			if ((tempICur - tempIOld) >= TEMP_DELTA_UP &&
					tempICur < TEMP_MIN && meas.lambda >= LAMBDA_BURNING) {
				dir = firing_up;
			}
			if (tempICur >= TEMP_MIN || meas.lambda < LAMBDA_BURNING) {
				dir = burning;
			}
			if ((tempIOld - tempICur) >= TEMP_DELTA_DOWN &&
					tempICur < TEMP_MIN && meas.lambda >= LAMBDA_BURNING &&
					tempIMax >= TEMP_AIRGATE_50) {
				dir = burning_down;
			}
		}
	}

	measCount++;
	tempIMax = MAX(tempIMax, meas.tempI);
}

void resetRules(bool const state) {
	if (state) {
		tempIMax = 0;
		measCount = 10;
		for (size_t i = 0; i < QUEUE_SIZE; i++) {
			tempIOldQueue[i] = 0;
		}
		dir = none;
		airgate = 100;
	}

	size_t rulesSize = sizeof(rules) / sizeof(rules[0]);
	for (size_t i = 0; i < rulesSize; i++) {
		rules[i].fired = false;
	}

	size_t heaterRulesSize = sizeof(heaterRules) / sizeof(heaterRules[0]);
	for (size_t i = 0; i < heaterRulesSize; i++) {
		heaterRules[i].fired = false;
	}
}
