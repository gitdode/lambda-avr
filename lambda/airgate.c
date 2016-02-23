/*
 * airgate.c
 *
 *  Created on: 19.02.2016
 *      Author: dode@luniks.net
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "airgate.h"
#include "integers.h"

// TODO pins

/* Direction */
volatile static int8_t dir = 0;
/* Current position */
volatile static uint16_t pos = 0;
/* Target position */
volatile static int16_t target = 0;
/* Steps remaining */
volatile static uint16_t steps = 0;
/* Steps done */
volatile static uint16_t done = 0;
/* Acceleration profile ramp */
volatile static uint16_t ramp = 0;
/* Speed */
volatile static uint8_t speed = MIN_SPEED;

/**
 * Wakes up the driver if sleeping, sets the direction and initial speed and
 * starts the motor by starting the timer.
 */
static void start(void) {
	if (bit_is_clear(PORTC, PC5)) {
		// wake up driver
		PORTC |= (1 << PC5);
		// wakeup time
		_delay_ms(2);
	}
	// set dir
	if (dir == 1) {
		PORTB &= ~(1 << PB6);
	} else {
		PORTB |= (1 << PB6);
	}
	// setup time
	_delay_us(1);
	// set start speed
	OCR2A = speed;
	// start timer2
	TCCR2B |= (1 << CS22) | (1 << CS21);
}

/**
 * Stops the motor by stopping the timer.
 */
static void stop(void) {
	// stop timer2
	TCCR2B = 0;
	// GTCCR |= (1 << PSRASY);
}

/**
 * Calculates the direction and steps to take to get to the target position,
 * the ramp length for the acceleration profile, sets the speed and starts
 * the motor.
 */
static void set(void) {
	int16_t diff = (target << SCALE) - pos;
	if (diff != 0) {
		dir = MAX(-1, MIN(diff, 1));
		speed = MIN_SPEED;
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			steps = abs(diff);
			ramp = MIN(MIN_SPEED - MAX_SPEED, steps >> 1);
		}
		start();
	}
}

void makeSteps(void) {
	if (steps > 0) {
		PORTB ^= (1 << PB7);
		pos += dir;
		done++;
		steps--;
		// accelerate within ramp
		if (done < ramp && speed > MAX_SPEED) speed--;
		// decelerate within ramp
		if (steps < ramp && speed < MIN_SPEED) speed++;
		OCR2A = speed;
	} else {
		done = 0;
		stop();
		// move to new target position if necessary
		set();
		if (pos == 0) {
			// driver sleep mode
			// TODO when is it best to do this?
			PORTC &= ~(1 << PC5);
		}
	}
}

void setAirgate(uint8_t const position) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		if (position == target) {
			return;
		}
		target = position;
		if (steps > 0) {
			// motor busy - decelerate and move to target position when stopped
			steps = MIN(ramp, steps);
		} else {
			// move to target position
			set();
		}
	}
}

uint8_t getAirgate(void) {
	return target;
}
