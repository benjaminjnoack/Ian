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

#define UART_MESSAGE_LENGTH 3
#define BUFFER_LENGTH 32

typedef enum {
	AXIS_THRUST = 0x54,
	AXIS_YAW = 0x59,
	AXIS_ROLL = 0x52,
	AXIS_PITCH = 0x50
} axis_t;


void adcInitialize(void);

#endif /* ADC_H_ */
