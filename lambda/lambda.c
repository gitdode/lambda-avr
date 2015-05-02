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
#include "alert.h"
#include "command.h"

volatile bool buttonPressed = false;
volatile bool usartReceived = false;
volatile uint8_t intCount = 0;
volatile uint8_t dataCount = 0;

char usartData[64];

/**
 * Called every 16 ms.
 */
ISR(TIMER0_OVF_vect) {
	intCount++;
	oscillateBeep();
	if (bit_is_clear(PINB, PB0) && ! buttonPressed) {
		buttonPressed = true;
		cycleDisplay();
	} else if (bit_is_set(PINB, PB0)) {
		buttonPressed = false;
	}
}

ISR(USART_RX_vect) {
	if (bit_is_set(UCSR0A, RXC0) && ! usartReceived) {
		char data = UDR0;
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = data;
		if (dataCount < sizeof(usartData) - 1 && data != '\n' && data != '\r') {
			usartData[dataCount++] = data;
		} else {
			usartData[dataCount++] = 0;
			usartReceived = true;
		}
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

	alert(1, 2, "     Hello!     ", "");

	measurement meas;
	// memset(&meas, 0, sizeof(meas));

	// main loop
	while (1) {
		if (intCount >= 62 && ! isSimulation()) {
			intCount = 0;
			// causes a click in the beep, because of sleep mode?
			meas = measure();
			printMeas(meas);
			updateMeas(meas);
		}
		if (buttonPressed) {
			// update display immediately
			updateMeas(meas);
		}
		if (usartReceived) {
			command(usartData);
			memset(usartData, 0, sizeof(usartData));
			dataCount = 0;
			usartReceived = false;
		}
	}

	// never reached
	return 0;
}
