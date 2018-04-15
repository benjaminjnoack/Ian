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

#include "fsl_debug_console.h"
#include "fsl_usart_freertos.h"

#define USART_BUF_SIZE 2

void usartInitialize(void);
void usartSendTask(void *pvParameters);
void usartReceiveTask(void *pvParameters);
portBASE_TYPE usartSendToQueue(uint8_t *buf, TickType_t n);

#endif /* USART_H_ */
