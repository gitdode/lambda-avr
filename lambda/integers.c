/*
 * integers.c
 *
 *  Created on: 07.03.2015
 *      Author: dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * GNU Lesser General Public License Version 2.1 (the "LGPL License"), in which
 * case the provisions of the LGPL License are applicable instead of those
 * above.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU (Lesser) General Public License for more details.
 *
 * DISCLAIMER: I'm experienced with Java, but new to C.
 *
 * ATTRIBUTION: This project includes the module USART and the Makefile from
 * the code accompanying the book Make: AVR Programming by Elliot Williams,
 * a great book and a pleasant read, that helped me tremendously to get
 * started with AVR programming.
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

