/*
 * command.c
 *
 *  Created on: 02.05.2015
 *      Author: dode@luniks.net
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#include "sensors.h"
#include "display.h"
#include "alert.h"
#include "command.h"

bool simulation = false;

void command(char* data) {
	char* fields[8];
	char* token = strtok(data, " ");
	uint8_t index = 0;
	while (token != NULL) {
		fields[index++] = token;
		token = strtok(NULL, " ");
	}
	if (strcmp(fields[0], "sien") == 0) {
		simulation = true;
		beep(1, 2);
	}
	else if (strcmp(fields[0], "sidi") == 0) {
		simulation = false;
		beep(1, 2);
	}
	else if (strcmp(fields[0], "menu") == 0) {
		cycleDisplay();
		// updateMeas(meas);
	}
	else if (strcmp(fields[0], "tsal") == 0) {
		alert(3, 20, "Beep Beep Beep!", fields[1]);
	}
	else if (simulation) {
		measurement meas = readMeas(fields);
		updateMeas(meas);
	}
}

bool isSimulation(void) {
	return simulation;
}
