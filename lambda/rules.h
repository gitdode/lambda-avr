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

typedef enum {
	none = 0,
	firing_up = 1,
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
