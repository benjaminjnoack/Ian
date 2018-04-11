/*
 * joystick.c
 *
 *  Created on: Apr 10, 2018
 *      Author: ben
 */

#include "joystick.h"

xQueueHandle xJoystickQueue = NULL;
void JoystickTask(void *pvParameters);

void initializeJoystick(void) {
	xJoystickQueue = xQueueCreate(2, sizeof(joystick_read_t));

	if (xTaskCreate(JoystickTask, "Joy Stick Task", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF("Joy Stick Task creation failed!.\r\n");
		while (1)
			;
	}
}

BaseType_t sendToJoystickQueue(joystick_read_t *read) {
	return xQueueSend(xJoystickQueue, read, portMAX_DELAY);
}

void JoystickTask(void *pvParameters) {
	joystick_read_t read;
	portBASE_TYPE xStatus;

	for (;;) {
		xStatus = xQueueReceive(xJoystickQueue, &read, portMAX_DELAY);

		if (xStatus == pdPASS) {
			switch (read.joystick) {
			case JOYSTICK_A:
//				PRINTF("YAW = %d\t", read.horizontal_axis);
//				PRINTF("POW = %d\r\n", read.vertical_axis);
				break;
			case JOYSTICK_B:
//				PRINTF("ROLL = %d\t", read.horizontal_axis);
//				PRINTF("PITCH = %d\r\n", read.vertical_axis);
				break;
			}
		}
	}
}
