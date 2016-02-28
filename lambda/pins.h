/*
 * pins.h
 *
 * Assignment of ADC and LCD pins.
 *
 *  Created on: 06.05.2015
 *      Author: dode@luniks.net
 */

#include <avr/io.h>

#ifndef PINS_H_
#define PINS_H_

/** ADC pin for the type K thermocouple signal */
#define ADC_TEMPI PC2
/** ADC pin for the PT1000 resistance thermometer signal */
#define ADC_TEMPO PC0
/** ADC pin for the LSM 11 oxygen sensor signal */
#define ADC_LAMBDA PC1
/** ADC pin for the LSM 11 oxygen sensor heater current */
#define ADC_HEATER PC3

/** Pin input register for menu button */
#define PIN PINB
/** Pin output register for menu button, beeper and oxygen sensor heater */
#define PORT PORTB
/** DDR for menu button, beeper and oxygen sensor heater */
#define DDR DDRB
/** Pin for the menu button */
#define PIN_BUTTON PB0
/** Pin for the beeper */
#define PIN_BEEPER PB1
/** Toggle beeper pin OC1A/PB1 */
#define PIN_BEEPER_TOGGLE COM1A0
/** Pin for the oxygen sensor heater */
#define PIN_HEATER PB2
/** Pin for the stepper motor driver sleep mode **/
#define PIN_SLEEP PB3
/** Pin for the stepper motor driver step input */
#define PIN_STEP PB4
/** Pin for the stepper motor driver direction input */
#define PIN_DIR PB5

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
