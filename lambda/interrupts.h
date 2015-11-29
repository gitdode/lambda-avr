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

#if F_CPU == 1000000
	// timer0 clock prescaler/256 = 3.906 kHz @ 1 MHz
	#define TIMER0_PRESCALE (1 << CS02)
	// timer0 compare match about every 31.25 ms = 1000 ms >> 5
	#define TIMER0_COMP_MATCH 123
	// timer1 clock prescaler/8 = 125 kHz @ 1MHz
	#define TIMER1_PRESCALE (1 << CS11)
	// timer1 compare match at 7.8 kHz generating a 3.9 kHz beep
	// #define TIMER1_CTC_TOP 15
	// 2 kHz is less noisy on the small piezo beeper
	#define TIMER1_COMP_MATCH 31
#elif F_CPU == 8000000
	// timer0 clock prescaler/1024 = 7.812 kHz @ 8 MHz
	#define TIMER0_PRESCALE (1 << CS02) | (1 << CS00)
	// timer0 compare match about every 31.25 ms = 1000 ms >> 5
	#define TIMER0_COMP_MATCH 246
	// timer1 clock prescaler/64 = 125 kHz @ 8MHz
	#define TIMER1_PRESCALE (1 << CS11) | (1 << CS10)
	// timer1 compare match at 7.8 kHz generating a 3.9 kHz beep
	// #define TIMER1_CTC_TOP 15
	// 2 kHz is less noisy on the small piezo beeper
	#define TIMER1_COMP_MATCH 31
#endif

/**
 * Returns the time in about seconds since last reset.
 */
uint32_t getTime(void);

/**
 * Adds the given number of seconds. Useful only for simulation.
 */
void addTime(uint32_t time);

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
