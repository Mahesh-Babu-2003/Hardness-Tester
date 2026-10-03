#include "stm_common.h"

volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004; //address of the register
volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000; //address of the register
volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC; //address of the register

bool read_digital_input(GPIO_TypeDef * gpio, uint16_t pin_num, bool activeState){
	if(activeState == PIN_ACTIVE_LOW){
		return !HAL_GPIO_ReadPin(gpio, pin_num);
	}else{
		return HAL_GPIO_ReadPin(gpio, pin_num);
	}
}

void write_digital_output(GPIO_TypeDef * gpio, uint16_t pin_num, bool status, bool activeState){
	if(activeState == PIN_ACTIVE_LOW){
		HAL_GPIO_WritePin(gpio, pin_num, !status);
	}else{
		HAL_GPIO_WritePin(gpio, pin_num, status);
	}
}
//
//void RDP_enable(void)
//{
////	FLASH_OBProgramInitTypeDef OBInit;
////	HAL_FLASHEx_OBGetConfig( (FLASH_OBProgramInitTypeDef *)&OBInit );
//
////	if (OBInit.RDPLevel != OB_RDP_LEVEL_1)
////	{
////		OBInit.OptionType = OPTIONBYTE_RDP;
////		OBInit.RDPLevel = OB_RDP_LEVEL_1;
////		HAL_FLASH_OB_Unlock();
////		HAL_FLASHEx_OBProgram( (FLASH_OBProgramInitTypeDef *)&OBInit );
////		HAL_FLASH_OB_Lock();
////		HAL_FLASH_OB_Launch();
////	}
//  FLASH_OBProgramInitTypeDef Optbyte;
//
//  __disable_irq();
//  HAL_FLASHEx_OBGetConfig(&Optbyte);   	// read out RDPLvL
//  if(Optbyte.RDPLevel == OB_RDP_LEVEL_0){   //
//    Optbyte.OptionType=OPTIONBYTE_RDP; 	// select RDP optionbyte
//    Optbyte.RDPLevel=OB_RDP_LEVEL_1;   	// select RDP level 1
//    HAL_FLASH_Unlock();                	// unlock Flash
//    HAL_FLASH_OB_Unlock();             	// unlock Optionbytes
//    HAL_FLASHEx_OBProgram(&Optbyte);   	// set RDP=1
//    HAL_FLASH_OB_Lock();                // Lock Optionbytes
//    HAL_FLASH_Lock();                   // lock Flash
//    HAL_FLASH_OB_Launch();             	// write OB to Flash and reset
//  }
//  __enable_irq();
//}
//
//
//void flash_initialize()
//{
//	HAL_FLASH_Unlock();
//}
//
//void flash_deinitialize()
//{
//	HAL_FLASH_Lock();
//}
//
//
//void enable_Usec_Timeing(){
//	//CYCCNT
//	*SCB_DEMCR = *SCB_DEMCR | 0x01000000;
//
//	*DWT_CYCCNT = 0; // reset the counter
//	*DWT_CONTROL = *DWT_CONTROL | DWT_CTRL_CYCCNTENA_Msk ; // enable the counter
//      //DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//}
//
//void Timing_Usec_Delay(unsigned int usec){
//	uint32_t number_of_ticks_per_us = SystemCoreClock/1000000U;
//	uint32_t total_ticks = usec * number_of_ticks_per_us;
//
//	*DWT_CONTROL = *DWT_CONTROL | DWT_CTRL_CYCCNTENA_Msk ; // enable the counter
//	*DWT_CYCCNT = 0; // reset the counter
//	 do{
//	 }while(*DWT_CYCCNT < total_ticks);
//	*DWT_CONTROL = *DWT_CONTROL & ~(DWT_CTRL_CYCCNTENA_Msk) ; // disable the counter
//
//}
//
//void enableUsecCounter(){
//		DWT->CYCCNT = 0; // reset the counter
//		DWT->CTRL = DWT->CTRL | DWT_CTRL_CYCCNTENA_Msk ; // enable the counter
//}
//
//uint32_t readUsecCounter(){
//	return DWT->CYCCNT;
//}
//
//void disableUsecCounter(){
//	DWT->CTRL = DWT->CTRL & ~(DWT_CTRL_CYCCNTENA_Msk) ;  // disable the counter
//}
//
//bool isTimerTimeOut(uint32_t delay_start_time, uint32_t delay_in_msec){
//	bool result = false;
//	if(getDiffrence(delay_start_time, HAL_GetTick()) >= delay_in_msec){
//		result = true;
//	}
//	return result;
//}
//
//
//void delayMicroseconds(uint32_t time)  //Time delay function - Arbitrary delay
//{
//	time = time * (SystemCoreClock/1000000);
//	time = time/2;
//	while(  time-- > 0);
//	//{__NOP();}
//}
