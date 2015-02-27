/*
 * lambda.h
 *
 *  Created on: 22.02.2015
 *      Author: dode
 */
void run(void);

void update(float tempIVoltage, float tempOVoltage, float lambdaVoltage);

void display(int tempIVoltage, int tempI, int tempOVoltage, int tempO, float lambdaVoltage, float lambda);

int getVoltage(int port);

int toTempI(float mV);

int toTempO(float mV);

float lookupLambdaInter(float mV);

const char* toInfo(float lambda);
