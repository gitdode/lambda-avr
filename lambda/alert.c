/*
 * alert.c
 *
 *  Created on: 24.04.2015
 *      Author: dode@luniks.net
 */

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include "USART.h"
#include "alert.h"

uint8_t oscCount = 0;
uint8_t beepCount = 0;
uint8_t beepLength = 0;

void oscillate(void) {
	if (beepCount == 0) {
		return;
	}
	if (oscCount == 0) {
		// turn beep on
		DDRB |= (1 << PB1);
	}
	if (oscCount == beepLength) {
		// turn beep off
		DDRB &= ~(1 << PB1);
		beepCount--;
		if (beepCount == 0) {
			// clear alert
		}
	}
	oscCount == beepLength * 2 ? oscCount = 0 : oscCount++;
}

void beep(uint8_t beeps, uint8_t length) {
	oscCount = 0;
	beepCount = beeps;
	beepLength = length;
}

void alert(uint8_t beeps, uint8_t length, char* line0, char* line1) {
	oscCount = 0;
	beepCount = beeps;
	beepLength = length;
	// set alert
	// set display text
}
