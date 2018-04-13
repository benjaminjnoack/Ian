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

#define BUFFER_LENGTH 32

void initializeADC(void);

#endif /* ADC_H_ */
