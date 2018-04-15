/*
 * uart.c
 *
 *  Created on: Apr 12, 2018
 *      Author: ben
 */

#include <usart.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

QueueHandle_t xUartQueue;

uint8_t txBuffer[USART_BUF_SIZE];
uint8_t rxBuffer[USART_BUF_SIZE];

struct rtos_usart_config usartConfig;
usart_handle_t usartHandle;
usart_rtos_handle_t usartRtosHandle;

void usartInitialize(void) {

	xUartQueue = xQueueCreate(10, USART_BUF_SIZE);

	usartConfig.base = USART4;
	usartConfig.baudrate = 9600U;
	usartConfig.buffer = txBuffer;
	usartConfig.buffer_size = USART_BUF_SIZE;
	usartConfig.parity = kUSART_ParityDisabled;
	usartConfig.srcclk = CLOCK_GetFreq(kCLOCK_FroHf);
	usartConfig.stopbits = kUSART_OneStopBit;

	NVIC_SetPriority(FLEXCOMM4_IRQn, 4);

	if (0 > USART_RTOS_Init(&usartRtosHandle, &usartHandle, &usartConfig)) {
		PRINTF("Failed to initialize USART!\r\n");
		while(1)
			;
	}

	if (xTaskCreate(usartReceiveTask, "USART RX", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF(" Sequence A Task creation failed!.\r\n");
		while (1)
			;
	}

	if (xTaskCreate(usartSendTask, "USART TX", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF("Sequence B Task creation failed!.\r\n");
		while (1)
			;
	}
}

void usartReceiveTask(void *pvParameters) {
	static int error;
	size_t n;

	do {
		error = USART_RTOS_Receive(&usartRtosHandle, rxBuffer, USART_BUF_SIZE, &n);
		if (n > 0) {
			PRINTF("Received %s\n", rxBuffer);
		}
	} while (kStatus_Success == error);
}

void usartSendTask(void *pvParameters) {
	uint8_t msgBuf[USART_BUF_SIZE];

	for (;;) {
		xQueueReceive(xUartQueue, msgBuf, portMAX_DELAY);
		PRINTF("SEND %c%d\r\n", msgBuf[0], msgBuf[1]);
		USART_RTOS_Send(&usartRtosHandle, msgBuf, USART_BUF_SIZE);
	}
}

portBASE_TYPE usartSendToQueue(uint8_t *buf, TickType_t n) {
	return xQueueSendToBack(xUartQueue, buf, n);
}
