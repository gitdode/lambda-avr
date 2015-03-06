/*
 * sensors.h
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 */
typedef struct {
    const int16_t mV;
    const int16_t value;
} tableEntry;

int16_t getVoltage(uint8_t port);

int16_t toLambda(int16_t mV);

/**
 * Returns the temperature for the given voltage of a type K thermocouple
 * amplified with an AD8495 (5 mV/°C). Type K thermocouple voltages are
 * about linear between 0 and 800°C.
 */
int16_t toTempI(int16_t mV);

int16_t toTempO(int16_t mV);

/**
 * Returns the value corresponding to the given voltage
 * from the lookup table using linear interpolation.
 * Thanks to http://stackoverflow.com/a/7091629/709426 and
 * http://en.wikipedia.org/wiki/Linear_interpolation
 */
int16_t lookupLinInter(int16_t mV, const tableEntry table[], uint8_t length);

const char* toInfo(int16_t lambda);
