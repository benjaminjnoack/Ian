/*
 * uart.h
 *
 *  Created on: Apr 12, 2018
 *      Author: ben
 */

#ifndef USART_H_
#define USART_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_debug_console.h"
#include "fsl_usart.h"
#include "peripherals.h"

/**
 * Break generation and detection
 */

void usartInitialize(void);
void usartTxTask(void *pvParameters);
void usartRxTask(void *pvParameters);
portBASE_TYPE usartSendToQueue(uint8_t *buf, TickType_t n);
void USART_UserCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData);

#endif /* USART_H_ */
