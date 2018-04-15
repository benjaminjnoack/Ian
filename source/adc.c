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

static uint32_t rollBuffer[BUFFER_LENGTH];
uint32_t *rollPtr = rollBuffer;
uint8_t lastRoll = 0x80;
uint8_t rollCmd[USART_BUF_SIZE] = {AXIS_ROLL};

static uint32_t thrustBuffer[BUFFER_LENGTH];
uint32_t *thrustPtr = thrustBuffer;
uint8_t lastThrust = 0x80;
uint8_t thrustCmd[USART_BUF_SIZE] = {AXIS_THRUST};

static uint32_t pitchBuffer[BUFFER_LENGTH];
uint32_t *pitchPtr = pitchBuffer;
uint8_t lastPitch = 0x80;
uint8_t pitchCmd[USART_BUF_SIZE] = {AXIS_PITCH};

static uint32_t yawBuffer[BUFFER_LENGTH];
uint32_t *yawPtr = yawBuffer;
uint8_t lastYaw = 0x80;
uint8_t yawCmd[USART_BUF_SIZE] = {AXIS_YAW};
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void adcReadSequenceATask(void *pvParameters);
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

	int i;

	for (i = 0; i < BUFFER_LENGTH; i++) {
		rollBuffer[i] = DEAD_CENTER;
		yawBuffer[i] = DEAD_CENTER;
		pitchBuffer[i] = DEAD_CENTER;
		thrustBuffer[i] = DEAD_CENTER;
	}

    if (xTaskCreate(adcReadSequenceATask, "Sequence A Task", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF(" Sequence A Task creation failed!.\r\n");
		while (1)
			;
	}

	if (xTaskCreate(adcReadSequenceBTask, "Sequence B Task", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
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

void adcReadSequenceATask(void *pvParameters) {
	static adc_result_info_t yaw;
	uint32_t avgYaw = 0;
	static adc_result_info_t thrust;
	uint32_t avgThrust = 0;
	static int count = 0;

	for (;;) {
		xSemaphoreTake(xSequenceASemaphore, portMAX_DELAY);

		if (++count == 1000) {
			count = 0;
			GPIO_PortToggle(GPIO, BOARD_INITPINS_LED1_PORT, 1 << BOARD_INITPINS_LED1_PIN);
		}

		ADC_GetChannelConversionResult(ADC0, 4U, &yaw);
		avgYaw = adcDoBoxcarAverage(yawBuffer, &yawPtr, yaw.result);
		if (abs((int)lastYaw - (int)avgYaw) > 0x03) {
			yawCmd[1] = avgYaw;
			if (pdTRUE == usartSendToQueue(yawCmd, 0)) {
				lastYaw = avgYaw;
			}
			//PRINTF("YAW = %d\r\n", avgYaw);
		}


		ADC_GetChannelConversionResult(ADC0, 5U, &thrust);
		avgThrust = adcDoBoxcarAverage(thrustBuffer, &thrustPtr, thrust.result);
		if (abs((int)lastThrust - (int)avgThrust) > 0x03) {
			thrustCmd[1] = avgThrust;
			if (pdTRUE == usartSendToQueue(thrustCmd, 0)) {
				lastThrust = avgThrust;
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
