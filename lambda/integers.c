/*
 * integers.c
 *
 *  Created on: 07.03.2015
 *      Author: dode@luniks.net
 */
#include <avr/io.h>
#include "integers.h"

int32_t roundNearest(int32_t num, int32_t den) {
	return ((num < 0) ^ (den < 0)) ?
			((num - den / 2) / den) :
			((num + den / 2) / den);
}

