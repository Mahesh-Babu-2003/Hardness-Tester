#include <serial_api.h>
#include<stdarg.h>
char end_line[] = {"\r\n"};
uint8_t print_buffer[256];


ST_UART_Transceiver    gstUART_TransceiverList[MAX_UART_COMMUNICATION];

void UpdateUart_Baudrate(ST_UART_Transceiver* stUART_Transceiver, uint32_t baudrate){
	if(baudrate == 0){
		baudrate = DEFAULT_BAUD_RATE;
	}
	HAL_UART_DeInit(stUART_Transceiver->huart);
	stUART_Transceiver->huart->Init.BaudRate = baudrate;
	HAL_UART_Init(stUART_Transceiver->huart);
//	InitializeUART_Receiver(&gstUART_TransceiverList[TPM_UART],
//			&TPM_USART, tpm_buffer, TPM_USART_BUFFER_SIZE);
//	StartUART_Receiver(&gstUART_TransceiverList[TPM_UART]);
	StartUART_Receiver(stUART_Transceiver);
}

ST_UART_Transceiver* GetUART_ReceiverObj(UART_HandleTypeDef *huart){
	ST_UART_Transceiver* stUART_ReceiverObj = NULL;
	if(huart != NULL){
		for(uint8_t ix = 0; ix < MAX_UART_COMMUNICATION; ++ix){
			if(gstUART_TransceiverList[ix].huart == huart){
				stUART_ReceiverObj = &gstUART_TransceiverList[ix];
				break;
			}
		}
	}
	return stUART_ReceiverObj;
}


void check_uart_receiver_errors(ST_UART_Transceiver* stUART_ReceiverObj){

	  uint32_t isrflags   = READ_REG(stUART_ReceiverObj->huart->Instance->ISR);
	  //| USART_SR_IDLE | USART_SR_CTS | USART_SR_LBD
	  uint32_t errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE  )); //USART_SR_IDLE_Pos
	  if(errorflags != RESET){
		  HAL_UART_ErrorCallback(stUART_ReceiverObj->huart);
	  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	ST_UART_Transceiver* stUART_ReceiverObj = GetUART_ReceiverObj(huart);
	if(stUART_ReceiverObj != NULL){
		stUART_ReceiverObj->noOfErrors++;
//		HAL_UART_DeInit(stUART_ReceiverObj->huart);
//		HAL_UART_Init(stUART_ReceiverObj->huart);
#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
		StartUART_Receiver_DMA(stUART_ReceiverObj);
#else
		StartUART_Receiver_IT(stUART_ReceiverObj);
#endif
	}
}

#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){

	ST_UART_Transceiver* stUART_ReceiverObj = GetUART_ReceiverObj(huart);
	if(stUART_ReceiverObj != NULL){
		volatile uint16_t usWriteIndex;
		for(uint8_t ix = 0; ix < Size; ++ix){
			usWriteIndex = stUART_ReceiverObj->usWriteIndex + 1;
			if(usWriteIndex >= stUART_ReceiverObj->usCircularBufferSize){
				usWriteIndex = 0;
			}
			if(usWriteIndex != stUART_ReceiverObj->usReadIndex){
				stUART_ReceiverObj->aucBuffer[stUART_ReceiverObj->usWriteIndex] = stUART_ReceiverObj->dmaRxbuffer[ix];
				stUART_ReceiverObj->usWriteIndex = usWriteIndex;
			}

		}
		StartUART_Receiver_DMA(stUART_ReceiverObj);
	}
}
#else
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
}
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	ST_UART_Transceiver* stUART_ReceiverObj = GetUART_ReceiverObj(huart);
	if(stUART_ReceiverObj != NULL && stUART_ReceiverObj->usCircularBufferSize > 0){
#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
		StartUART_Receiver_DMA(stUART_ReceiverObj);
#else
		volatile uint8_t rx_byte = stUART_ReceiverObj->interrupt_rx_byte;
		StartUART_Receiver_IT(stUART_ReceiverObj);
		volatile uint16_t usWriteIndex = stUART_ReceiverObj->usWriteIndex + 1;
		if (usWriteIndex >= stUART_ReceiverObj->usCircularBufferSize) {
			usWriteIndex = 0;
		}
		if (usWriteIndex != stUART_ReceiverObj->usReadIndex) {
			stUART_ReceiverObj->aucBuffer[stUART_ReceiverObj->usWriteIndex] = rx_byte;
			stUART_ReceiverObj->usWriteIndex = usWriteIndex;
		}
#endif
	}

}

void ResetUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj){
	if(stUART_ReceiverObj->usCircularBufferSize > 0){
		stUART_ReceiverObj->usWriteIndex = 0;
		stUART_ReceiverObj->usReadIndex = 0;
		memset(stUART_ReceiverObj->aucBuffer, 0,
				stUART_ReceiverObj->usCircularBufferSize);
	}
}

void InitializeUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj,
		UART_HandleTypeDef* huart,
		uint8_t* 	aucBuffer, uint16_t	usCircularBufferSize){
	stUART_ReceiverObj->huart = huart;
	stUART_ReceiverObj->aucBuffer = aucBuffer;
	stUART_ReceiverObj->usCircularBufferSize = usCircularBufferSize;
	ResetUART_Receiver(stUART_ReceiverObj);
}

