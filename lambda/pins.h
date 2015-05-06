/*
 * pins.h
 *
 *  Created on: 06.05.2015
 *      Author: dode@luniks.net
 */

#ifndef PINS_H_
#define PINS_H_

/** ADC pin for the type K thermocouple signal */
#define ADC_TEMPI PC5  	// PC5 on diagram
/** ADC pin for the PT1000 resistance thermometer signal */
#define ADC_TEMPO PC0  	// PC4 on diagram
/** ADC pin for the LSM 11 oxygen sensor signal */
#define ADC_LAMBDA PC2 	// PC3 on diagram

/* Pins for the LCD */
#define LCD_PORT PORTD
#define LCD_DDR	 DDRD
#define LCD_RS   PD6 	// PD7 on diagram
#define LCD_EN   PD7 	// PD6 on diagram
#define LCD_DB4  PD5 	// PD5 on diagram
#define LCD_DB5  PD2 	// PD4 on diagram
#define LCD_DB6  PD3 	// PD3 on diagram
#define LCD_DB7  PD4 	// PD2 on diagram

#endif /* PINS_H_ */
