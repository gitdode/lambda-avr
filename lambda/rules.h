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

#define BEEPS 20
#define LENGTH 10
#define TONE 31

/** Exhaust reaches at least this temperature when the fire is burning */
#define TEMP_MIN 700
/** Min. increase in temperature during AGE_TEMPI_OLD when firing up */
#define TEMP_DELTA_UP 10
/** Min. decrease in temperature during AGE_TEMPI_OLD when burning down */
#define TEMP_DELTA_DOWN 1
/** Min. temperature at which to set the air gate to 50% when firing up  */
#define TEMP_AIRGATE_50 500
/** Max. temperature at which to set the air gate to 25% when burning down */
#define TEMP_AIRGATE_25 700
/**
 * Max. temperature at which to set the air gate to 0% and to switch off the
 * oxygen sensor heater when burning down
 */
#define TEMP_AIRGATE_0 450
/** Max. temperature at which to consider the fire to have gone out */
#define TEMP_FIRE_OUT 100
/** Min. temperature at which to consider the fire to fire up again */
#define TEMP_FIRE_OUT_RESET 125
/** Max. lambda value that can be measured */
#define LAMBDA_MAX 2000
/** Fire is considered fully burning if lambda is below this value */
#define LAMBDA_BURNING 1900
/** Combustion is considered too rich if lambda is below this value */
#define LAMBDA_TOO_RICH 1100
/**
 * Combustion is considered too lean if lambda is above this value
 * Too lean here rather means lean enough so the air gate can be set to 50%
 */
#define LAMBDA_TOO_LEAN 1400

/** Last 18 firebox temperature values updated every 10 seconds = 3 minutes */
#define QUEUE_SIZE 18

typedef enum {
	none = 0,
	firing_up = 1,
	burning = 2,
	burning_down = -1
} FireDir;

/**
 * An attempt to create some sort of rule "object".
 */
typedef struct {
	bool fired;
	void (*cond)(bool* fired, int8_t dir, Measurement meas);
} Rule;

/**
 * Returns the "direction" of the fire, i.e. DIR_BURN_DOWN when it is burning
 * down.
 */
int8_t getDir(void);

/**
 * Applies all rules against the given measurements.
 */
void reason(Measurement meas);

/**
 * Resets all rules and internal state.
 */
void resetRules(bool state);

#endif /* RULES_H_ */
