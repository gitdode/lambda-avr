/*
 * adc.h
 *
 * Functions to set up the ADC and get voltages from its pins.
 *
 *  Created on: 11.03.2015
 *      Author: dode@luniks.net
 *
 * DISCLAIMER: I'm new to C.
 */

#ifndef ADC_H_
#define ADC_H_

// TODO put in Makefile
#define AREF_MV 5000
#define ADC_OFFSET_MV 7
#define TEMPO_OP_OFFSET_MV 454

/**
 * Sets up reference voltage and clock prescaler of the ADC and enables it.
 */
void setupADC(void);

/**
 * Returns the voltage sampled at the given ADC input pin doing
 * 16x oversampling and taking in account the calibrated AREF and
 * ADC offset voltages.
 */
uint16_t getVoltage(uint8_t pin);

#endif /* ADC_H_ */
