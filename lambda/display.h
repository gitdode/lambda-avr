/*
 * display.h
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/**
 * Cycles through the "menu" (display options). Returns quickly so it can
 * be called from an ISR.
 */
void cycleDisplay(void);

/**
 * Updates the measurements, tracks min and max values since last start/reset
 * and updates the display.
 */
void updateMeas(measurement);

/**
 * Updates the display with the selected measurement values.
 */
void updateDisplay(void);

/**
 * Updates the display if an update is pending.
 */
void updateDisplayIfRequested(void);

/**
 * Formats the given measurement values and prints them via USART.
 */
void printMeas(measurement meas);

/**
 * Formats the given measurement values and displays them on an 16x2 LCD along
 * with the given hint.
 */
void displayMeas(measurement meas, char* hint);

/**
 * Displays the given two lines of text.
 */
void displayText(char* line0, char* line1);

#endif /* DISPLAY_H_ */
