/*
 * sensors.h
 *
 * Functions to evaluate measurements from the sensors.
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU Lesser General Public License Version 2.1 (the "LGPL License"), in which
 * case the provisions of the LGPL License are applicable instead of those
 * above.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU (Lesser) General Public License for more details.
 *
 * DISCLAIMER: I'm experienced with Java and I like C, but I'm new here.
 */

#ifndef SENSORS_H_
#define SENSORS_H_

// TODO put in Makefile?
#define LEAN "Mager"
#define IDEAL "Ideal";
#define RICH "Fett!";

/**
 * Entry for the lookup tables.
 */
typedef struct {
    const int16_t mV;
    const int16_t value;
} tableEntry;

/**
 * Measures the "input" and "output" temperatures and the lambda value
 * and displays the measured values.
 */
void measure(void);

/**
 * Formats the given values, displays them on an 16x2 LCD
 * and prints them over USART.
 */
void display(
		int16_t tempIVoltage, int16_t tempI,
		int16_t tempOVoltage, int16_t tempO,
		int16_t lambdaVoltage, int16_t lambda);

/**
 * Creates an exponential moving average of the given value and
 * average weighted by the given weight.
 */
int32_t average(int32_t value, int32_t average, uint8_t weight);

/**
 * Returns the temperature for the given voltage of a type K thermocouple
 * amplified with an AD8495 (5 mV/°C). Type K thermocouple voltages are
 * about linear between 0 and 800°C.
 * With the current circuit the measuring range is from about 3 to 990 °C
 * at 5000 mV OP supply and ADC reference voltage.
 */
int16_t toTempI(int16_t mV);

/**
 * Looks up the temperature in °C mapped to the given voltage measured at
 * the PT1000 using linear interpolation and returns it.
 * With the current circuit and the LM358 the measuring range is from about
 * -40 to 375°C at 5000 mV OP supply and ADV reference voltage.
 */
int16_t toTempO(int16_t mV);

/**
 * Looks up the lambda value x1000 mapped to the given voltage measured at
 * the LSM 11 oxygen sensor using linear interpolation and returns it.
 * With the current circuit and the AD8551 the measuring range is from about
 * lambda 1.0 to 2.0 at 5000 mV OP supply and ADC reference voltage.
 */
int16_t toLambda(int16_t mV);

/**
 * Returns the value corresponding to the given voltage
 * from the given lookup table using linear interpolation.
 * Thanks to http://stackoverflow.com/a/7091629/709426 and
 * http://en.wikipedia.org/wiki/Linear_interpolation
 */
int16_t lookupLinInter(int16_t mV, const tableEntry table[], uint8_t length);

/**
 * Returns a descriptive term such as "Lean" for the given lambda value x1000.
 * For a wood fire, residual oxygen between 5% and 7% (lambda 1.3 and 1.5) is
 * a good value, below is rich and above is lean.
 */
const char* toInfo(int16_t lambda);

#endif /* SENSORS_H_ */
