#include "platform_specific.h"

void TIM6_Init(void) ;

void initialize_platform_specific(){
	TIM6_Init();
}

uint16_t getSystemFrequencyInMHz(){
	return SystemCoreClock / 1000000;
}
// Initialize TIM6 for microsecond delay
void TIM6_Init(void) {
    // Enable TIM6 clock
    __HAL_RCC_TIM6_CLK_ENABLE();

    // Configure TIM6
    TIM6->PSC = SystemCoreClock / 1000000 - 1;  // Prescaler for 1 us
    TIM6->ARR = 0xFFFF;  // Auto-reload value

    // Enable the update event
    TIM6->EGR = TIM_EGR_UG;

    // Start the timer
    TIM6->CR1 = TIM_CR1_CEN;
}

// Microsecond delay function
void __delay_us(uint32_t us) {
    TIM6->CNT = 0;  // Reset the timer counter

    while (TIM6->CNT < us) {
        // Wait for the desired amount of time
    }
}

time_t get_system_time(struct tm* localtime){
	volatile time_t t_of_day = 0;
	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef DateToUpdate = { 0 };
	if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) == HAL_OK
			&& HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) == HAL_OK) {
		localtime->tm_year = (APP_START_YEAR - RTC_BASE_YEAR) +  DateToUpdate.Year ;  // Year - 2000
		localtime->tm_mon = DateToUpdate.Month - 1;           // Month, where 0 = jan
		localtime->tm_mday = DateToUpdate.Date;          // Day of the month
		localtime->tm_hour = sTime.Hours;
		localtime->tm_min = sTime.Minutes;
		localtime->tm_sec = sTime.Seconds;
		localtime->tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
		t_of_day = mktime(localtime);
		//		t_of_day += 19800;
		//		t = *localtime(&t_of_day);
	}else{
		memset(localtime, 0, sizeof(struct tm));
	}
	return t_of_day;
}

void set_system_time(struct tm*  localtime){
	RTC_TimeTypeDef sTime = { 0 };
	RTC_DateTypeDef DateToUpdate = { 0 };
	sTime.Hours = localtime->tm_hour;
	sTime.Minutes = localtime->tm_min;
	sTime.Seconds = localtime->tm_sec;

	DateToUpdate.WeekDay =  localtime->tm_wday; //RTC_WEEKDAY_MONDAY;
	DateToUpdate.Date = localtime->tm_mday;
	DateToUpdate.Month = localtime->tm_mon + 1;
	if(localtime->tm_year >= 100){
		DateToUpdate.Year = localtime->tm_year - 100;
	}else{
		DateToUpdate.Year = localtime->tm_year;
	}

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//	application_delay(100);
	HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);
//	application_delay(100);
//	HAL_RTCEx_SetSecond_IT(&hrtc);
}

//void set_system_timestamp(time_t new_time_of_day){
//	struct tm t;
//	t = *localtime(&new_time_of_day);
//	set_system_time(&t);
//}

void set_controller_output(CONTROLLER_OP output, uint8_t status){
	switch(output){
	case HX711_CLK_1_OP:
	{
		write_digital_output(HX711_CLK_1_OP_GPIO_Port, HX711_CLK_1_OP_Pin, status, PIN_ACTIVE_HIGH);
	}
	break;
	case HX711_CLK_2_OP:
	{
		write_digital_output(HX711_CLK_2_OP_GPIO_Port, HX711_CLK_2_OP_Pin, status, PIN_ACTIVE_HIGH);
	}
	break;
	case STEPPER_MOTOR_EN_OP:
	{
		write_digital_output(STEPPER_EN_OP_GPIO_Port, STEPPER_EN_OP_Pin, status, PIN_ACTIVE_LOW);
	}
		break;
	case STEPPER_MOTOR_DIR_OP:
	{
		write_digital_output(STEPPER_DIR_OP_GPIO_Port, STEPPER_DIR_OP_Pin, status, PIN_ACTIVE_HIGH);
	}
		break;
	default:
		break;
	}
}

uint8_t get_controller_output(CONTROLLER_OP output){
	uint8_t status = false;
	switch(output){
	case HX711_CLK_1_OP:
	{
		status = read_digital_input(HX711_CLK_1_OP_GPIO_Port, HX711_CLK_1_OP_Pin, PIN_ACTIVE_HIGH);
	}
	break;
	case HX711_CLK_2_OP:
	{
		status = read_digital_input(HX711_CLK_2_OP_GPIO_Port, HX711_CLK_2_OP_Pin, PIN_ACTIVE_HIGH);
	}
	break;
	case STEPPER_MOTOR_EN_OP:
	{
		status = read_digital_input(STEPPER_EN_OP_GPIO_Port, STEPPER_EN_OP_Pin, PIN_ACTIVE_LOW);
	}
		break;
	case STEPPER_MOTOR_DIR_OP:
	{
		status = read_digital_input(STEPPER_DIR_OP_GPIO_Port, STEPPER_DIR_OP_Pin, PIN_ACTIVE_HIGH);
	}
		break;
	default:
		break;
	}
	return status;
}

uint8_t get_controller_input(CONTROLLER_IP input){
	uint8_t status = false;
	switch(input){
	case HOME_INT_IP:
	{
		status = read_digital_input(HOME_IP_GPIO_Port, HOME_IP_Pin, PIN_ACTIVE_HIGH);
	}
	break;
	case  END_INT_IP:
	{
		status = read_digital_input(END_IP_GPIO_Port, END_IP_Pin, PIN_ACTIVE_HIGH);
	}
	break;
	case  RPM_INT_IP:
	{
//		status = read_digital_input(RPM_INT_GPIO_Port, RPM_INT_Pin, PIN_ACTIVE_HIGH);
		status = read_digital_input(GPIOB, GPIO_PIN_0, PIN_ACTIVE_HIGH);
	}
	break;
	case  DOP_INT_IP:
	{
		status = read_digital_input(DOP_SW_IP_GPIO_Port, DOP_SW_IP_Pin, PIN_ACTIVE_LOW);
	}
	break;
	case  HX711_DATA_1_IP:
	{
		status = read_digital_input(HX711_DATA_1_IP_GPIO_Port, HX711_DATA_1_IP_Pin, PIN_ACTIVE_HIGH);
	}
	break;
	case  HX711_DATA_2_IP:
	{
		status = read_digital_input(HX711_DATA_2_IP_GPIO_Port, HX711_DATA_2_IP_Pin, PIN_ACTIVE_HIGH);
	}
	break;
	default:
		break;
	}
	return status;
}

void application_delay(uint32_t delay){
	HAL_Delay(delay);
}
