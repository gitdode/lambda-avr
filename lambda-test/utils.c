/*
 * utils.c
 *
 *  Created on: 28.03.2016
 *      Author: dode@luniks.net
 */

#include "utils.h"
#include "airgate.h"

void stepUntilDone(void) {
	while (isAirgateBusy()) {
		makeSteps();
	}
	makeSteps();
}
