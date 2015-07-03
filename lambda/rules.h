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

#define TEMP_MAX 800
#define TEMP_DELTA_UP 10
#define TEMP_DELTA_DOWN 1
#define TEMP_AIRGATE_50 500
#define TEMP_AIRGATE_25 800
#define TEMP_AIRGATE_0 450
#define TEMP_FIRE_OUT 100
#define TEMP_FIRE_OUT_RESET 125
#define LAMBDA_MAX 2000
#define LAMBDA_TOO_RICH 1200
#define LAMBDA_TOO_RICH_RESET 1300
#define LAMBDA_TOO_LEAN 1600
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
