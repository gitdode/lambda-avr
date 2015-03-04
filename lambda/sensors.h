/*
 * sensors.h
 *
 *  Created on: 02.03.2015
 *      Author: dode@luniks.net
 */
typedef struct {
    const int mV;
    const float value;
} tableEntry;

int getVoltage(int port);

float toLambda(float mV);

/**
 * Returns the temperature for the given voltage of a type K thermocouple
 * amplified with an AD8495 (5 mV/°C). Type K thermocouple voltages are
 * about linear between 0 and 800°C.
 */
int toTempI(float mV);

int toTempO(float mV);

/**
 * Returns the value corresponding to the given voltage
 * from the lookup table using linear interpolation.
 * Thanks to http://stackoverflow.com/a/7091629/709426 and
 * http://en.wikipedia.org/wiki/Linear_interpolation
 */
float lookupLinInter(float mV, const tableEntry table[], int length);

const char* toInfo(float lambda);
