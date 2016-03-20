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
#include "airgate.h"
#include "scheduler.h"

#include "usart.h"

uint8_t measCount = MEAS_INT;
FireState state = undefined;

static int32_t deltaAvg = 0;
static int16_t tempIMax = TEMP_INIT;
static int16_t tempIDiffQueue[QUEUE_SIZE];

/**
 * Pushes the given new value in the queue of temperature differences
 * and returns the oldest value being pushed off the array.
 */
static int16_t pushQueue(int16_t const value) {
	int16_t last = tempIDiffQueue[QUEUE_SIZE - 1];
	for (size_t i = QUEUE_SIZE - 1; i > 0; i--) {
		tempIDiffQueue[i] = tempIDiffQueue[i - 1];
	}
	tempIDiffQueue[0] = value;

	return last;
}

/**
 * Initializes all elements in the queue of temperature differences
 * to the given value.
 */
static void initQueue(int16_t const value) {
	for (size_t i = 0; i < QUEUE_SIZE; i++) {
		tempIDiffQueue[i] = value;
	}
}

/**
 * Closes the airgate and puts the motor driver in sleep mode after a delay,
 * giving the motor sufficient time.
 */
static void closeAirgateAndSleep(void) {
	setAirgate(AIRGATE_CLOSE);
	void func(void) {
		setSleepMode(true);
	}
	// put stepper motor driver in sleep mode in 60 seconds
	scheduleTask(func, 60);
}

/**
 * Reminds to set the air gate to 50% when the fire is still firing up
 * and the temperature has reached TEMP_AIRGATE_50.
 */
