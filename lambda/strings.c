/*
 * strings.c
 *
 * Functions to help with strings.
 *
 *  Created on: 10.05.2015
 *      Author: dode@luniks.net
 */

#include <stdio.h>
#include <string.h>
#include "strings.h"

void split(char* const string, char* const delim,
		char* fields[], size_t const size) {
	size_t i = 0;
	char* token = strtok(string, delim);
	while (token != NULL && i < size) {
		fields[i++] = token;
		token = strtok(NULL, delim);
	}
	for (; i < size; i++) {
		fields[i] = '\0';
	}
}
