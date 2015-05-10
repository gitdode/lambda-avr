/*
 * strings.c
 *
 *  Created on: 10.05.2015
 *      Author: dode@luniks.net
 */

#include <stdio.h>
#include <string.h>

void split(char* string, char* delim, char* fields[], uint8_t size) {
	uint8_t index = 0;
	char* token = strtok(string, delim);
	while (token != NULL && index < size) {
		fields[index++] = token;
		token = strtok(NULL, delim);
	}
	for (; index < size; index++) {
		fields[index] = '\0';
	}
}
