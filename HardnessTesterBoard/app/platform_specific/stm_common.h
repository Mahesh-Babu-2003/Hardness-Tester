/*
 * stm_common.h
 *
 *  Created on: 07-Aug-2020
 *      Author: VGS TECHNOLOGIES
 */

#ifndef CUSTOM_LIBRARY_INC_STM_COMMON_H_
#define CUSTOM_LIBRARY_INC_STM_COMMON_H_
#include "common.h"
#include "main.h"


#define IS_BIT_ENABLE(val, ix)  ((val >> ix) & 0x01U)

bool read_digital_input(GPIO_TypeDef * gpio, uint16_t pin_num, bool activeState);
void write_digital_output(GPIO_TypeDef * gpio, uint16_t pin_num, bool status, bool activeState);

//
//void flash_initialize();
//void flash_deinitialize();
//
//void delayMicroseconds(uint32_t time) ;
//void RDP_enable(void);

#endif /* CUSTOM_LIBRARY_INC_STM_COMMON_H_ */
