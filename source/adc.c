/*
 * adc.c
 *
 *  Created on: Apr 8, 2018
 *      Author: ben
 *
 * Note: Centered from 2000-2200
 */

#include "adc.h"

SemaphoreHandle_t xSequenceASemaphore = NULL;
SemaphoreHandle_t xSequenceBSemaphore = NULL;

void ADC_Read_A(void *pvParameters) {
	static adc_result_info_t yaw;
	static adc_result_info_t power;
	static int reads = 0;

	for (;;) {
		xSemaphoreTake(xSequenceASemaphore, portMAX_DELAY);

		if (++reads == 1000) {
			reads = 0;
			GPIO_PortToggle(GPIO, BOARD_INITPINS_LED1_PORT, 1 << BOARD_INITPINS_LED1_PIN);
		}

		ADC_GetChannelConversionResult(ADC0, 4U, &yaw);
		//PRINTF("YAW = %d\t", yaw.result);

		ADC_GetChannelConversionResult(ADC0, 5U, &power);
		//PRINTF("POW = %d\t", power.result);

		ADC_DoSoftwareTriggerConvSeqB(ADC0);
	}
}

void ADC_Read_B(void *pvParameters) {
	static adc_result_info_t pitch;
	static adc_result_info_t roll;
	static int reads = 0;
	for (;;) {
		xSemaphoreTake(xSequenceBSemaphore, portMAX_DELAY);

		if (++reads == 1000) {
			reads = 0;
			GPIO_PortToggle(GPIO, BOARD_INITPINS_LED2_PORT, 1 << BOARD_INITPINS_LED2_PIN);
		}

		ADC_GetChannelConversionResult(ADC0, 6U, &pitch);
		//PRINTF("PITCH = %d\t", pitch.result);

		ADC_GetChannelConversionResult(ADC0, 7U, &roll);
		//PRINTF("ROLL = %d\r\n", roll.result);

		ADC_DoSoftwareTriggerConvSeqA(ADC0);
	}
}

void Joy_ADC_Init(void) {
	ADC_ClockPower_Configuration();
	PERIPH_InitAdc();

	xSequenceASemaphore = xSemaphoreCreateBinary();
	xSequenceBSemaphore = xSemaphoreCreateBinary();

    if (xTaskCreate(ADC_Read_A, "Sequence A Task", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF(" Sequence A Task creation failed!.\r\n");
		while (1)
			;
	}

	if (xTaskCreate(ADC_Read_B, "Sequence B Task", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
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

void ADC_ClockPower_Configuration(void)
{
    /* SYSCON power. */
    POWER_DisablePD(kPDRUNCFG_PD_VDDA);    /* Power on VDDA. */
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);    /* Power on the ADC converter. */
    POWER_DisablePD(kPDRUNCFG_PD_VD2_ANA); /* Power on the analog power supply. */
    POWER_DisablePD(kPDRUNCFG_PD_VREFP);   /* Power on the reference voltage source. */
    POWER_DisablePD(kPDRUNCFG_PD_TS);      /* Power on the temperature sensor. */

    CLOCK_EnableClock(kCLOCK_Adc0); /* SYSCON->AHBCLKCTRL[0] |= SYSCON_AHBCLKCTRL_ADC0_MASK; */
}

/*
 * ISR for ADC conversion
 * Release semaphore for the completed conversion sequence
 * Trigger the next sequence (TODO can this be done in hardware through configuration?)
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
 * Trigger the next sequence (TODO can this be done in hardware through configuration?)
 */
void ADC0_SEQB_IRQHandler(void)
{
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (kADC_ConvSeqBInterruptFlag == (kADC_ConvSeqBInterruptFlag & ADC_GetStatusFlags(ADC0))) {
    	ADC_ClearStatusFlags(ADC0, kADC_ConvSeqBInterruptFlag);
    	xSemaphoreGiveFromISR(xSequenceBSemaphore, &xHigherPriorityTaskWoken);
    }
}
