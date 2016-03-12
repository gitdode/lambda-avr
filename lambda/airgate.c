/*
 * airgate.c
 *
 *  Created on: 19.02.2016
 *      Author: dode@luniks.net
 *
 *  Simple stepper motor control with a linear acceleration profile using the
 *  DRV8825. An absolute position from 0 to 255 can be set where 200 units
 *  correspond to 360° rotation.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "airgate.h"
#include "integers.h"
#include "interrupts.h"
#include "pins.h"

/* Direction */
static volatile int8_t dir = 0;
/* Current position */
static volatile uint16_t pos = 0;
/* Steps remaining */
static volatile uint16_t steps = 0;
/* Steps done */
static volatile uint16_t done = 0;
/* Acceleration profile ramp */
static volatile uint16_t ramp = 0;
/* Speed */
static volatile uint8_t speed = MIN_SPEED;

/**
 * Sets increased current for higher torque,sets the direction and initial
 * speed and starts the motor by starting the timer.
 */
static void start(void) {
	// set increased current for higher torque
	PORT &= ~(1 << PIN_CURRENT);
	// some time for the power supply to stabilize?
	_delay_ms(2);
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
		OCR2A = ((uint16_t)MIN_SPEED * MAX_SPEED) / speed;
	} else {
		stop();
		pos += (done * dir);
		done = 0;
		// set reduced current to save power
		PORT |= (1 << PIN_CURRENT);
	}
}

void setAirgate(uint8_t const target) {
	if (target == getAirgate() || isAirgateBusy()) {
		return;
	}
	if (bit_is_clear(PORT, PIN_SLEEP)) {
		setSleepMode(false);
	}
	int16_t diff = (((int16_t)target) << SCALE) - pos;
	if (diff != 0) {
		dir = (diff > 0) - (diff < 0);
		steps = abs(diff);
		ramp = MIN(abs(MAX_SPEED - MIN_SPEED), steps >> 1);
		start();
	}
}

uint8_t getAirgate(void) {
	return pos >> SCALE;
}

uint8_t getAirgateInPercent(void) {
	if (getAirgate() == 0) {
		return 0;
	}

	return 100 / (AIRGATE_OPEN / getAirgate());
}

bool isAirgateBusy(void) {
	uint16_t steps_c;
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		steps_c = steps;
	}
	return steps_c > 0;
}

void setSleepMode(bool const on) {
	if (on) {
		PORT &= ~(1 << PIN_SLEEP);
	} else {
		// wake up driver
		PORT |= (1 << PIN_SLEEP);
		// wakeup time
		// should not be woken up just before stepping anyway, the power supply
		// might need much more time to stabilize
		_delay_ms(2);
	}
}
