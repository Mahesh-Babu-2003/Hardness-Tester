#include "application.h"
#include "scpi.h"
#include "adc.h"
#include "math.h"

ApplicationLiveStatus live_obj;
THT_Configuration configurationObj;


#define COMMUNICATION_USART_BUFFER_SIZE			2048
uint8_t communication_usart_buffer[COMMUNICATION_USART_BUFFER_SIZE];
float Temperature = 0.0f;


uint16_t ADC_Read(void)
{
    uint16_t value = 0;

    HAL_ADC_Start(&hadc);

    if(HAL_ADC_PollForConversion(&hadc,10) == HAL_OK)
    {
        value = HAL_ADC_GetValue(&hadc);
    }

    HAL_ADC_Stop(&hadc);

    return value;
}

float GetTemperature(void)
{
    uint16_t adc;
    float resistance;
    float temperature;

    adc = ADC_Read();

    if(adc == 0)
        return -100.0f;

    resistance = SERIES_RESISTOR * ((ADC_MAX_VALUE / (float)adc) - 1.0f);

    temperature = logf(resistance / NTC_R25);
    temperature /= NTC_BETA;
    temperature += (1.0f / 298.15f);
    temperature = 1.0f / temperature;
    temperature -= 273.15f;

    return temperature;
}
void CoolingFanControl(void)
{
    Temperature = GetTemperature();

    if(Temperature >= FAN_ON_TEMP)
    {
        HAL_GPIO_WritePin(COLLING_FAN_GPIO_Port,
                          COLLING_FAN_Pin,
                          GPIO_PIN_SET);
    }
    else if(Temperature <= FAN_OFF_TEMP)
    {
        HAL_GPIO_WritePin(COLLING_FAN_GPIO_Port,
                          COLLING_FAN_Pin,
                          GPIO_PIN_RESET);
    }
}

void initialization_application(){

//	fill_default_configuration();
	load_configuration();
	live_obj.requiredSpeedRotationTime = configurationObj.fastSpeedRotationTime;
	live_obj.lastTravelPulseCount = 0;

	initialize_platform_specific();

	HAL_TIM_Base_Start_IT(&PERIODIC_TIMER);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);

	set_controller_output(STEPPER_MOTOR_EN_OP, false);
	setSlowRequiredDistance(0);
	set_motor_movement(NO_DIRECTION);

	InitializeUART_Receiver(&gstUART_TransceiverList[COMMUNICATION_UART],
			&COMMUNICATION_USART, communication_usart_buffer,
			COMMUNICATION_USART_BUFFER_SIZE);
	StartUART_Receiver(&gstUART_TransceiverList[COMMUNICATION_UART]);

	init_weight(WH_CHANNEL_1, &live_obj.hx711[WH_CHANNEL_1], configurationObj.scaleFactor[WH_CHANNEL_1]);
	init_weight(WH_CHANNEL_2, &live_obj.hx711[WH_CHANNEL_2], configurationObj.scaleFactor[WH_CHANNEL_2]);

//	HAL_UART_Transmit(&LIMES_USART, "Start AJ\r\n", 10, 10);
}



void check_usb_communication() {
	check_uart_receiver_errors(&gstUART_TransceiverList[COMMUNICATION_UART]);

	if (gstUART_TransceiverList[COMMUNICATION_UART].huart->hdmatx->State == HAL_DMA_STATE_READY){
		if (IsUART_ReceiverIsEmpty(
				&gstUART_TransceiverList[COMMUNICATION_UART]) == false) {
			do {
				uint8_t status = process_usb_communication_byte(
						ReadByteFrom_UART_Receiver(
								&gstUART_TransceiverList[COMMUNICATION_UART]));
	//			if (status == false && IsUART_ReceiverIsEmpty(
	//					&gstUART_TransceiverList[COMMUNICATION_UART])) {
	//				application_delay(1);
	//				application_delay(1);
	//			}
			} while (IsUART_ReceiverIsEmpty(
					&gstUART_TransceiverList[COMMUNICATION_UART]) == false);
		}
	}
}


