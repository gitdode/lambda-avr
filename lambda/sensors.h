/*
 * sensors.h
 *
 * Functions to evaluate measurements from the sensors.
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 *
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stdio.h>

#define SHUNT_MILLIOHMS 100

/**
 * Oxygen sensor heater current limits at certain states in milliamps.
 */
typedef enum {
	milliAmpsReady = 1350,
	milliAmpsShort = 7500,
	milliAmpsDisconn = 100
} HeaterMilliAmps;

/**
 * Oxygen sensor heater states.
 */
typedef enum {
	heaterStateFault = -1,
	heaterStateOff = 0,
	heaterStateOn = 1,
	heaterStateReady = 2
} HeaterState;

/**
 * Entry for the lookup tables.
 */
typedef struct {
    const uint16_t mV;
    const int16_t value;
} TableEntry;

/**
 * Measured values.
 */
typedef struct {
	int16_t tempI;
	int16_t tempO;
	uint16_t lambda;
	uint16_t current;
} Measurement;

/**
 * Measures the "input" and "output" temperatures and the lambda value
 * and returns them.
 */
Measurement measure(void);

/**
 * Reads measurement data from the given array of strings, used to simulate
 * a burnoff with data logged during real burnoffs. The voltages are not used,
 * the space separated fields are tempI, tempO and lambda.
 */
Measurement readMeas(char* const usartData[], size_t size);

/**
 * Returns the temperature for the given voltage of a type K thermocouple
 * amplified with an AD8495 (5 mV/°C). Type K thermocouple voltages are
 * about linear between 0 and 800°C.
 * With the current circuit the measuring range is from about 3 to 990 °C
 * at 5000 mV OP supply and ADC reference voltage.
 */
int16_t toTempI(uint16_t mV);

/**
 * Looks up the temperature in °C mapped to the given voltage measured at
 * the PT1000 using linear interpolation and returns it.
 * With the current circuit and the LM358 the measuring range is from about
 * -40 to 375°C at 5000 mV OP supply and ADV reference voltage.
 */
int16_t toTempO(uint16_t mV);

/**
 * Looks up the lambda value x1000 mapped to the given voltage measured at
 * the LSM 11 oxygen sensor using linear interpolation and returns it.
 * With the current circuit and the AD8551 the measuring range is from about
 * lambda 1.0 to 2.0 at 5000 mV OP supply and ADC reference voltage.
 */
uint16_t toLambda(uint16_t mV);

/**
 * Returns the current of the oxygen sensor heater in mA.
 */
uint16_t toCurrent(uint16_t mv);

/**
 * Returns the value corresponding to the given voltage
 * from the given lookup table using linear interpolation.
 * Thanks to http://stackoverflow.com/a/7091629/709426 and
 * http://en.wikipedia.org/wiki/Linear_interpolation
 */
int16_t lookupLinInter(uint16_t mV, TableEntry const table[], size_t length);

/**
 * Returns the given ADC measurement with compensated ADC non-linearity.
 */
int32_t linADC(uint16_t mV);

/**
 * Returns a descriptive term such as "Lean" for the given lambda value x1000.
 * For a wood fire, residual oxygen between 5% and 7% (lambda 1.3 and 1.5) is
 * a good value, below is rich and above is lean.
 */
char* toInfo(uint16_t lambda);

/**
 * Returns true if the heater of the oxygen sensor is turned on,
 * false otherwise.
 */
bool isHeaterOn(void);

/**
 * Sets the state of the heater of the oxygen sensor to the given value.
 */
void setHeaterState(int8_t state);

/**
 * Returns the state of the heater of the oxygen sensor.
 */
int8_t getHeaterState(void);

/**
 * Returns the time in seconds passed since that the heater was switched on.
 */
uint32_t getHeaterUptime(void);

#endif /* SENSORS_H_ */
