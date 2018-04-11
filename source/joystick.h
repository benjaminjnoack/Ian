/*
 * joystick.h
 *
 *  Created on: Apr 10, 2018
 *      Author: ben
 *
 *  TODO
 *  Create a queue for incoming reads.
 *  Single task to wait for at least one read
 *  Filtering and conversion done here
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "fsl_debug_console.h"

typedef enum joystick {
	JOYSTICK_A = 0x41,//ASCII 'A'
	JOYSTICK_B
} joystick_t;

typedef struct joystick_read {
	joystick_t joystick;
	uint32_t horizontal_axis;
	uint32_t vertical_axis;
} joystick_read_t;

void initializeJoystick(void);
BaseType_t sendToJoystickQueue(joystick_read_t *read);

#endif /* JOYSTICK_H_ */
