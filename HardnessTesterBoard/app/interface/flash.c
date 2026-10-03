#include "stm_common.h"
#include "flash.h"


//static uint32_t GetBank(uint32_t Addr)
//{
//  uint32_t bank = 0;
//
//#ifndef FLASH_BANK_2
//  bank = FLASH_BANK_1;
//#else
//  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
//  {
//  	/* No Bank swap */
//    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
//    {
//      bank = FLASH_BANK_1;
//    }
//    else
//    {
//      bank = FLASH_BANK_2;
//    }
//  }
//  else
//  {
//  	/* Bank swap */
//    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
//    {
//      bank = FLASH_BANK_2;
//    }
//    else
//    {
//      bank = FLASH_BANK_1;
//    }
//  }
//#endif
//
//  return bank;
//}
//
//static uint32_t GetPage(uint32_t Addr)
//{
//  uint32_t page = 0;
//
//  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
//  {
//    /* Bank 1 */
//    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
//  }
//  else
//  {
//    /* Bank 2 */
//    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
//  }
//
//  return page;
//}

void ClearFlashErrors(){
	 /* Clear OPTVERR bit set on virgin samples */
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	  __HAL_FLASH_CLEAR_FLAG(FLASH_SR_PGERR);
	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PROGERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGAERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_SIZERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_MISERR);
//	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_FASTERR);
}

HAL_StatusTypeDef EraseFlashPages(uint32_t address, uint8_t NbOfPages){
	HAL_StatusTypeDef result = HAL_OK;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;

//	 /* Unlock the Flash to enable the flash control register access *************/
//	  //__disable_irq();            // disable all interrupts
//	  HAL_FLASH_Unlock();
//	 /* Clear OPTVERR bit set on virgin samples */
//	  ClearFlashErrors();
//
//	  /* Erase the user Flash area
////	    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
//	  /* Fill EraseInit structure*/
//	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
//	EraseInitStruct.Banks = GetBank(address);
//	EraseInitStruct.Page = GetPage(address);
//	EraseInitStruct.NbPages = NbOfPages;
//
//	  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
//	     you have to make sure that these data are rewritten before they are accessed during code
//	     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
//	     DCRST and ICRST bits in the FLASH_CR register. */
//	  result = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
//	  PageError = HAL_FLASH_GetError();
//
//	  /* Lock the Flash to disable the flash control register access (recommended
//	     to protect the FLASH memory against possible unwanted operation) *********/
//	  HAL_FLASH_Lock();
//	    //__enable_irq();            // re-enable interrupts


	__disable_irq();
	HAL_FLASH_Unlock();
	ClearFlashErrors();

	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = address;
	EraseInitStruct.NbPages = NbOfPages; //(FLASH_USER_END_ADDR - FLASH_USER_START_ADDR)/FLASH_PAGE_SIZE;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

	HAL_FLASH_Lock();
	__enable_irq();
	  return result;
}

HAL_StatusTypeDef EraseFlashPage(uint32_t address){
	return EraseFlashPages(address, 1);
}

HAL_StatusTypeDef SaveDataOnFlashPage(uint32_t address, uint8_t* bufferPtr, uint16_t bufLen){
	HAL_StatusTypeDef result = HAL_OK;

	  EraseFlashPage(address);
	  __disable_irq();            // disable all interrupts
	  HAL_FLASH_Unlock();
	  ClearFlashErrors();

	do{
		uint64_t data64ToProg;
		uint16_t writeByteCount  = bufLen < sizeof(data64ToProg)? bufLen: sizeof(data64ToProg);
		memcpy((uint8_t*)&data64ToProg, bufferPtr, writeByteCount);
		result = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data64ToProg);
		if(result == HAL_OK){
			address += sizeof(data64ToProg);
			bufferPtr += sizeof(data64ToProg);
			bufLen -= writeByteCount;

		}
	}while(result == HAL_OK && bufLen > 0);

	  HAL_FLASH_Lock();
	  __enable_irq();            // re-enable interrupts
	return result;
}


HAL_StatusTypeDef LoadDataFromFlashPage(uint32_t address, uint8_t* bufferPtr, uint16_t bufLen){
	HAL_StatusTypeDef result = HAL_OK;
	if((address < (FLASH_BASE + FLASH_SIZE)) && address >= FLASH_BASE){
		memcpy(bufferPtr, (uint8_t*)address, bufLen);
	}else{
		result = HAL_ERROR;
	}
	return result;
}



uint8_t loadDataFromFlash_WithBkup(uint8_t* data, uint16_t size_of_data, uint32_t address, uint32_t bkup_address){
	return (((LoadDataFromFlashPage(address, data, size_of_data) == HAL_OK)
				&& validate_crc((uint8_t*)data, size_of_data))
				|| ((LoadDataFromFlashPage(bkup_address, data, size_of_data) == HAL_OK)
						&& validate_crc(data, size_of_data)));
}


uint8_t saveDataToFlash_WithBkup(uint8_t* data, uint16_t size_of_data, uint32_t address, uint32_t bkup_address){
	uint8_t result= false;
	uint8_t attempts = FLASH_SYNC_FAILURE_ATTEMPTS;
	calculate_and_fill_crc(data, size_of_data);
	while (result == false && attempts > 0) {
		result = SaveDataOnFlashPage(address, data, size_of_data) == HAL_OK;
		if (result) {
			result = SaveDataOnFlashPage(bkup_address, data, size_of_data) == HAL_OK;
		}

		if (result == false && attempts > 0) {
			--attempts;
		}
	}
	return result;
}
