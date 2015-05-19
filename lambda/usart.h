/*
 * usart.h
 *
 *  Created on: 19.05.2015
 *      Author: dode@luniks.net
 */

#ifndef USART_H_
#define USART_H_

#ifndef BAUD
#define BAUD 9600
#endif

#include <stdbool.h>
#include <stdio.h>

/**
 * Sets the baudrate and enables the transmitter and receiver.
 */
void initUSART(void);

/**
 * Returns true if a CR or LF terminated line of data was received via USART.
 */
bool isUSARTReceived(void);

/**
 * Appends the data received via USART to the given string with the given
 * length.
 */
void getUSARTData(char* data, size_t length);

/**
 * Prints the given string via USART.
 */
void printString(char* data);

#endif /* USART_H_ */
