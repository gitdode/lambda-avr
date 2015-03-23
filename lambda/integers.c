/*
 * integers.c
 *
 * Functions to help with integers.
 *
 *  Created on: 07.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 */

#include <avr/io.h>
#include "integers.h"

int32_t divRoundNearest(int32_t num, int32_t den) {
	return ((num < 0) ^ (den < 0)) ?
			((num - den / 2) / den) :
			((num + den / 2) / den);
}

int32_t divRoundUp(int32_t num, int32_t den) {
	return ((num < 0) ^ (den < 0)) ?
			((num - ((den < 0) ? den + 1 : den - 1)) / den) :
			((num + ((den < 0) ? den + 1 : den - 1)) / den);
}

