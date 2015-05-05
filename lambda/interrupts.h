/*
 * interrupts.h
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

bool isButtonPressed(void);

bool isUSARTReceived(void);

void getUSARTData(char*, uint8_t);

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
