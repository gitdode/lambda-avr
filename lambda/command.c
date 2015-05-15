/*
 * command.c
 *
 * Functions to evaluate and run commands sent via USART.
 *
 *  Created on: 02.05.2015
 *      Author: dode@luniks.net
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "sensors.h"
#include "display.h"
#include "alert.h"
#include "command.h"
#include "strings.h"

static bool simulation = false;
static bool logging = false;

bool isSimulation(void) {
	return simulation;
}

bool isLogging(void) {
	return logging;
}

void runCommand(char* const data) {
	size_t fieldCount = 8;
	char* fields[fieldCount];
	split(data, " ", fields, fieldCount);
	if (strcmp_P(fields[0], PSTR("se")) == 0) {
		// simulation enable
		resetMeas();
		simulation = true;
		beep(1, 2, 31);
	}
	else if (strcmp_P(fields[0], PSTR("sd")) == 0) {
		// simulation disable
		resetMeas();
		simulation = false;
		beep(1, 2, 31);
	}
	else if (strcmp_P(fields[0], PSTR("le")) == 0) {
		// logging enable
		logging = true;
		beep(1, 2, 31);
	}
	else if (strcmp_P(fields[0], PSTR("ld")) == 0) {
		// logging disable
		logging = false;
		beep(1, 2, 31);
	}
	else if (strcmp_P(fields[0], PSTR("cm")) == 0) {
		// cycle menu
		cycleDisplay();
	}
	else if (strcmp_P(fields[0], PSTR("ta")) == 0) {
		// test alert
		char buf[16];
		strcpy_P(buf, PSTR("Beep Beep Beep!"));
		alert(3, 10, 15, buf, fields[1]);
	}
	else if (strcmp_P(fields[0], PSTR("tb")) == 0) {
		// test beep
		uint8_t length = 2;
		if (fields[1] != '\0') {
			length = atoi(fields[1]);
		}
		uint16_t tone = 31;
		if (fields[2] != '\0') {
			tone = atoi(fields[2]);
		}
		beep(1, length, tone);
	}
	else if (simulation) {
		Measurement meas = readMeas(fields, fieldCount);
		updateMeas(meas);
	}
}

