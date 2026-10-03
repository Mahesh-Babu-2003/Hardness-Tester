/*
 * scpi.h
 *
 *  Created on: Dec 15, 2025
 *      Author: aj
 */

#ifndef COMMUNICATION_SCPI_H_
#define COMMUNICATION_SCPI_H_
#include "application.h"

#include "serial_api.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define COMMAND_START_BYTE 				'*'
#define COMMAND_END_BYTE_1 				'\r'
#define COMMAND_END_BYTE_2 				'\n'
#define COMMAND_DATA_SEPARATOR_BYTE 	':'
#define DATA_SEPARATOR_BYTE  			','

#define PKT_TO_PKT_DELAY					5
#define COMMAND_MAX_LENGTH					16
#define COMMAND_MIN_LEN						3 //(START_BYTE+DATA+END_BYTE)
#define COMMUNCATION_BUFFER_LENGTH			512

typedef enum {
	COMMUNICATION_RESPOSE_NOT_REQUIRED = -1,
	COMMUNICATION_OK = 0,
	COMMUNICATION_INVALID_LENGTH,
	COMMUNICATION_INVALID_COMMAND,
	COMMUNICATION_INVALID_VALUE
} USB_COMM_ERROR_CODES;

extern const char SUCCESSFUL_CMD[];
extern const char ERROR_CMD[];

#define USB_COMM_UART					COMMUNICATION_UART


void SCPI_Parse(char *input);
void SCPI_Send(const char *msg);
void SCPI_Printf(const char *format, ... );

uint8_t process_usb_communication_byte(uint8_t byte);
void scpi_get_status(char *args);

#endif /* COMMUNICATION_SCPI_H_ */
