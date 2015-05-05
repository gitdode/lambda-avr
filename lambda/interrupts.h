/*
 * interrupts.h
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

/**
 * Returns true if a CR or LF terminated line of data was received via USART.
 */
bool isUSARTReceived(void);

/**
 * Appends the data received via USART to the given string with the given
 * length.
 */
void getUSARTData(char* data, uint8_t length);

/**
 * Returns true if the current timer interrupt count is equal or greater to
 * the given count and resets it if the given boolean is true.
 */
bool hasIntCount(uint8_t count, bool reset);

/**
 * Sets up ports.
 */
void setupPorts(void);

/**
 * Sets up sleep mode.
 */
void setupSleepMode(void);

/**
 * Inits the interrupts.
 */
void initInterrupts(void);

/**
 * Inits the timers.
 */
void initTimers(void);

#endif /* INTERRUPTS_H_ */
