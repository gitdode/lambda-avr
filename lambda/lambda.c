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
#include <util/delay.h>
#include <avr/io.h>
#include "USART.h"
#include "adc.h"
#include "sensors.h"
#include "integers.h"
#include "lcdroutines.h"

/**
 * Initializes the USART transmitter and receiver, the lcd, sets up the ADC
 * and sleep mode and then infinitely measures with a 1 second delay
 * in between.
 * TODO replace delay by a timer?
 */
int main(void) {
	initUSART();
	lcd_init();
	setupADC();
	setupSleepMode();

	// main loop
	while (1) {
		measurement meas = measure();
		display(meas);
		_delay_ms(1000);
	}

	// never reached
	return 0;
}
