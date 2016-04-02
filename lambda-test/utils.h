/*
 * utils.h
 *
 *  Created on: 28.03.2016
 *      Author: dode@luniks.net
 *
 * Some utility functions used by many tests.
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>

/**
 * Trigger steps as done by the timer ISR until the requested motor position
 * is reached.
 */
void stepUntilDone(void);

/**
 * Simulates fault or normal operation of the stepper motor driver.
 */
void setDriverFault(bool const fault);

#endif /* UTILS_H_ */
