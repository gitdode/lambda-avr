/*
 * sensors.h
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 */

// TODO is this the right place for these definitions? Put in makefile?
// #define AREF_MV 4850
#define AREF_MV 5000
#define ADC_OFFSET_MV 7
// #define TMP_OP_OFFSET_MV 441
#define TMP_OP_OFFSET_MV 454

#define LEAN "Mager"
#define IDEAL "Ideal";
#define RICH "Fett!";

/**
 * Struct used as entries for the lookup tables.
 */
typedef struct {
    const int16_t mV;
    const int16_t value;
} tableEntry;

/**
 * Returns the voltage sampled at the given ADC input port doing
 * 16x oversampling and taking in account the calibrated AREF and
 * ADC offset voltages.
 */
int16_t getVoltage(uint8_t port);

/**
 * Returns the temperature for the given voltage of a type K thermocouple
 * amplified with an AD8495 (5 mV/°C). Type K thermocouple voltages are
 * about linear between 0 and 800°C.
 */
int16_t toTempI(int16_t mV);

/**
 * Looks up the temperature in °C mapped to the given voltage measured at
 * the PT1000 using linear interpolation and returns it.
 */
int16_t toTempO(int16_t mV);

/**
 * Looks up the lambda value x1000 mapped to the given voltage measured at
 * the oxygen sensor using linear interpolation and returns it.
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
 */
const char* toInfo(int16_t lambda);
