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

#define MIN_SPEED 150
#define MAX_SPEED 20
// 1 = full step, 2 = half step, 3 = 1/4 step, 4 = 8 microsteps, ...
#define SCALE 3

// TODO pins

static int8_t dir = 0;
static uint16_t pos = 0;
static uint16_t steps = 0;
static uint16_t done = 0;
static uint16_t ramp = 0;
static uint8_t speed = MIN_SPEED;

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

static void stop(void) {
	// stop timer2
	TCCR2B = 0;
	// GTCCR |= (1 << PSRASY);
}

void makeSteps(void) {
	if (steps > 0) {
		PORTB ^= (1 << PB7);
		pos += dir;
		steps--;
		done++;
		if (done < ramp && speed > MAX_SPEED) --speed;
		if (steps < ramp && speed < MIN_SPEED) ++speed;
		OCR2A = speed;
	} else {
		stop();
		done = 0;
		if (pos == 0) {
			// driver sleep mode
			PORTC &= ~(1 << PC5);
		}
	}
}

void setAirgate(uint8_t const position) {
	stop();
	done = 0;
	int16_t diff = (((int16_t)position) << SCALE) - pos;
	if (diff != 0) {
		dir = MAX(-1, MIN(diff, 1));
		steps = abs(diff);
		speed = MIN_SPEED;
		ramp = MIN(MIN_SPEED - MAX_SPEED, steps >> 1);
		start();
	}
}

uint8_t getAirgate(void) {
	return 100; //pos >> SCALE;
}
