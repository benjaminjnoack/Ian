/*
 * ctimer.c
 *
 *  Created on: May 6, 2018
 *      Author: ben
 */

#include "ctimer.h"

SemaphoreHandle_t timer0Semaphore = NULL;

void ctimerMatch0(uint32_t flags);

ctimer_callback_t timer_callback;

void ctimerInitialize(void) {
	ctimer_config_t config;
	ctimer_match_config_t match;

	timer0Semaphore = xSemaphoreCreateBinary();

	CTIMER_GetDefaultConfig(&config);
	CTIMER_Init(CTIMER0, &config);

	match.matchValue = USEC_TO_COUNT(100, CLOCK_GetFreq(kCLOCK_BusClk));
	match.enableCounterReset = true;
	match.enableCounterStop = true;
	match.enableInterrupt = true;
	match.outControl = kCTIMER_Output_NoAction;
	match.outPinInitState = true;
	CTIMER_SetupMatch(CTIMER0, kCTIMER_Match_0, &match);

	NVIC_SetPriority(CTIMER0_IRQn, 2);
	CTIMER_RegisterCallBack(CTIMER0, &timer_callback, kCTIMER_SingleCallback);
}

void ctimerMatch0(uint32_t flags) {
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	debugToggle();
	xSemaphoreGiveFromISR(timer0Semaphore, &xHigherPriorityTaskWoken);
}
