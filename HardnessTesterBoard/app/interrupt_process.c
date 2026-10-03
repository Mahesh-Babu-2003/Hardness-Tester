#include "application.h"

#define MINIMUM_SENSE_DETECTION_TIME 	2
#define MINIMUM_MOMNETUM_MOVEMENT_TIME 100
uint32_t start_time = 0, execution_time = 0, movingPulseCount = 0;

void process_sensors_functionality() {

#if(0)
	if( get_controller_input(HOME_INT_IP)){
		if(live_obj.home_sense_detected == true){
			live_obj.home_sense_detection_delay = setDelayInMSecs(MINIMUM_SENSE_DETECTION_TIME);
			getTravelPulseCount() = 0;
			live_obj.home_sense_not_found = false;
		}else if(isDelayActive(live_obj.home_sense_detection_delay) == false){
			if(live_obj.home_sense_detected == false){
				live_obj.movement_delay = setDelayInMSecs(MINIMUM_MOMNETUM_MOVEMENT_TIME);
			}
			live_obj.home_sense_detected = true;
		}
	}else{
#if(0)
		if(live_obj.home_sense_detected == false){
			live_obj.home_sense_detection_delay = setDelayInMSecs(MINIMUM_SENSE_DETECTION_TIME);
		}else if(isDelayActive(live_obj.home_sense_detection_delay) == false){
			live_obj.home_sense_detected = false;
		}
#else
		live_obj.home_sense_detected = false;
#endif
	}

	if(get_controller_input(DOP_INT_IP)){
		if(live_obj.dop_sense_detected == true){
			live_obj.dop_sense_detection_delay = setDelayInMSecs(MINIMUM_SENSE_DETECTION_TIME);
		}else if(isDelayActive(live_obj.dop_sense_detection_delay) == false){
			live_obj.dop_sense_detected = true;
		}
	}else{
		if(live_obj.dop_sense_detected == false){
			live_obj.dop_sense_detection_delay = setDelayInMSecs(MINIMUM_SENSE_DETECTION_TIME);
		}else if(isDelayActive(live_obj.dop_sense_detection_delay) == false){
			live_obj.dop_sense_detected = false;
		}
	}
#if(ENABLE_END_SENSE == 0)
	if(get_controller_input(END_INT_IP)){
		if(live_obj.end_sense_detected == true){
			live_obj.end_sense_detection_delay = setDelayInMSecs(MINIMUM_SENSE_DETECTION_TIME);
		}else if(isDelayActive(live_obj.end_sense_detection_delay) == false){
			if(live_obj.end_sense_detected == false){
				live_obj.movement_delay = setDelayInMSecs(MINIMUM_MOMNETUM_MOVEMENT_TIME);
			}
			live_obj.end_sense_detected = true;
		}
	}else{
#if(0)
		if(live_obj.end_sense_detected == false){
			live_obj.end_sense_detection_delay = setDelayInMSecs(MINIMUM_SENSE_DETECTION_TIME);
		}else if(isDelayActive(live_obj.end_sense_detection_delay) == false){
			live_obj.end_sense_detected = false;
		}
#else
		live_obj.end_sense_detected = false;
#endif
	}
#endif
#else
	live_obj.dop_sense_detected = get_controller_input(DOP_INT_IP);
	live_obj.end_sense_detected = get_controller_input(END_INT_IP);
	if((live_obj.homeDetectionCount > 2 &&
			live_obj.home_sense_detected != get_controller_input(HOME_INT_IP))
			|| (live_obj.home_sense_detected == get_controller_input(HOME_INT_IP))){
		live_obj.home_sense_detected = get_controller_input(HOME_INT_IP);
		live_obj.homeDetectionCount = 0;
		if(live_obj.home_sense_detected == true){
			live_obj.travelPulseCount = 0;
			live_obj.forwardCount = 0;
			live_obj.home_sense_not_found = false;
		}
	}else{
		++live_obj.homeDetectionCount;
	}
#endif

	if (live_obj.movement_direction == BACKWARD_DIRECTION
			&& live_obj.home_sense_detected) {
		//go to pause state
		set_motor_movement(NO_DIRECTION);
	} else if (live_obj.movement_direction == FORWARD_DIRECTION
			&& live_obj.end_sense_detected) {
		//go to pause state
		set_motor_movement(NO_DIRECTION);
	} else if (live_obj.dop_sense_detected) {
	}
}

void process_rpm_pulses() {
	if (live_obj.resetPulseCount) {
		live_obj.resetPulseCount = false;
		live_obj.errorPulseCount = 0;
		live_obj.movingPulseCount = 0;
	}
	++live_obj.movingPulseCount;
	if ((live_obj.movement_direction == BACKWARD_DIRECTION)
			&& getTravelPulseCount() > 0) {
		--live_obj.travelPulseCount;
	} else if (live_obj.movement_direction == FORWARD_DIRECTION) {
		++live_obj.travelPulseCount;
		++live_obj.forwardCount;

	}
	live_obj.position_sense_not_found = false;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
		live_obj.pulseGapTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3); // clear CC flag
		__HAL_TIM_SET_COUNTER(htim, 0);   // RESET counter
		process_sensors_functionality();
		if (live_obj.pulseGapTime >= 3) {
			process_rpm_pulses();
		}else{
			++live_obj.errorPulseCount;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == HOME_IP_Pin) {
		process_sensors_functionality();
	} else if (GPIO_Pin == END_IP_Pin) {
		process_sensors_functionality();
	}
//	else if(GPIO_Pin == RPM_INT_Pin)
//	{
//		if(get_controller_input(RPM_INT_IP))
//		{
//			process_rpm_pulses();
//		}
//	}
	else if (GPIO_Pin == DOP_SW_IP_Pin) {
		process_sensors_functionality();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &PERIODIC_TIMER) {
		tick_count += PERIODIC_TIMER_INTERRUPT_INTERVAL;

		process_sensors_functionality();

//		else if(live_obj.movement_direction == NO_DIRECTION){
//			set_motor_movement(NO_DIRECTION);
//		}

	}
}
