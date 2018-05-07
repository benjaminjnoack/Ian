/*
 * ctimer.h
 *
 *  Created on: May 6, 2018
 *      Author: ben
 */

#ifndef CTIMER_H_
#define CTIMER_H_

#include "stdbool.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "fsl_ctimer.h"
#include "debug.h"

void ctimerInitialize(void);

extern SemaphoreHandle_t timer0Semaphore;

#endif /* CTIMER_H_ */
