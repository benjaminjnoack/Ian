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

void ADC_Read_A(void *pvParameters);
void ADC_Read_B(void *pvParameters);
void ADC_Conversion_Configuration(void);
void ADC_ClockPower_Configuration(void);
void Joy_ADC_Init(void);

#endif /* ADC_H_ */
