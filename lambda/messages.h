/*
 * messages.h
 *
 *  Created on: 24.05.2015
 *      Author: dode@luniks.net
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

#ifndef LANG
	#define LANG 0
#endif

#if LANG == 0

	/* lambda.c */
	#define MSG_WELCOME "     Hello!     "

	/* sensors.c */
	#define MSG_LEAN "Lean "
	#define MSG_OKAY "Okay "
	#define MSG_IDEAL "Ideal"
	#define MSG_RICH "Rich!"

	/* rules.c */
	#define MSG_AIRGATE_50_0 "Air gate 50%"
	#define MSG_AIRGATE_25_0 "Air gate 25%"
	#define MSG_AIRGATE_CLOSE_0 "Close air gate"
	#define MSG_TOO_RICH_0 "Too rich, open"
	#define MSG_TOO_RICH_1 "air gate"
	#define MSG_FIRE_OUT_0 "Fire out?"
	#define MSG_HEATING_UP_0 "Oxygen sensor"
	#define MSG_HEATING_UP_1 "heating up..."
	#define MSG_HEATING_READY_0 "Oxygen sensor"
	#define MSG_HEATING_READY_1 "ready"
	#define MSG_HEATING_FAULT_0 "Oxygen sensor"
	#define MSG_HEATING_FAULT_1 "heating fault!"

#elif LANG == 1

	/* lambda.c */
	#define MSG_WELCOME "     Hallo!     "

	/* sensors.c */
	#define MSG_LEAN "Mager"
	#define MSG_OKAY "Okay "
	#define MSG_IDEAL "Ideal"
	#define MSG_RICH "Fett!"

	/* rules.c */
	#define MSG_AIRGATE_50_0 "Luftschieber 50%"
	#define MSG_AIRGATE_25_0 "Luftschieber 25%"
	#define MSG_AIRGATE_CLOSE_0 "Luftschieber zu"
	#define MSG_TOO_RICH_0 "Zu fett, Luft-"
	#define MSG_TOO_RICH_1 "schieber oeffnen"
	#define MSG_FIRE_OUT_0 "Feuer aus?"
	#define MSG_HEATING_UP_0 "Lambdasonde"
	#define MSG_HEATING_UP_1 "waermt auf..."
	#define MSG_HEATING_READY_0 "Lambdasonde"
	#define MSG_HEATING_READY_1 "bereit"
	#define MSG_HEATING_FAULT_0 "Lambdasonden-"
	#define MSG_HEATING_FAULT_1 "heizung Fehler!"

#endif

#endif /* MESSAGES_H_ */
