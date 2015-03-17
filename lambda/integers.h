/*
 * integers.h
 *
 * Functions to help with integers.
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
 * DISCLAIMER: I'm experienced with Java and I like C, but I'm new here.
 */

#ifndef INTEGERS_H_
#define INTEGERS_H_

/**
 * Divides the given numerator by the given denominator,
 * rounds to the nearest int and returns it.
 * http://stackoverflow.com/a/18067292/709426
 */
int32_t divRoundNearest(int32_t num, int32_t den);

/**
 * Divides the given numerator by the given denominator,
 * rounds up and returns it.
 */
int32_t divRoundUp(int32_t num, int32_t den);

// TODO function to divide and return result as decimal string?
// char* divToDecimalString(int32_t num, int32_t den);

#endif /* INTEGERS_H_ */
