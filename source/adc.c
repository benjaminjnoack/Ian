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

uint8_t rollCmd[USART_1_TX_BUFFER_SIZE] = {AXIS_ROLL};

uint8_t thrustCmd[USART_1_TX_BUFFER_SIZE] = {AXIS_THRUST};

uint8_t pitchCmd[USART_1_TX_BUFFER_SIZE] = {AXIS_PITCH};

uint8_t yawCmd[USART_1_TX_BUFFER_SIZE] = {AXIS_YAW};

struct sequence_parameter {
	SemaphoreHandle_t semaphore;
	int xChannel;
	int yChannel;
};

struct sequence_parameter parameters[2];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void adcReadSequenceTask(void *pvParameters);
void adcReadSequenceBTask(void *pvParameters);
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

	parameters[0].semaphore = xSequenceASemaphore;
	parameters[0].xChannel = 4;
	parameters[0].yChannel = 5;
	parameters[1].semaphore = xSequenceBSemaphore;
	parameters[1].xChannel = 6;
	parameters[1].yChannel = 7;

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
	NVIC_SetPriority(ADC0_SEQA_IRQn, 4U);
	NVIC_SetPriority(ADC0_SEQB_IRQn, 4U);
	NVIC_EnableIRQ(ADC0_SEQA_IRQn);
	NVIC_EnableIRQ(ADC0_SEQB_IRQn);

	//kick it off
	ADC_DoSoftwareTriggerConvSeqA(ADC0);
}

//TODO initialize the buffers here, before the for loop
void adcReadSequenceTask(void *pvParameters) {
	int count, i;
	struct sequence_parameter parameters;

	uint32_t xAvgerage, yAverage;
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
	//TODO set the axis_t in the appropriate command
	xLast = DEAD_CENTER;
	yLast = DEAD_CENTER;
	xPtr = xBuffer;
	yPtr = yBuffer;

	for (i = 0; i < BUFFER_LENGTH; i++) {
		xBuffer[i] = DEAD_CENTER;
		yBuffer[i] = DEAD_CENTER;
	}

	parameters = (sequence_parameter *) pvParameters;

	for (;;) {
		xSemaphoreTake(parameters.semaphore, portMAX_DELAY);

		if (++count == 1000) {
			count = 0;
			GPIO_PortToggle(GPIO, BOARD_INITPINS_LED1_PORT, 1 << BOARD_INITPINS_LED1_PIN);//TODO pass the pin?
		}

		ADC_GetChannelConversionResult(ADC0, parameters.xChannel, &xResult);
		xAverage = adcDoBoxcarAverage(xBuffer, &xPtr, xResult.result);
		if (abs((int)xLast - (int)xAverage) > 0x03) {
			yawCmd[1] = xAvg;
			if (pdTRUE == usartSendToQueue(yawCmd, 0)) {
				xLast = xAvg;
			}
		}


		ADC_GetChannelConversionResult(ADC0, 5U, &yRes);
		yAvg = adcDoBoxcarAverage(thrustBuffer, &thrustPtr, yRes.result);
		if (abs((int)lastThrust - (int)yAvg) > 0x03) {
			thrustCmd[1] = yAvg;
			if (pdTRUE == usartSendToQueue(thrustCmd, 0)) {
				lastThrust = yAvg;
			}
			//PRINTF("THR = %d\r\n", lastThrust);
		}

		ADC_DoSoftwareTriggerConvSeqB(ADC0);
		taskYIELD();
	}
}

void adcReadSequenceBTask(void *pvParameters) {
	static adc_result_info_t pitch;
	uint32_t avgPitch = 0;
	static adc_result_info_t roll;
	uint32_t avgRoll = 0;
	static int count = 0;

	for (;;) {
		xSemaphoreTake(xSequenceBSemaphore, portMAX_DELAY);

		if (++count == 1000) {
			count = 0;
			GPIO_PortToggle(GPIO, BOARD_INITPINS_LED2_PORT, 1 << BOARD_INITPINS_LED2_PIN);
		}

		ADC_GetChannelConversionResult(ADC0, 6U, &pitch);
		avgPitch = adcDoBoxcarAverage(pitchBuffer, &pitchPtr, pitch.result);
		if (abs((int)lastPitch - (int)avgPitch) > 0x03) {
			pitchCmd[1] = avgPitch;
			if (pdTRUE == usartSendToQueue(pitchCmd, 0)) {
				lastPitch = avgPitch;
			}

//			PRINTF("PITCH = %d\r\n", lastPitch);
		}



		ADC_GetChannelConversionResult(ADC0, 7U, &roll);
		avgRoll = adcDoBoxcarAverage(rollBuffer, &rollPtr, roll.result);
		if (abs((int)lastRoll - (int)avgRoll) > 0x03) {
			rollCmd[1] = avgRoll;
			if (pdTRUE == usartSendToQueue(rollCmd, 0)) {
				lastRoll = avgRoll;
			}

//			PRINTF("ROLL = %d\r\n", lastRoll);
		}

		ADC_DoSoftwareTriggerConvSeqA(ADC0);
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
