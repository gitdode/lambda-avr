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

/**
 * Trigger steps as done by the timer ISR until the requested motor position
 * is reached.
 */
void stepUntilDone(void);

#endif /* UTILS_H_ */
