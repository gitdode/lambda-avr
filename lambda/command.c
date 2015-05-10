/*
 * command.c
 *
 *  Created on: 02.05.2015
 *      Author: dode@luniks.net
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#include "sensors.h"
#include "display.h"
#include "alert.h"
#include "command.h"
#include "strings.h"

bool simulation = false;
bool logging = false;

bool isSimulation(void) {
	return simulation;
}

bool isLogging(void) {
	return logging;
}

void runCommand(char* data) {
	char* fields[8];
	split(data, " ", fields, 8);
	if (strcmp(fields[0], "se") == 0) {
		// simulation enable
		resetMeas();
		simulation = true;
		beep(1, 2);
	}
	else if (strcmp(fields[0], "sd") == 0) {
		// simulation disable
		resetMeas();
		simulation = false;
		beep(1, 2);
	}
	else if (strcmp(fields[0], "le") == 0) {
		// logging enable
		logging = true;
		beep(1, 2);
	}
	else if (strcmp(fields[0], "ld") == 0) {
		// logging disable
		logging = false;
		beep(1, 2);
	}
	else if (strcmp(fields[0], "cm") == 0) {
		// cycle menu
		cycleDisplay();
	}
	else if (strcmp(fields[0], "ta") == 0) {
		// test alert
		alert(1, 20, "Beep!", fields[1]);
	}
	else if (simulation) {
		measurement meas = readMeas(fields);
		updateMeas(meas);
	}
}

