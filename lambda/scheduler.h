/*
 * scheduler.h
 *
 *  Created on: 09.03.2016
 *      Author: dode@luniks.net
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdbool.h>

/* Not used, but interesting */
#define lambda(return_type, function_body) ({ \
      return_type function_name function_body function_name; \
})

/*
	void (*bla)(char*, int8_t) = lambda(void, (char* text, int8_t num) {
		printf("Bla: %s, %d\n", text, num);
	});

	bla("blubb", 3);
 */

/**
 * Internally used task.
 */
typedef struct {
	void (*func)(void);
	uint16_t delay;
	uint16_t time;
	bool done;
} Task;

/**
 * Schedules the given function for execution after the given delay in seconds.
 * Returns false if the function could not be scheduled because there were
 * no free slots available, true otherwise.
 * The function must have no return value and take no arguments. Arbitrary code
 * can be passed for execution by wrapping it in such a function:
 *
 * void task(void) {
 *     // some code here
 * }
 * // run task in 30 seconds
 * scheduleTask(task, 30);
 */
bool scheduleTask(void (*func)(void), uint16_t const delay);

/**
 * Runs the tasks where the delay has passed. A task that was run frees up a
 * slot so another task can be scheduled.
 */
void runTasks(void);

#endif /* SCHEDULER_H_ */
