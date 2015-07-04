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

#define BEEPS 30
#define LENGTH 10
#define TONE 31

/** Age of previous measurements to compare against */
#define AGE_MEAS_PREV 180
/** Oven reaches at least this temperature when the fire is burning */
#define TEMP_MAX 800
/** Min. increase in temperature during AGE_MEAS_PREV when firing up */
#define TEMP_DELTA_UP 10
/** Min. decrease in temperature during AGE_MEAS_PREV when burning down */
#define TEMP_DELTA_DOWN 1
/** Min. temperature at which to set the air gate to 50% when firing up  */
#define TEMP_AIRGATE_50 500
/** Max. temperature at which to set the air gate to 25% when burning down */
#define TEMP_AIRGATE_25 800
/**
 * Max. temperature at which to set the air gate to 0% and to switch off the
 * oxygen sensor heater when burning down
 */
#define TEMP_AIRGATE_0 450
/** Max. temperature at which to consider the fire to have gone out */
#define TEMP_FIRE_OUT 100
/** Min. temperature at which to consider the fire to fire up again */
#define TEMP_FIRE_OUT_RESET 125
/** Fire is considered burning if lambda is below this value */
#define LAMBDA_MAX 2000
/** Combustion is considered too rich if lambda is below this value */
#define LAMBDA_TOO_RICH 1200
/** Combustion is considered lean enough again if lambda is above this value */
#define LAMBDA_TOO_RICH_RESET 1300
/** Combustion is considered too lean if lambda is above this value */
#define LAMBDA_TOO_LEAN 1600
/** Combustion is considered rich enough again if lambda is below this value */
#define LAMBDA_TOO_LEAN_RESET 1500

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
void resetRules(void);

#endif /* RULES_H_ */
