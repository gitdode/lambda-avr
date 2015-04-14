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
void cycle(void);

/**
 * Updates the measurements, tracks min and max values since last start/reset
 * and displays the current measurements unless the "menu" is active.
 */
void update(measurement);

/**
 * Formats the given values, displays them on an 16x2 LCD
 * and prints them over USART.
 */
void display(measurement);

#endif /* DISPLAY_H_ */