static void airgate50(bool* const fired, Measurement const meas) {
	if ((state == firing_up) && ! isAirgateBusy() &&
			meas.tempI >= TEMP_AIRGATE_50 &&
			meas.lambda >= LAMBDA_TOO_LEAN &&
			getAirgate() != AIRGATE_50) {
		setAirgate(AIRGATE_50);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Reminds to set the air gate to 25% when the fire is burning down and the
 * temperature went below TEMP_AIRGATE_25.
 */
static void airgate25(bool* const fired, Measurement const meas) {
	if (state == burning_down && ! isAirgateBusy() &&
			meas.tempI < TEMP_AIRGATE_25 &&
			meas.lambda >= LAMBDA_TOO_LEAN && getAirgate() > AIRGATE_25) {
		setAirgate(AIRGATE_25);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_25_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Reminds to close the air gate when the fire is burning down and the
 * temperature went below TEMP_AIRGATE_0 (no more flames).
 */
static void airgateClose(bool* const fired, Measurement const meas) {
	if (state == burning_down && ! isAirgateBusy() &&
			meas.tempI < TEMP_AIRGATE_0 &&
			meas.lambda >= LAMBDA_MAX && getAirgate() > AIRGATE_CLOSE) {
		setHeaterState(heaterStateOff);
		closeAirgateAndSleep();
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
static void tooRich(bool* const fired, Measurement const meas) {
	if (! isAirgateBusy() &&
			meas.tempI > TEMP_FIRE_OUT && meas.lambda < LAMBDA_TOO_RICH &&
			getHeaterState() == heaterStateReady && getAirgate() < AIRGATE_50) {
		setAirgate(AIRGATE_50);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Notifies that the combustion is lean (again) and suggests to set the air
 * gate to 50%.
 */
static void tooLean(bool* const fired, Measurement const meas) {
	if (! isAirgateBusy() &&
			meas.tempI > TEMP_AIRGATE_50 &&	meas.lambda > LAMBDA_TOO_LEAN &&
			getHeaterState() == heaterStateReady && getAirgate() > AIRGATE_50) {
		setAirgate(AIRGATE_50);
		alert_P(BEEPS, LENGTH, TONE, PSTR(MSG_AIRGATE_50_0), PSTR(""), false);
		*fired = true;
	}
}

/**
 * Notifies that the fire might have gone out at the beginning of firing up.
 */
static void fireOut(bool* const fired, Measurement const meas) {
	if (! *fired && meas.tempI < TEMP_FIRE_OUT &&
			tempIMax < TEMP_AIRGATE_50 &&
			tempIMax >= TEMP_FIRE_OUT_RESET) {
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
 */
static void warmStart(bool* const fired, Measurement const meas) {
	if (! *fired && state == firing_up &&
			meas.tempI > TEMP_FIRE_OUT && tempIMax >= TEMP_AIRGATE_50) {
		setSleepMode(false);
		resetRules(false);
		tempIMax = meas.tempI;
		if (getHeaterState() != heaterStateFault) {
			setHeaterState(heaterStateOn);
		}
		setAirgate(AIRGATE_OPEN);
		*fired = true;
	}
	if (state == burning_down && meas.tempI < TEMP_AIRGATE_0) {
		*fired = false;
	}
}

/**
 * Notifies that the heater is ready and sets the corresponding state.
 */
static void heaterReady(bool* const fired, Measurement const meas) {
	if (getHeaterState() == heaterStateOff ||
			getHeaterState() == heaterStateReady) {
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
static void heaterFault(bool* const fired, Measurement const meas) {
	if (getHeaterState() == heaterStateOff ||
			getHeaterState() == heaterStateFault) {
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
 * Switches the heater off if it is on for a while and there does not seem
 * to be a fire.
 */
static void heaterTimeout(bool* const fired, Measurement const meas) {
	if (getHeaterState() == heaterStateOff ||
			getHeaterState() == heaterStateFault) {
		return;
	}
	uint32_t heaterUptime = getHeaterUptime();
	if (heaterUptime >= 1800 && meas.tempI < TEMP_FIRE_OUT &&
			meas.lambda >= LAMBDA_MAX) {
		setHeaterState(heaterStateOff);
		closeAirgateAndSleep();
	}
	if (heaterUptime >= 10800 && meas.tempI < TEMP_AIRGATE_0 &&
			meas.lambda >= LAMBDA_MAX) {
		setHeaterState(heaterStateOff);
		closeAirgateAndSleep();
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
		{true, warmStart}
};

int8_t getState(void) {
	return state;
}

// called about every second
void reason(Measurement const meas) {

	tempIMax = MAX(tempIMax, meas.tempI);

	// rules applied at each measurement
	for (size_t i = 0; i < ARRAY_LENGTH(heaterRules); i++) {
		heaterRules[i].cond(&(heaterRules[i].fired), meas);
	}

	// evaluation of the fire state and rules applied
	// at every MEAS_INT'th measurement
	if (measCount == MEAS_INT) {
		measCount = 0;

		for (size_t i = 0; i < ARRAY_LENGTH(rules); i++) {
			rules[i].cond(&(rules[i].fired), meas);
		}

		// difference between current and previous temperature
		int16_t tempIOld = pushQueue(meas.tempI);
		int16_t delta = meas.tempI - tempIOld;

		// extra smoothing of the temperature difference
		deltaAvg = delta + deltaAvg - (deltaAvg >> TEMP_DELTA_SMOOTH);
		int16_t deltaSmooth = deltaAvg >> TEMP_DELTA_SMOOTH;

		// try to figure out if the fire is firing up or burning down
		state = undefined;
		if (deltaSmooth >= TEMP_DELTA_UP) {
			state = firing_up;
		}
		if (deltaSmooth <= TEMP_DELTA_DOWN && tempIMax >= TEMP_AIRGATE_50) {
			state = burning_down;
		}
	}

	measCount++;
}

void resetRules(bool const intState) {
	if (intState) {
		deltaAvg = 0;
		tempIMax = TEMP_INIT;
		initQueue(TEMP_INIT);
		measCount = MEAS_INT;
		state = undefined;
	}

	for (size_t i = 0; i < ARRAY_LENGTH(rules); i++) {
		rules[i].fired = false;
	}
	// default for warmStart should be true
	rules[6].fired = true;

	for (size_t i = 0; i < ARRAY_LENGTH(heaterRules); i++) {
		heaterRules[i].fired = false;
	}
}
