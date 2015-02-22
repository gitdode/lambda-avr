/*
 * lambda.h
 *
 *  Created on: 22.02.2015
 *      Author: dode
 */
void run(void);

void update(float tempVoltage, float lambdaVoltage);

void display(int tempVoltage, int temp, float lambdaVoltage, float lambda);

int getVoltage(int port);

int toTemp(float mV);

float lookupLambdaInter(float mV);

const char* toInfo(float lambda);
