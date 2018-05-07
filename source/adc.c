/*
 * adc.c
 *
 *  Created on: Apr 8, 2018
 *      Author: ben
 *
 * Note: Centered from 2000-2200
 */

#include "adc.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

SemaphoreHandle_t xSequenceASemaphore = NULL;
SemaphoreHandle_t xSequenceBSemaphore = NULL;

static const uint32_t DEAD_CENTER = 0x800;

struct sequence_parameter parameters[2];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void adcReadSequenceTask(void *pvParameters);
uint8_t adcDoBoxcarAverage(uint32_t *buffer, uint32_t **bufferPtr, uint32_t reading);


/*******************************************************************************
 * Code
 ******************************************************************************/

uint8_t adcDoBoxcarAverage(uint32_t *buffer, uint32_t **bufferPtr, uint32_t reading) {
	uint32_t avg = 0;
	int i;

	*(*bufferPtr) = reading;
	if (++(*bufferPtr) == (buffer + (BUFFER_LENGTH - 1))) {
		(*bufferPtr) = buffer;
	}

	for (i = 0; i < BUFFER_LENGTH; i++) {
		avg += buffer[i];
	}

	return (uint8_t)((avg / BUFFER_LENGTH) >> 4);
}

void adcInitialize(void) {
	/* SYSCON power. */
	POWER_DisablePD(kPDRUNCFG_PD_VDDA);    /* Power on VDDA. */
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);    /* Power on the ADC converter. */
	POWER_DisablePD(kPDRUNCFG_PD_VD2_ANA); /* Power on the analog power supply. */
	POWER_DisablePD(kPDRUNCFG_PD_VREFP);   /* Power on the reference voltage source. */
	POWER_DisablePD(kPDRUNCFG_PD_TS);      /* Power on the temperature sensor. */

	CLOCK_EnableClock(kCLOCK_Adc0); /* SYSCON->AHBCLKCTRL[0] |= SYSCON_AHBCLKCTRL_ADC0_MASK; */

	PERIPH_InitAdc();

	xSequenceASemaphore = xSemaphoreCreateBinary();
	xSequenceBSemaphore = xSemaphoreCreateBinary();

	parameters[0].led = BOARD_INITPINS_LED1_PIN;
	parameters[0].next = ADC_DoSoftwareTriggerConvSeqB;
	parameters[0].semaphore = xSequenceASemaphore;
	parameters[0].xAxis.channel = 4;
	parameters[0].xAxis.command = AXIS_YAW;
	parameters[0].yAxis.channel = 5;
	parameters[0].yAxis.command = AXIS_THRUST;

	parameters[1].led = BOARD_INITPINS_LED2_PIN;
	parameters[1].next = ADC_DoSoftwareTriggerConvSeqA;
	parameters[1].semaphore = xSequenceBSemaphore;
	parameters[1].xAxis.channel = 6;
	parameters[1].xAxis.command = AXIS_ROLL;
	parameters[1].yAxis.channel = 7;
	parameters[1].yAxis.command = AXIS_PITCH;

    if (xTaskCreate(adcReadSequenceTask, "Sequence A", configMINIMAL_STACK_SIZE + 128, &parameters[0], (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF(" Sequence A Task creation failed!.\r\n");
		while (1)
			;
	}

	if (xTaskCreate(adcReadSequenceTask, "Sequence B", configMINIMAL_STACK_SIZE + 128, &parameters[1], (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF("Sequence B Task creation failed!.\r\n");
		while (1)
			;
	}

	/* Enable interrupt from ADC */
	ADC_EnableInterrupts(ADC0, kADC_ConvSeqAInterruptEnable | kADC_ConvSeqBInterruptEnable);
	/* Enable interrupt in Nested Vectored Interrupt Controller */
	NVIC_SetPriority(ADC0_SEQA_IRQn, 3U);
	NVIC_SetPriority(ADC0_SEQB_IRQn, 3U);
	NVIC_EnableIRQ(ADC0_SEQA_IRQn);
	NVIC_EnableIRQ(ADC0_SEQB_IRQn);

	//kick it off
	CTIMER_StartTimer(CTIMER0);
	ADC_DoSoftwareTriggerConvSeqA(ADC0);
}

void adcReadSequenceTask(void *pvParameters) {
	int count;
	struct sequence_parameter parameters;

	uint32_t xAverage, yAverage;
	uint32_t xBuffer[BUFFER_LENGTH];
	uint32_t yBuffer[BUFFER_LENGTH];
	uint8_t xCmd[USART_1_TX_BUFFER_SIZE];
	uint8_t yCmd[USART_1_TX_BUFFER_SIZE];
	uint8_t xLast, yLast;
	uint32_t *xPtr, *yPtr;
	adc_result_info_t xResult, yResult;

	count = 0;
	xAverage = 0;
	yAverage = 0;
	xLast = DEAD_CENTER;
	yLast = DEAD_CENTER;
	xPtr = xBuffer;
	yPtr = yBuffer;

	memset(xBuffer, DEAD_CENTER, BUFFER_LENGTH);
	memset(yBuffer, DEAD_CENTER, BUFFER_LENGTH);

	parameters = *(struct sequence_parameter *) pvParameters;
	xCmd[0] = parameters.xAxis.command;
	yCmd[0] = parameters.yAxis.command;

	for (;;) {
		xSemaphoreTake(parameters.semaphore, portMAX_DELAY);

		if (++count == 100) {
			count = 0;
			GPIO_PortToggle(GPIO, 3U, 1 << parameters.led);
		}

		ADC_GetChannelConversionResult(ADC0, parameters.xAxis.channel, &xResult);
		xAverage = adcDoBoxcarAverage(xBuffer, &xPtr, xResult.result);
		if (abs((int)xLast - (int)xAverage) > 0x03) {
			xCmd[1] = xAverage;
			if (pdTRUE == usartSendToQueue(xCmd, 0)) {
				xLast = xAverage;
//				PRINTF("%c = %d\r\n", xCmd[0], xLast);
			}
		}


		ADC_GetChannelConversionResult(ADC0, parameters.yAxis.channel, &yResult);
		yAverage = adcDoBoxcarAverage(yBuffer, &yPtr, yResult.result);
		if (abs((int)yLast - (int)yAverage) > 0x03) {
			yCmd[1] = yAverage;
			if (pdTRUE == usartSendToQueue(yCmd, 0)) {
				yLast = yAverage;
//				PRINTF("%c = %d\r\n", yCmd[0], yLast);
			}
		}

		xSemaphoreTake(timer0Semaphore, portMAX_DELAY);
		CTIMER_StartTimer(CTIMER0);
		parameters.next(ADC0);
		taskYIELD();
	}
}

/*
 * ISR for ADC conversion
 * Release semaphore for the completed conversion sequence
 */
void ADC0_SEQA_IRQHandler(void)
{
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (kADC_ConvSeqAInterruptFlag == (kADC_ConvSeqAInterruptFlag & ADC_GetStatusFlags(ADC0))) {
    	ADC_ClearStatusFlags(ADC0, kADC_ConvSeqAInterruptFlag);
    	xSemaphoreGiveFromISR(xSequenceASemaphore, &xHigherPriorityTaskWoken);
    }
}

/*
 * ISR for ADC conversion
 * Release semaphore for the completed conversion sequence
 */
void ADC0_SEQB_IRQHandler(void)
{
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (kADC_ConvSeqBInterruptFlag == (kADC_ConvSeqBInterruptFlag & ADC_GetStatusFlags(ADC0))) {
    	ADC_ClearStatusFlags(ADC0, kADC_ConvSeqBInterruptFlag);
    	xSemaphoreGiveFromISR(xSequenceBSemaphore, &xHigherPriorityTaskWoken);
    }
}
