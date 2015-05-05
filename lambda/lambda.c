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
 * ATTRIBUTION: This project includes the module USART and the Makefile from
 * the code accompanying the book Make: AVR Programming by Elliot Williams,
 * a great book and a pleasant read, that helped me tremendously to get
 * started with AVR programming.
 * ATTRIBUTION: This project includes the module lcdroutines from
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "lcdroutines.h"
#include "adc.h"
#include "interrupts.h"
#include "sensors.h"
#include "display.h"
#include "alert.h"
#include "command.h"

/**
 * Does initialization and measures, displays and logs the measurements
 * infinitely.
 */
int main(void) {
	initUSART();
	lcd_init();
	setupPorts();
	setupADC();
	setupSleepMode();
	initInterrupts();
	initTimers();

	alert(1, 2, "     Hello!     ", "");

	measurement meas;

	// main loop
	while (1) {
		if (hasIntCount(62, true) && ! isSimulation()) {
			meas = measure();
			if (isLogging()) {
				printMeas(meas);
			}
			updateMeas(meas);
		}
		if (isButtonPressed()) {
			// update display immediately
			updateMeas(meas);
		}
		if (isUSARTReceived()) {
			char data[64];
			getUSARTData(data, sizeof(data));
			command(data);
		}
	}

	// never reached
	return 0;
}
