/*
 * serial.h
 *
 *  Created on: 29-Sep-2021
 *      Author: AJ
 */

#ifndef SRC_APPLICATION_SERIAL_API_H_
#define SRC_APPLICATION_SERIAL_API_H_
#include "hal.h"
#define	APP_TRUE		    		1
#define	APP_FALSE		    		0

#define UART_INTERRUPT_RECEIVE_METHOD			0
#define UART_DMA_RECEIVE_METHOD					1
#define UART_RECEIVE_METHOD						UART_DMA_RECEIVE_METHOD


#define DEFAULT_BAUD_RATE						9600

#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
#define UART_DMA_RX_BUFFER_SIZE					10
#endif

#define PRINTF_DEBUG_SUPPORT		1

typedef struct{
	UART_HandleTypeDef* huart;
#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
	uint8_t dmaRxbuffer[UART_DMA_RX_BUFFER_SIZE];
#else
	uint8_t interrupt_rx_byte;
#endif
	uint8_t* aucBuffer;
	uint16_t usCircularBufferSize;
    uint16_t usReadIndex;
    uint16_t usWriteIndex;
    uint16_t noOfErrors;
}ST_UART_Transceiver;

extern ST_UART_Transceiver    gstUART_TransceiverList[MAX_UART_COMMUNICATION];

void check_uart_receiver_errors(ST_UART_Transceiver* stUART_ReceiverObj);
void ResetUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj);
void InitializeUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj,
		UART_HandleTypeDef* huart,
		uint8_t* 	aucBuffer, uint16_t	usCircularBufferSize);

HAL_StatusTypeDef StartUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj);
#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
HAL_StatusTypeDef StartUART_Receiver_DMA(ST_UART_Transceiver* stUART_ReceiverObj);
#else
HAL_StatusTypeDef StartUART_Receiver_IT(ST_UART_Transceiver* stUART_ReceiverObj);
#endif
void StopUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj);
uint16_t GetUART_ReceiverBytesAvailable(ST_UART_Transceiver* stUART_ReceiverObj);
bool IsUART_ReceiverIsEmpty(ST_UART_Transceiver *stUART_ReceiverObj);
uint8_t ReadByteFrom_UART_Receiver(ST_UART_Transceiver *stUART_ReceiverObj);
uint8_t IsByteReceived(ST_UART_Transceiver *stUART_ReceiverObj, uint8_t *ucByteReceived);

HAL_StatusTypeDef WriteByteStreamTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const uint8_t* data, uint16_t len);
HAL_StatusTypeDef WriteByteTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, uint8_t data);
HAL_StatusTypeDef WriteStringTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const char* data);
HAL_StatusTypeDef WriteLineTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const char *str);
void DebugPrintToUART(ST_UART_Transceiver *stUART_ReceiverObj, const char *format, ... );
void UpdateUart_Baudrate(ST_UART_Transceiver* stUART_Transceiver, uint32_t baudrate);

#endif /* SRC_APPLICATION_SERIAL_API_H_ */
