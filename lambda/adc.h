/*
 * adc.h
 *
 * Functions to set up the ADC and get voltages from its ports.
 *
 *  Created on: 11.03.2015
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

// TODO Put in makefile
// #define AREF_MV 4850
#define AREF_MV 5000
#define ADC_OFFSET_MV 7
// #define TEMPO_OP_OFFSET_MV 441
#define TEMPO_OP_OFFSET_MV 454

/**
 * Sets up reference voltage and clock prescaler of the ADC and enables it.
 */
void setupADC(void);

/**
 * Sets up sleep mode and enables ADC and global interrupts.
 */
void setupSleepMode(void);

/**
 * Returns the voltage sampled at the given ADC input port doing
 * 16x oversampling and taking in account the calibrated AREF and
 * ADC offset voltages.
 */
int16_t getVoltage(uint8_t port);
