/*
 * integers.h
 *
 *  Created on: 07.03.2015
 *      Author: dode@luniks.net
 */

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
// char* toDecimalString(int32_t num, int32_t den);
