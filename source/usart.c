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
SemaphoreHandle_t xUartTxSemaphore;

void usartInitialize(void) {

	xUartQueue = xQueueCreate(10, USART_1_TX_BUFFER_SIZE);

	xUartTxSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xUartTxSemaphore);

	NVIC_SetPriority(FLEXCOMM4_IRQn, 4);

//	if (xTaskCreate(usartRxTask, "USART RX", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
//	{
//		PRINTF(" Sequence A Task creation failed!.\r\n");
//		while (1)
//			;
//	}

	if (xTaskCreate(usartTxTask, "USART TX", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), NULL) != pdPASS)
	{
		PRINTF("Sequence B Task creation failed!.\r\n");
		while (1)
			;
	}
}

void usartRxTask(void *pvParameters) {
	/**
	 * TODO define a bare bones receive task
	 */
}

void usartTxTask(void *pvParameters) {
	for (;;) {
		xQueueReceive(xUartQueue, USART_1_txTransfer.data, portMAX_DELAY);
		xSemaphoreTake(xUartTxSemaphore, portMAX_DELAY);
		//TODO casting usart_transfer_t is a hack to avoid compile warnings
		USART_TransferSendNonBlocking(USART4, &USART_1_handle, (usart_transfer_t *)&USART_1_txTransfer);
	}
}

portBASE_TYPE usartSendToQueue(uint8_t *buf, TickType_t n) {
	return xQueueSendToBack(xUartQueue, buf, n);
}

void USART_UserCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData) {
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (kStatus_USART_TxIdle == status) {
		xSemaphoreGiveFromISR(xUartTxSemaphore, &xHigherPriorityTaskWoken);
	} else {
		PRINTF("STAT: %d\n\n", status);
	}
}
