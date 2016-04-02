/*
 * utils.c
 *
 *  Created on: 28.03.2016
 *      Author: dode@luniks.net
 */

#include "utils.h"
#include "airgate.h"
#include "pins.h"

void stepUntilDone(void) {
	while (isAirgateBusy()) {
		makeSteps();
	}
	makeSteps();
}

void setDriverFault(bool const fault) {
	if (fault) {
		// disable pull-up resistor
		PORT &= ~(1 << PIN_FAULT);
	} else {
		// enable pull-up resistor
		PORT |= (1 << PIN_FAULT);
	}
}
