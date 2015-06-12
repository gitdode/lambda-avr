/*
 * lambda.c
 *
 * Program for an AVR micro controller based circuit to measure exhaust
 * gas temperatures and residual oxygen of a wood oven.
 *
 *  Created on: 22.02.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 *
 * ATTRIBUTION: This project includes the the Makefile from the code
 * accompanying the book Make: AVR Programming by Elliot Williams, a great book
 * and a pleasant read that helped me tremendously to get started with AVR
 * programming.
 * ATTRIBUTION: This project includes the module lcdroutines from
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "usart.h"
#include "lcdroutines.h"
#include "adc.h"
#include "interrupts.h"
#include "sensors.h"
#include "display.h"
#include "alert.h"
#include "command.h"
#include "rules.h"
#include "messages.h"

/**
 * Does initialization, measures, displays and logs the measurements and
 * runs commands sent via USART.
 */
int main(void) {
	initUSART();
	lcd_init();
	setupPorts();
	setupADC();
	setupSleepMode();
	initInterrupts();
	initTimers();

	alert_P(1, 2, 31, PSTR(MSG_WELCOME), PSTR(""), false);
	// spend some time on being polite
	while (getTime() < 3) {}
	setHeatingOn(true);
	while (getTime() < 4) {}

	uint32_t ints = 0;
	Measurement meas;

	// main loop
	while (true) {
		if (! isSimulation() && getInts() >= ints + INTS_PER_SEC &&
				getHeatingState() != HEATING_FAULT) {
			ints = getInts();
			meas = measure();
			if (isLogging()) {
				logMeas(meas);
			}
			if (getHeatingState() == HEATING_OFF ||
					getHeatingState() == HEATING_READY) {
				updateMeas(meas);
			}
			reason(meas);
		}
		if (! isSimulation()) {
			updateDisplayIfPending();
		}
		if (isUSARTReceived()) {
			char data[64];
			getUSARTData(data, sizeof(data));
			runCommand(data);
		}
	}

	// never reached
	return 0;
}
