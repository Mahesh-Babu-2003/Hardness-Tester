/*
 * platform_specific.h
 *
 *  Created on: 11-Mar-2023
 *      Author: AJ
 */

#ifndef PLATFORM_SPECIFIC_PLATFORM_SPECIFIC_H_
#define PLATFORM_SPECIFIC_PLATFORM_SPECIFIC_H_
#include "hal.h"
#define NOP()			asm("NOP")

void initialize_platform_specific();
uint16_t getSystemFrequencyInMHz();

time_t get_system_time(struct tm* localtime);
void set_system_time(struct tm*  localtime);

void __delay_us(uint32_t us);

void set_controller_output(CONTROLLER_OP output, uint8_t status);
uint8_t get_controller_output(CONTROLLER_OP output);

uint8_t get_controller_input(CONTROLLER_IP input);

void application_delay(uint32_t);

#endif /* PLATFORM_SPECIFIC_PLATFORM_SPECIFIC_H_ */
