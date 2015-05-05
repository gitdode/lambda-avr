/*
 * command.h
 *
 *  Created on: 02.05.2015
 *      Author: dode@luniks.net
 */

#ifndef COMMAND_H_
#define COMMAND_H_

/**
 * Returns true if simulation mode is enabled, false otherwise.
 */
bool isSimulation(void);

/**
 * Returns true if logging is enabled, false otherwise.
 */
bool isLogging(void);

/**
 * Evaluates and runs the given command and/or data.
 */
void runCommand(char* data);

#endif /* COMMAND_H_ */
