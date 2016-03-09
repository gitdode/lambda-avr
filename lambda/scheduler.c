/*
 * scheduler.c
 *
 *  Created on: 09.03.2016
 *      Author: dode@luniks.net
 */

#include <stdio.h>
#include "scheduler.h"
#include "integers.h"
#include "interrupts.h"

static Task tasks[] = {
		{0, 0, 0, true},
		{0, 0, 0, true},
		{0, 0, 0, true}
};

bool scheduleTask(void (*func)(void), uint16_t const delay) {
	bool scheduled = false;
	for (int i = 0; i < ARRAY_LENGTH(tasks); i++) {
		if (tasks[i].done) {
			tasks[i].func = func;
			tasks[i].delay = delay;
			tasks[i].time = getTime();
			tasks[i].done = false;
			scheduled = true;
			break;
		}
	}

	return scheduled;
}

void runTasks(void) {
	for (int i = 0; i < ARRAY_LENGTH(tasks); i++) {
		if (! tasks[i].done && getTime() - tasks[i].time >= tasks[i].delay) {
			tasks[i].func();
			tasks[i].done = true;
		}
	}
}
