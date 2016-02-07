/*
 * strings.h
 *
 * Functions to help with strings.
 *
 *  Created on: 10.05.2015
 *      Author: dode@luniks.net
 */

#ifndef STRINGS_H_
#define STRINGS_H_

/**
 * Splits the given string at the given delimiter and puts the split strings in
 * the given array with the given size.
 */
void split(char* string, char* delim, char* fields[], size_t size);

#endif /* STRINGS_H_ */
