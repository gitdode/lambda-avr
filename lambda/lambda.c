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
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "lcdroutines.h"
#include "adc.h"
#include "interrupts.h"
#include "sensors.h"
#include "integers.h"
#include "display.h"

volatile bool buttonPressed = false;
volatile uint8_t intCount = 0;

/**
 * Called every 16.32 ms.
 */
ISR(TIMER0_OVF_vect) {
	intCount++;
	if (bit_is_clear(PINB, PB0) && ! buttonPressed) {
		// PORTB ^= (1 << PB1);
		cycle();
		buttonPressed = true;
	} else if (bit_is_set(PINB, PB0)) {
		buttonPressed = false;
	}
}

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

	// main loop
	while (1) {
		measurement meas;
		if (intCount >= 60) {
			intCount = 0;
			meas = measure();
			update(meas);
			print(meas);
		}
		if (buttonPressed) {
			// update display immediately
			update(meas);
		}
	}

	// never reached
	return 0;
}
