/*
 * airgate.h
 *
 *  Created on: 19.02.2016
 *      Author: dode@luniks.net
 */

#ifndef AIRGATE_H_
#define AIRGATE_H_

/** Min value 1 */
#define MIN_SPEED 200
/** Max value 255 for 8 bit timer and 65535 for 16 bit timer */
#define MAX_SPEED 250

#define AIRGATE_CLOSE  	0
#define AIRGATE_25	 	15
#define AIRGATE_50 		30
#define AIRGATE_OPEN 	60

/**
 * 1 = full step, 2 = half step, 3 = 1/4 step, 4 = 8 microsteps, ...
 * for 180°.
 */
#define SCALE 3

/**
 * Called from the timer interrupt ISR and makes a half step or stops the
 * timer/motor if the target position is reached.
 */
void makeSteps(void);

/**
 * Sets the airgate position 0 - 255, where 200 units correspond to 360°
 * rotation.
 */
void setAirgate(uint8_t const position);

/**
 * Returns the current airgate position, assuming the motor did all the steps
 * it was requested to do.
 */
uint8_t getAirgate(void);

/**
 * Returns the current airgate position translated to percent.
 */
uint8_t getAirgateInPercent(void);

/**
 * Returns true if the motor is currently busy setting an airgate position,
 * false otherwise.
 */
bool isAirgateBusy(void);

/**
 * Wakes up the driver or puts it in sleep mode.
 */
void setSleepMode(bool const on);

/**
 * Resets to initial state and the given position. For unit tests.
 */
void resetAirgate(uint16_t const position);

#endif /* AIRGATE_H_ */
