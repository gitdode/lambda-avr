/*
 * usart.c
 *
 *  Created on: 19.05.2015
 *      Author: dode@luniks.net
 */

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include <util/setbaud.h>

static volatile bool usartReceived = false;

static char usartData[64];

/**
 * Called when data was received via USART.
 */
ISR(USART_RX_vect) {
	if (bit_is_set(UCSR0A, RXC0) && ! usartReceived) {
		char data = UDR0;
		size_t length = strlen(usartData);
		if (length < sizeof(usartData) - 1 && data != '\n' && data != '\r') {
			usartData[length] = data;
		} else {
			usartData[length] = '\0';
			usartReceived = true;
		}
	}
}

void initUSART(void) {
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

bool isUSARTReceived(void) {
	return usartReceived;
}

void getUSARTData(char* const data, size_t const size) {
	if (size > 0) {
	    data[0] = '\0';
	    strncat(data, usartData, size - 1);
	    memset(usartData, 0, sizeof(usartData));
	    usartReceived = false;
	}
}

void printString(char* const data) {
	uint8_t i = 0;
	char c;
	while ((c = data[i++]) != '\0') {
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = c;
	}
}
