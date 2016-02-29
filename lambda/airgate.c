/*
 * airgate.c
 *
 *  Created on: 19.02.2016
 *      Author: dode@luniks.net
 *
 *  Simple (maybe naive) stepper motor control with a linear acceleration
 *  profile using the DRV8825. An absolute position from 0 to 255 can be set
 *  which relates to the actual number of degrees by the SCALE constant and the
 *  stepping mode. If a new position is set while the motor is busy, it is
 *  decelerated before it starts to move to the new position.
 *  The idea is to be able to set the airgate position from 0 - 100%.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "airgate.h"
#include "integers.h"
#include "interrupts.h"
#include "pins.h"

/* Direction */
volatile static int8_t dir = 0;
/* Target position */
volatile static uint8_t target = 0;
/* Current position */
volatile static uint16_t pos = 0;
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
	// set rated current for max torque
	PORT &= ~(1 << PIN_CURRENT);
	// some time to stabilize?
	_delay_us(3);
	// set dir
	if (dir == 1) {
		PORT &= ~(1 << PIN_DIR);
	} else {
		PORT |= (1 << PIN_DIR);
	}
	// setup time
	_delay_us(3);
	// set start speed
	OCR2A = MIN_SPEED;
	// start timer2
	TCCR2B |= TIMER2_PRESCALE;
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
 * the ramp length for the acceleration profile and starts the motor.
 */
static void set(void) {
	int16_t diff = (((int16_t)target) << SCALE) - pos;
	if (diff != 0) {
		dir = (diff > 0) - (diff < 0);
		steps = abs(diff);
		ramp = MIN(abs(MAX_SPEED - MIN_SPEED), steps >> 1);
		start();
	} else {
		// set reduced current to save power
		PORT |= (1 << PIN_CURRENT);
	}
}

void makeSteps(void) {
	if (steps > 0) {
		PORT ^= (1 << PIN_STEP);
		done++;
		steps--;
		if (done < ramp && speed < MAX_SPEED) {
			// accelerate within ramp
			speed++;
		} else if (steps < ramp && speed > MIN_SPEED) {
			// decelerate within ramp
			speed--;
		}
		// linearize an unfavourably increasing acceleration curve
		OCR2A = (MIN_SPEED * MAX_SPEED) / speed;
	} else {
		stop();
		pos += (done * dir);
		done = 0;
		// move to new target position if necessary
		set();
	}
}

void setAirgate(uint8_t const position) {
	if (position == target) {
		return;
	}
	target = position;
	bool busy = false;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		busy = steps > 0;
		if (busy) {
			// motor busy - decelerate and move to target position when stopped
			steps = MIN(ramp, steps);
		}
	}
	if (! busy) {
		// move to target position
		set();
	}
}

uint8_t getAirgate(void) {
	return target;
}

void setSleepMode(bool const on) {
	if (on) {
		// wake up driver
		PORT |= (1 << PIN_SLEEP);
		// wakeup time
		// should not be woken up just before stepping anyway, the power supply
		// might need much more time to stabilize
		// _delay_ms(2);
	} else {
		PORT &= ~(1 << PIN_SLEEP);
	}
}
