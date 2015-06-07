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

#define SECOND 61

/**
 * Returns the time in units of about 16.4 ms since last reset.
 */
uint32_t getTime(void);

/**
 * Resets the time to 0.
 */
void resetTime(void);

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
