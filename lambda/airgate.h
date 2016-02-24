/*
 * airgate.h
 *
 *  Created on: 19.02.2016
 *      Author: dode@luniks.net
 */

#ifndef AIRGATE_H_
#define AIRGATE_H_

#define MIN_SPEED 150
#define MAX_SPEED 20
/**
 * 1 = full step, 2 = half step, 3 = 1/4 step, 4 = 8 microsteps, ...
 * for 180°.
 */
#define SCALE 3

/**
 * Called from the timer interrupt ISR and makes one step or stops the
 * timer/motor if the target position is reached.
 */
void makeSteps(void);

/**
 * Sets the airgate position 0 - 100%. The actual number of degrees the motor
 * spins depends on the SCALE and the stepping mode. If the motor is currently
 * moving to a target position when this function is called, it is first
 * decelerated and then moves to the new target position.
 */
void setAirgate(uint8_t const position);

/**
 * Returns the current airgate position, assuming the motor did all the steps
 * it was requested to do.
 */
uint8_t getAirgate(void);

/**
 * Sets the driver in sleep mode.
 */
void setSleepMode(void);

#endif /* AIRGATE_H_ */
