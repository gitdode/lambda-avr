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

// 61 +/-1 depending on internal oscillator?
#define INTS_PER_SEC 61

/**
 * Returns the count of interrupts occurring about every 16.4 ms
 * since last reset.
 */
uint32_t getInts(void);

/**
 * Returns the time in about seconds since last reset.
 */
uint32_t getTime(void);

/**
 * Writes the time since last reset in format HHHHH:MM:SS to the given string.
 */
void formatTime(char* str, size_t size);

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
