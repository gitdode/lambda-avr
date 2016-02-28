/*
 * display.h
 *
 * Functions to format, display and log measurements.
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 *
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "sensors.h"

/**
 * Display positions.
 */
typedef enum {
	displayPosCurrent = 0,
	displayPosMax = 1,
	displayPosAirgate = 2,
	displayPosHeater = 3,
	displayPosLastText = 4,
	displayPosTime = 5
} DisplayPos;

uint8_t getPosition(void);

/**
 * Cycles through the "menu" (display options) and flags pending display update.
 */
void cycleDisplay(void);

/**
 * Updates the measurements, tracks max values since last start/reset
 * and flags pending display update.
 */
void updateMeas(Measurement);

/**
 * Resets max measurements to initial values and flags pending display update.
 */
void resetDisplay(void);

/**
 * Updates the display if an update is pending.
 */
void updateDisplayIfPending(void);

/**
 * Formats the given measurement values and prints them via USART.
 */
void logMeas(Measurement meas);

/**
 * Displays the given two lines of text.
 */
void displayText(const char* line0, const char* line1);

/**
 * Causes the display to be updated next time
 * updateDisplayIfPending() is called.
 */
void setUpdatePending(void);

#endif /* DISPLAY_H_ */
