/*
 * alert.h
 *
 *  Created on: 24.04.2015
 *      Author: dode@luniks.net
 */

#ifndef ALERT_H_
#define ALERT_H_

void oscillateBeep(void);

void beep(uint8_t beeps, uint8_t length);

void alert(uint8_t beeps, uint8_t length, char* line0, char* line1);

void cancelAlert(void);

bool isAlertActive(void);

#endif /* ALERT_H_ */
