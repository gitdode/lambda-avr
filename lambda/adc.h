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

/**
 * Returns the voltage sampled at the given ADC input port doing
 * 16x oversampling and taking in account the calibrated AREF and
 * ADC offset voltages.
 */
int16_t getVoltage(uint8_t port);
