/*
 * hal.h
 *
 *  Created on: 17-Feb-2023
 *      Author: AJ
 */

#ifndef APPLICATION_HAL_H_
#define APPLICATION_HAL_H_
#include "stm_common.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "tim.h"
#include "rtc.h"
#include "delaytimer.h"

#define PERIODIC_TIMER												htim7
#define PERIODIC_TIMER_INTERRUPT_INTERVAL							1


#define STEPPER_MOTOR_PWM_TIMER										htim14
#define STEPPER_MOTOR_PWM_PULSE_PIN									TIM_CHANNEL_1

#define COMMUNICATION_USART   				huart4
#define LIMES_USART   						huart2

typedef enum{
	WAITING_FOR_OP_COMPLETION = 0xFF, OP_COMPLETED = 0, OP_TIMEOUT = 1, OP_OVER_RUN = 2,
	OP_COMM_ERROR = 3, OP_ERROR = 4, OP_HW_NOT_FOUND = 5, OP_MEMORY_ERROR = 6
}OPERATION_STATE;

typedef enum{
	COMMUNICATION_UART = 0, MAX_UART_COMMUNICATION
}SYSTEM_UART;


typedef enum{
	RTC_I2C_COMMUNICATION, EEPROM_I2C_COMMUNICATION, MAX_I2C_COMMUNICATION
}I2C_COMMUNICATION;

#define BOARD_VERSION_V_0				0
#define BOARD_VERSION					BOARD_VERSION_V_0

typedef enum{
	HOME_INT_IP = 0, END_INT_IP, RPM_INT_IP, DOP_INT_IP,
	HX711_DATA_1_IP, HX711_DATA_2_IP,
	MAX_CONTROLLER_IP
}CONTROLLER_IP;

typedef enum{
	HX711_CLK_1_OP, HX711_CLK_2_OP,
	STEPPER_MOTOR_EN_OP, STEPPER_MOTOR_DIR_OP,
	MAX_CONTROLLER_OP
}CONTROLLER_OP;


#endif /* APPLICATION_HAL_H_ */
