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

QueueHandle_t xUsartQueue;
SemaphoreHandle_t xUartTxSemaphore;
TaskHandle_t xUsartTxTask;

uint8_t usartCalculateCRC(uint8_t *buf, int n);

void usartInitialize(void) {

	xUsartQueue = xQueueCreate(10, USART_1_TX_BUFFER_SIZE);

	xUartTxSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xUartTxSemaphore);

	NVIC_SetPriority(FLEXCOMM4_IRQn, 2U);

	if (xTaskCreate(usartTxTask, "USART TX", configMINIMAL_STACK_SIZE + 128, NULL, (configMAX_PRIORITIES + 1), &xUsartTxTask) != pdPASS)
	{
		PRINTF("USART TX Task creation failed!.\r\n");
		while (1)
			;
	}
}

uint8_t usartCalculateCRC(uint8_t *buf, int n) {
	uint8_t i, result;

	for (i = 0, result = 0xFF; i < n; i++) {
		result = result ^ buf[i];
	}

	return result;
}

/**
 * TODO
 * use the clear to send semaphore to ensure the XBee is ready to receive
 */
void usartTxTask(void *pvParameters) {
	static status_t txStatus;
	static uint8_t frame[FRAME_SIZE];

	frame[0] = 0x01;

	for (;;) {
		xQueueReceive(xUsartQueue, &frame[1], portMAX_DELAY);
		xSemaphoreTake(xUartTxSemaphore, portMAX_DELAY);
		frame[3] = usartCalculateCRC(&frame[1], 2);
		//TODO casting usart_transfer_t is a hack to avoid compile warnings
		memcpy(USART_1_txTransfer.data, frame, FRAME_SIZE);
		txStatus = USART_TransferSendNonBlocking(USART4, &USART_1_handle, (usart_transfer_t *)&USART_1_txTransfer);

		if (txStatus != kStatus_Success) {
			PRINTF("TX ERR: %d", txStatus);
			break;
		}
	}

	vTaskDelete(xUsartTxTask);
}

portBASE_TYPE usartSendToQueue(uint8_t *buf, TickType_t n) {
	return xQueueSendToBack(xUsartQueue, buf, n);
}

void USART_UserCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData) {
	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (kStatus_USART_TxIdle == status) {
		xSemaphoreGiveFromISR(xUartTxSemaphore, &xHigherPriorityTaskWoken);
	} else {
		PRINTF("STAT: %d\n\n", status);
	}
}
