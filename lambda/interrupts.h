/*
 * interrupts.h
 *
 * ISRs and functions to handle and set up interrupts and timers.
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 *
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

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
