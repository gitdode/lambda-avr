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

#define AREF_MV 5000
#define ADC_NONLIN_0 -8
#define ADC_NONLIN_AREF 12

#if F_CPU == 1000000
	// ADC clock prescaler/8 = 125kHz @ 1MHz
	#define ADC_PRESCALE (1 << ADPS1) | (1 << ADPS0)
#elif F_CPU == 8000000
	// ADC clock prescaler/64 = 125kHz @ 8MHz
	#define ADC_PRESCALE (1 << ADPS2) | (1 << ADPS1)
#endif

/**
 * Sets up reference voltage and clock prescaler of the ADC and enables it.
 */
void setupADC(void);

/**
 * Returns the voltage sampled at the given ADC input pin doing
 * 16x oversampling and taking in account the calibrated AREF voltage.
 */
uint16_t getVoltage(uint8_t pin);

#endif /* ADC_H_ */
