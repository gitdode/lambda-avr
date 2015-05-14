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

void oscillateBeep(void);

/**
 * Beeps the given number of beeps with the given length. Returns quickly so
 * it can be called from an ISR.
 */
void beep(uint8_t beeps, uint8_t length);

/**
 * Beeps the given number of beeps with the given length and displays the
 * given two texts on the first and second line of the display, respectively.
 */
void alert(uint8_t beeps, uint8_t length, char* line0, char* line1);

/**
 * Stops beeping and blocking display updates.
 */
void cancelAlert(void);

/**
 * Returns true if an alert is active, false otherwise.
 */
bool isAlertActive(void);

#endif /* ALERT_H_ */
