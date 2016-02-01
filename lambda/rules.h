/*
 * rules.h
 *
 *  Created on: 22.05.2015
 *      Author: dode@luniks.net
 */

#ifndef RULES_H_
#define RULES_H_

#include <stdbool.h>
#include "sensors.h"

#define BEEPS 10
#define LENGTH 10
#define TONE 31

/** Some rules and the fire state is evaluated every so many measurements */
#define MEAS_INT 10
/**
 * Last 18 firebox temperature values updated every MEAS_INT measurements
 * (i.e. 18 * 10 = 3 minutes @ one measurement per second)
 */
#define QUEUE_SIZE 18
/** Initial temperature value */
#define TEMP_INIT 20
/** Smoothing of the temperature difference used to determine the fire state */
#define TEMP_DELTA_SMOOTH 6
/** Min. increase in temp. during QUEUE_SIZE * MEAS_INT when firing up */
#define TEMP_DELTA_UP 10
/** Min. decrease in temp. during QUEUE_SIZE * MEAS_INT when burning down */
#define TEMP_DELTA_DOWN -3
/** Temperature at which to set the air gate to 50% when firing up  */
#define TEMP_AIRGATE_50 500
/** Temperature at which to set the air gate to 25% when burning down */
#define TEMP_AIRGATE_25 700
/**
 * Max. temperature at which to set the air gate to 0% and to switch off the
 * oxygen sensor heater when burning down
 */
#define TEMP_AIRGATE_0 450
/** Temperature at which to consider the fire to have gone out */
#define TEMP_FIRE_OUT 100
/** Min. temperature at which to consider the fire to fire up again */
#define TEMP_FIRE_OUT_RESET 110
/** Max. lambda value that can be measured */
#define LAMBDA_MAX 2000
/** Combustion is considered too rich if lambda is below this value */
#define LAMBDA_TOO_RICH 1100
/**
 * Combustion is considered too lean if lambda is above this value
 * Too lean here rather means lean enough so the air gate can be set to 50%
 */
#define LAMBDA_TOO_LEAN 1400

typedef enum {
	undefined = 0,
	firing_up = 1,
	// burning = 2,
	burning_down = -1
} FireState;

/**
 * An attempt to create some sort of rule "object".
 */
typedef struct {
	bool fired;
	void (*cond)(bool* fired, Measurement meas);
} Rule;

/**
 * Returns the current state of the fire, i.e. burning_down when it is burning
 * down.
 */
int8_t getState(void);

/**
 * Applies all rules against the given measurements.
 */
void reason(Measurement meas);

/**
 * Resets all rules and internal state.
 */
void resetRules(bool state);

#endif /* RULES_H_ */
