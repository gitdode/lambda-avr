/*
 * scheduler-test.c
 *
 *  Created on: 20.03.2016
 *      Author: dode@luniks.net
 */

#include "avrjunit.h"
#include "integers.h"
#include "interrupts.h"
#include "scheduler.h"

/* Module scheduler */

static bool val1 = false;
static bool val2 = false;
static bool val3 = false;
static bool val4 = false;

static void task1(void) {
	val1 = true;
}

static void task2(void) {
	val2 = true;
}

static void task3(void) {
	val3 = true;
}

static void task4(void) {
	val4 = true;
}

static void runAllTasks(void) {
	resetTime();
	// make sure any tasks scheduled by other tests run
	addTime(UINT32_MAX);
	runTasks();
}

static bool testSchedule(void) {

	runAllTasks();
	resetTime();

	assertTrue(scheduleTask(task1, 0));
	assertTrue(scheduleTask(task2, 10));
	assertTrue(scheduleTask(task3, 20));
	assertFalse(scheduleTask(task4, 30));

	return true;
}

static bool testRunTasks(void) {

	resetTime();

	runTasks();
	assertTrue(val1);
	assertFalse(val2);
	assertFalse(val3);
	assertFalse(val4);
	assertTrue(scheduleTask(task4, 30));

	addTime(10);

	runTasks();
	assertTrue(val1);
	assertTrue(val2);
	assertFalse(val3);
	assertFalse(val4);

	addTime(15);

	runTasks();
	assertTrue(val1);
	assertTrue(val2);
	assertTrue(val3);
	assertFalse(val4);

	addTime(30);

	runTasks();
	assertTrue(val1);
	assertTrue(val2);
	assertTrue(val3);
	assertTrue(val4);

	return true;
}

/* Test "class" */
static const char class[] PROGMEM = "scheduler";

/* Test names */
static const char testSchedule_P[] PROGMEM = "testSchedule";
static const char testRunTasks_P[] PROGMEM = "testRunTasks";

/* Tests */
static TestCase const tests[] = {
		{class, testSchedule_P, testSchedule},
		{class, testRunTasks_P, testRunTasks}
};

TestClass schedulerClass = {tests, ARRAY_LENGTH(tests)};
