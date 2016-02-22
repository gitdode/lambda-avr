/*
 * airgate.c
 *
 *  Created on: 19.02.2016
 *      Author: dode@luniks.net
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "airgate.h"
#include "integers.h"

#include "usart.h"

// TODO pins

/* Direction */
static int8_t dir = 0;
/* Current position */
static uint16_t pos = 0;
/* Target position */
static int16_t target = 0;
/* Steps remaining */
static uint16_t steps = 0;
/* Steps done */
static uint16_t done = 0;
/* Acceleration profile ramp */
static uint16_t ramp = 0;
/* Speed */
static uint8_t speed = MIN_SPEED;

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
	done = 0;
	int16_t diff = (target << SCALE) - pos;
	if (diff != 0) {
		target = -1;
		dir = MAX(-1, MIN(diff, 1));
		steps = abs(diff);
		ramp = MIN(MIN_SPEED - MAX_SPEED, steps >> 1);
		speed = MIN_SPEED;
		start();
	}
}

void makeSteps(void) {
	if (steps > 0) {
		PORTB ^= (1 << PB7);
		pos += dir;
		steps--;
		done++;
		// accelerate within ramp
		if (done < ramp && speed > MAX_SPEED) speed--;
		// decelerate within ramp
		if (steps < ramp && speed < MIN_SPEED) speed++;
		OCR2A = speed;
	} else {
		done = 0;
		stop();
		if (target != -1) {
			// move to target position
			set();
		} else if (pos == 0) {
			// driver sleep mode
			// TODO when is it best to do this?
			PORTC &= ~(1 << PC5);
		}
	}
}

void setAirgate(uint8_t const position) {
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

uint8_t getAirgate(void) {
	return pos >> SCALE;
}
