/*
 * adc.h
 *
 *  Created on: Apr 8, 2018
 *      Author: ben
 */

#ifndef ADC_H_
#define ADC_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "fsl_debug_console.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "peripherals.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include <stdlib.h>
#include "usart.h"

#define BUFFER_LENGTH 32

typedef enum {
	AXIS_THRUST = 'T',	//0x54
	AXIS_YAW = 'Y',		//0x59
	AXIS_ROLL = 'R',	//0x52
	AXIS_PITCH = 'P'	//0x50
} axis_cmd_t;

typedef struct axis {
	uint8_t channel;
	axis_cmd_t command;
} axis_t;

struct sequence_parameter {
	uint8_t led;
	SemaphoreHandle_t semaphore;
	axis_t xAxis;
	axis_t yAxis;
	void (*next)(ADC_Type *base);
};

void adcInitialize(void);

#endif /* ADC_H_ */
