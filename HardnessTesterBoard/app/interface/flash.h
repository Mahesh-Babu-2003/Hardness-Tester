/*
 * flash.h
 *
 *  Created on: 08-May-2022
 *      Author: AJ
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define FLASH_SYNC_FAILURE_ATTEMPTS							2
#define FLASH_SIZE									((uint32_t)(FLASH_BANK1_END - FLASH_BASE + 1UL))
#define FLASH_USER_ADDRESS_START					((uint32_t)FLASH_BASE + FLASH_SIZE -( (uint32_t)FLASH_PAGE_SIZE * 2))

#define FLASH_CONFIGUARATION_ADDRESS				FLASH_USER_ADDRESS_START
#define FLASH_CONFIGUARATION_BKUP_ADDRESS			(FLASH_CONFIGUARATION_ADDRESS + FLASH_PAGE_SIZE)

//#define FLASH_READING_ADDRESS						(FLASH_CONFIGUARATION_BKUP_ADDRESS + FLASH_PAGE_SIZE)
//#define FLASH_READING_BKUP_ADDRESS					(FLASH_READING_ADDRESS + FLASH_PAGE_SIZE)

HAL_StatusTypeDef LoadDataFromFlashPage(uint32_t address, uint8_t* bufferPtr, uint16_t bufLen);
HAL_StatusTypeDef SaveDataOnFlashPage(uint32_t address, uint8_t* bufferPtr, uint16_t bufLen);
HAL_StatusTypeDef EraseFlashPage(uint32_t address);

uint8_t saveDataToFlash_WithBkup(uint8_t* data, uint16_t size_of_data, uint32_t address, uint32_t bkup_address);
uint8_t loadDataFromFlash_WithBkup(uint8_t* data, uint16_t size_of_data, uint32_t address, uint32_t bkup_address);
#endif /* FLASH_H_ */
