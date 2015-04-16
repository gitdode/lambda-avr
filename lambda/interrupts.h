/*
 * interrupts.h
 *
 *  Created on: 10.04.2015
 *      Author: dode@luniks.net
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

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
