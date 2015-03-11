/*
 * adc.h
 *
 *  Created on: 11.03.2015
 *      Author: dode@luniks.net
 */

/**
 * Sets up reference voltage and clock prescaler of the ADC and enables it.
 */
void setupADC(void);

/**
 * Sets up sleep mode and enables ADC and global interrupts.
 */
void setupSleepMode(void);
