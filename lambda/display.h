/*
 * display.h
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/**
 * Cycles through the "menu" (display options).
 */
void cycleDisplay(void);

/**
 * Updates the measurements, tracks min and max values since last start/reset
 * and displays the selected measurement values.
 */
void updateMeas(measurement);

/**
 * Formats the given measurement values and prints them via USART.
 */
void printMeas(measurement);

/**
 * Formats the given measurement values and displays them on an 16x2 LCD.
 */
void displayMeas(measurement, char*);

/**
 * Displays the given two lines of text.
 */
void displayText(char*, char*);

#endif /* DISPLAY_H_ */
