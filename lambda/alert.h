/*
 * alert.h
 *
 * Functions to beep and show an alert on the display.
 *
 *  Created on: 24.04.2015
 *      Author: dode@luniks.net
 *
 */

#ifndef ALERT_H_
#define ALERT_H_

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdio.h>

void oscillateBeep(void);

/**
 * Beeps the given number of beeps with the given length and tone.
 * Returns quickly so it can be called from an ISR.
 */
void beep(uint8_t beeps, uint8_t length, uint16_t tone);

/**
 * Beeps the given number of beeps with the given length and tone and displays
 * the given two texts on the first and second line of the display,
 * respectively, and if keep is true, keeps the alert active until the next
 * alert, when the button is pressed, only the beep stops.
 */
void alert(uint8_t beeps, uint8_t length, uint16_t tone,
		const char* line0, const char* line1, bool keep);

/**
 * Like alert(), but line0_P and line1_P are expected to be static pointers to
 * strings stored in program space like so: PSTR("string").
 */
void alert_P(uint8_t beeps, uint8_t length, uint16_t tone,
		PGM_P line0_P, PGM_P line1_P, bool keep);

/**
 * Stops beeping and blocking display updates.
 */
void cancelAlert(void);

/**
 * Returns true if an alert is active, false otherwise.
 */
bool isAlertActive(void);

#endif /* ALERT_H_ */
