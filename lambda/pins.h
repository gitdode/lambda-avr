/*
 * pins.h
 *
 *  Created on: 06.05.2015
 *      Author: dode@luniks.net
 */

#ifndef PINS_H_
#define PINS_H_

/** ADC pin for the type K thermocouple signal */
#define ADC_TEMPI PC5
/** ADC pin for the PT1000 resistance thermometer signal */
#define ADC_TEMPO PC4	// prototype: PC0
/** ADC pin for the LSM 11 oxygen sensor signal */
#define ADC_LAMBDA PC3	// prototype: PC2

/* Pins for the LCD */
#define LCD_PORT PORTD
#define LCD_DDR	 DDRD
#define LCD_RS   PD7	// prototype: PD6
#define LCD_EN   PD6	// prototype: PD7
#define LCD_DB4  PD5
#define LCD_DB5  PD4	// prototype: PD2
#define LCD_DB6  PD3
#define LCD_DB7  PD2	// prototype: PD4

#endif /* PINS_H_ */