#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
HAL_StatusTypeDef StartUART_Receiver_DMA(ST_UART_Transceiver* stUART_ReceiverObj){
	HAL_StatusTypeDef result = HAL_ERROR;

	if(stUART_ReceiverObj != NULL && stUART_ReceiverObj->huart != NULL && stUART_ReceiverObj->huart->hdmarx != NULL){
		HAL_UARTEx_ReceiveToIdle_DMA(stUART_ReceiverObj->huart,
				stUART_ReceiverObj->dmaRxbuffer, sizeof(stUART_ReceiverObj->dmaRxbuffer));
		__HAL_DMA_DISABLE_IT(stUART_ReceiverObj->huart->hdmarx, DMA_IT_HT);

//		HAL_UARTEx_ReceiveToIdle_DMA(&PC_USART, (uint8_t *) dmaRxbuffer, sizeof(dmaRxbuffer));
//		__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);


		result = HAL_OK;
	}

	return result;
}
#else
HAL_StatusTypeDef StartUART_Receiver_IT(ST_UART_Transceiver* stUART_ReceiverObj){
	HAL_StatusTypeDef result = HAL_ERROR;

	if(stUART_ReceiverObj != NULL && stUART_ReceiverObj->huart != NULL){
		HAL_UART_Receive_IT(stUART_ReceiverObj->huart, &stUART_ReceiverObj->interrupt_rx_byte, 1);
		result = HAL_OK;
	}

	return result;
}
#endif

HAL_StatusTypeDef StartUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj){
#if(UART_RECEIVE_METHOD	== UART_DMA_RECEIVE_METHOD)
	return StartUART_Receiver_DMA(stUART_ReceiverObj);
#else
	return StartUART_Receiver_IT(stUART_ReceiverObj);
#endif
}
void StopUART_Receiver(ST_UART_Transceiver* stUART_ReceiverObj){
	ResetUART_Receiver(stUART_ReceiverObj);
	if(stUART_ReceiverObj != NULL){
		HAL_UART_AbortReceive(stUART_ReceiverObj->huart);
	}
}

uint16_t GetUART_ReceiverBytesAvailable(ST_UART_Transceiver* stUART_ReceiverObj){
	uint16_t count = 0;
	if(stUART_ReceiverObj->usWriteIndex != stUART_ReceiverObj->usReadIndex){
		if(stUART_ReceiverObj->usReadIndex < stUART_ReceiverObj->usWriteIndex){
			count = stUART_ReceiverObj->usWriteIndex - stUART_ReceiverObj->usReadIndex;
		}else{
			count = stUART_ReceiverObj->usCircularBufferSize - stUART_ReceiverObj->usReadIndex + stUART_ReceiverObj->usWriteIndex;
		}
	}

	return count;
}

bool IsUART_ReceiverIsEmpty(ST_UART_Transceiver *stUART_ReceiverObj){
    return (GetUART_ReceiverBytesAvailable(stUART_ReceiverObj) == 0);
}

uint8_t ReadByteFrom_UART_Receiver(ST_UART_Transceiver *stUART_ReceiverObj) {
	uint8_t byte = 0;
	if(stUART_ReceiverObj->usCircularBufferSize > 0
			&& stUART_ReceiverObj->usWriteIndex != stUART_ReceiverObj->usReadIndex){
		byte = stUART_ReceiverObj->aucBuffer[stUART_ReceiverObj->usReadIndex];
		++stUART_ReceiverObj->usReadIndex;
		if(stUART_ReceiverObj->usReadIndex >= stUART_ReceiverObj->usCircularBufferSize){
			stUART_ReceiverObj->usReadIndex = 0;
		}
	}
	return byte;
}

uint8_t IsByteReceived(ST_UART_Transceiver *stUART_ReceiverObj, uint8_t *ucByteReceived)
{
    if(stUART_ReceiverObj->usCircularBufferSize == 0
    		|| stUART_ReceiverObj->usWriteIndex == stUART_ReceiverObj->usReadIndex)
    {
        *ucByteReceived = 0u;
        return APP_FALSE;
    }
    *ucByteReceived	= stUART_ReceiverObj->aucBuffer[stUART_ReceiverObj->usReadIndex];
    ++stUART_ReceiverObj->usReadIndex;
	if(stUART_ReceiverObj->usReadIndex >= stUART_ReceiverObj->usCircularBufferSize){
		stUART_ReceiverObj->usReadIndex = 0;
	}

    return APP_TRUE;
}


HAL_StatusTypeDef WriteAsyncronusByteStreamTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const uint8_t* data, uint16_t len){
	uint8_t time_out = 10;
	while (stUART_ReceiverObj->huart->hdmatx->State != HAL_DMA_STATE_READY && time_out > 0){
		--time_out;
		HAL_Delay(1);
	}
	return HAL_UART_Transmit_DMA(stUART_ReceiverObj->huart, data, len);
}

HAL_StatusTypeDef WriteByteStreamTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const uint8_t* data, uint16_t len){
	return HAL_UART_Transmit(stUART_ReceiverObj->huart, data, len, len);
}

HAL_StatusTypeDef WriteByteTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, uint8_t data) {
	return WriteByteStreamTo_UART(stUART_ReceiverObj, &data, 1);
}

HAL_StatusTypeDef WriteStringTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const char* data){
	return WriteByteStreamTo_UART(stUART_ReceiverObj, (const uint8_t*)data, strlen(data));
}

HAL_StatusTypeDef WriteLineTo_UART(ST_UART_Transceiver *stUART_ReceiverObj, const char *str) {
	return WriteStringTo_UART(stUART_ReceiverObj, str) && WriteStringTo_UART(stUART_ReceiverObj, end_line);
}

void DebugPrintToUART(ST_UART_Transceiver *stUART_ReceiverObj, const char *format, ... )
{
#if(PRINTF_DEBUG_SUPPORT)
  va_list args;
  va_start(args, format);
  uint16_t len;
  /*convert into string at buff[0] of length iw*/
  len = vsprintf((char*)print_buffer, format, args);
  va_end(args);

  WriteByteStreamTo_UART(stUART_ReceiverObj, (const uint8_t*)print_buffer, len);
#endif
}
