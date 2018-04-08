/*
 * adc.c
 *
 *  Created on: Apr 8, 2018
 *      Author: ben
 */

#include "adc.h"

SemaphoreHandle_t xSequenceASemaphore = NULL;
SemaphoreHandle_t xSequenceBSemaphore = NULL;

void ADC_Read_A(void *pvParameters) {
	static adc_result_info_t yaw;
	static adc_result_info_t power;

	for (;;) {
		xSemaphoreTake(xSequenceASemaphore, portMAX_DELAY);

		ADC_GetChannelConversionResult(ADC0, 4U, &yaw);
		PRINTF("YAW = %d\t", yaw.result);

		ADC_GetChannelConversionResult(ADC0, 5U, &power);
		PRINTF("POW = %d\t", power.result);

		ADC_DoSoftwareTriggerConvSeqB(ADC0);
	}
}

void ADC_Read_B(void *pvParameters) {
	static adc_result_info_t pitch;
	static adc_result_info_t roll;

	for (;;) {
		xSemaphoreTake(xSequenceBSemaphore, portMAX_DELAY);

		ADC_GetChannelConversionResult(ADC0, 6U, &pitch);
		PRINTF("PITCH = %d\t", pitch.result);

		ADC_GetChannelConversionResult(ADC0, 7U, &roll);
		PRINTF("ROLL = %d\r\n", roll.result);

		ADC_DoSoftwareTriggerConvSeqA(ADC0);
	}
}

void Joy_ADC_Init(void) {
	ADC_ClockPower_Configuration();
	ADC_Conversion_Configuration();

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

void ADC_Conversion_Configuration(void) {
	const adc_config_t ADC_1configStruct = {
	  .clockMode = kADC_ClockSynchronousMode,
	  .clockDividerNumber = 1,
	  .resolution = kADC_Resolution12bit,
	  .sampleTimeNumber = 0,
	  .enableBypassCalibration = false
	};
	/* Conversion sequence A configuration structure */
	const adc_conv_seq_config_t ADC_1ConvSeqAConfigStruct = {
	  .channelMask = 16U | 32U,
	  .triggerMask = 0U,
	  .triggerPolarity = kADC_TriggerPolarityNegativeEdge,
	  .enableSyncBypass = false,
	  .enableSingleStep = false,
	  .interruptMode = kADC_InterruptForEachSequence
	};
	/* Conversion sequence B configuration structure */
	const adc_conv_seq_config_t ADC_1ConvSeqBConfigStruct = {
	  .channelMask = 64U | 128U,
	  .triggerMask = 0U,
	  .triggerPolarity = kADC_TriggerPolarityNegativeEdge,
	  .enableSyncBypass = false,
	  .enableSingleStep = false,
	  .interruptMode = kADC_InterruptForEachSequence
	};

	/* Perform self calibration */
	ADC_DoSelfCalibration(ADC0);
	/* Initialize ADC0 peripheral */
	ADC_Init(ADC0, &ADC_1configStruct);
	/* Configure the conversion sequence A */
	ADC_SetConvSeqAConfig(ADC0, &ADC_1ConvSeqAConfigStruct);
	/* Enable the conversion sequence A */
	ADC_EnableConvSeqA(ADC0, true);
	/* Configure the conversion sequence B */
	ADC_SetConvSeqBConfig(ADC0, &ADC_1ConvSeqBConfigStruct);
	/* Enable the conversion sequence B */
	ADC_EnableConvSeqB(ADC0, true);
}

void ADC_ClockPower_Configuration(void)
{
    /* SYSCON power. */
    POWER_DisablePD(kPDRUNCFG_PD_VDDA);    /* Power on VDDA. */
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);    /* Power on the ADC converter. */
    POWER_DisablePD(kPDRUNCFG_PD_VD2_ANA); /* Power on the analog power supply. */
    POWER_DisablePD(kPDRUNCFG_PD_VREFP);   /* Power on the reference voltage source. */
    POWER_DisablePD(kPDRUNCFG_PD_TS);      /* Power on the temperature sensor. */

    /* Enable the clock. */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);

    /* CLOCK_AttachClk(kMAIN_CLK_to_ADC_CLK); */
    /* Sync clock source is not used. Using sync clock source and would be divided by 2.
     * The divider would be set when configuring the converter.
     */

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