void process_application(){

	check_usb_communication();
	CoolingFanControl();

	if(isDelayActive(live_obj.pkt_delay) == false){
		//		SCPI_Send("Start AJ\r\n");
		live_obj.pkt_delay = setDelayInMSecs(1000);
	}

	if(live_obj.operation != OP_NO_STATE){

		resetTestStatus();
		switch(live_obj.operation){
		case OP_GO_TO_HOME:
			waitForMotorHomePosition();
//			if(live_obj.home_sense_detected){
//				tare_weight(&live_obj.hx711[WH_CHANNEL_1]);
//				tare_weight(&live_obj.hx711[WH_CHANNEL_2]);
//			}
			scpi_get_status(NULL);
			break;
		case OP_GO_TO_END:
			waitForMotorEndPosition();
			scpi_get_status(NULL);
			break;
		case OP_TARE:
			tare_weight(&live_obj.hx711[WH_CHANNEL_1]);
			tare_weight(&live_obj.hx711[WH_CHANNEL_2]);
			break;
		case OP_INIT_WEIGHT:
			init_weight(WH_CHANNEL_1, &live_obj.hx711[WH_CHANNEL_1], configurationObj.scaleFactor[WH_CHANNEL_1]);
			init_weight(WH_CHANNEL_2, &live_obj.hx711[WH_CHANNEL_2], configurationObj.scaleFactor[WH_CHANNEL_2]);
			break;
		case OP_RST_WEIGHT:
			reset_weight(&live_obj.hx711[WH_CHANNEL_1]);
			reset_weight(&live_obj.hx711[WH_CHANNEL_2]);
			break;
		case OP_MEASURE_WEIGHT:{

			live_obj.live_force = measure_weight(&live_obj.hx711[WH_CHANNEL_1]);
			live_obj.calibration_force = measure_weight(&live_obj.hx711[WH_CHANNEL_2]);
			SCPI_Printf("MEASURE:WEIGHT %d,%d\r\n", live_obj.live_force, live_obj.calibration_force);
			application_delay(10);
			live_obj.operation = OP_NO_STATE;
			scpi_get_status(NULL);
			ReleaseLiveForce();
		}
			break;
		case OP_MAINTAIN_FORCE:
			live_obj.lastStatus = maintain_weight();
			SCPI_Printf("MAINTAIN:FORCE %d, %d, %d\r\n",
					live_obj.lastStatus, (int)live_obj.lastLive_force,
					(int)live_obj.lastCalibration_force);
			application_delay(10);
			scpi_get_status(NULL);
			break;
		case OP_MEASURE_WIDTH:{
			live_obj.lastStatus = processTabletTest(&live_obj.lastMeasuredValue,
					&live_obj.lastRemainingDistance, &live_obj.lastMeasuredForce, WIDTH_TEST, 0);
			SCPI_Printf("MEASURE:WIDTH %d, %d, %d, %d, %d\r\n",
					live_obj.lastStatus, (int)live_obj.lastMeasuredValue,
					(int)live_obj.lastRemainingDistance, (int)live_obj.lastMeasuredForce,
					(int)live_obj.lastTravelPulseCount
					);
			application_delay(10);
			scpi_get_status(NULL);
		}
			break;
		case OP_MEASURE_FORCE:{
			live_obj.lastStatus = processTabletTest(&live_obj.lastMeasuredValue,
					&live_obj.lastRemainingDistance, &live_obj.lastMeasuredForce, HARDNESS_TEST, 0);
			SCPI_Printf("MEASURE:FORCE %d, %d, %d, %d, %d\r\n",
					live_obj.lastStatus, (int)live_obj.lastMeasuredValue,
					(int)live_obj.lastRemainingDistance, (int)live_obj.lastMeasuredForce,
					(int)getTravelPulseCount());
			application_delay(10);
			scpi_get_status(NULL);
		}
			break;
		case OP_SELF_TEST_LOAD_CELL:{
			process_loadcell_sensor_selftest();
		}
			break;
		case OP_SELF_TEST_MOTOR:{
			process_motor_selftest ();
		}
			break;
		case OP_SAVE_CONFIGURATION:{
			save_application_configuration();
		}
		default:
			break;
		}
		resetTestStatus();
		live_obj.operation = OP_NO_STATE;
	}else{
		if(live_obj.movement_direction != NO_DIRECTION){
			set_motor_movement(live_obj.movement_direction);
		}else{
			live_obj.live_force = measure_weight(&live_obj.hx711[WH_CHANNEL_1]);
			live_obj.calibration_force = measure_weight(&live_obj.hx711[WH_CHANNEL_2]);
		}
	}
}
