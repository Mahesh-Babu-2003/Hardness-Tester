#include "application.h"
#include "moving_average.h"
uint32_t finalWeight = 0, start_time_loop;
uint32_t movementTimeLoop = 0;
int32_t presentWeight = 0;

#define TRAVEL_SLOW_REQUIRED_DISTANCE	24000
#define TRAVEL_DELTA_DISTANCE	4000
uint32_t currentSlowRequiredDistance = 0;

uint8_t getHomePositionStatus() {
	return live_obj.home_sense_detected;
}

uint8_t getEndPositionStatus() {
	return live_obj.end_sense_detected;
}

uint32_t getMaximumTravelDistance() {
	return configurationObj.home_to_end_distance;
}
uint32_t getTravelPulseCount(){
	return live_obj.travelPulseCount + get_controller_input(RPM_INT_IP);
}
uint32_t getTravelDistance() {
	uint32_t distance = (configurationObj.home_to_end_distance
			* getTravelPulseCount())
							/ configurationObj.home_to_end_pulseCount;
	return distance; // <= configurationObj.home_to_end_distance ? distance:configurationObj.home_to_end_distance;
}


void setSlowRequiredDistance(uint32_t distance) {
	currentSlowRequiredDistance = distance;
}

uint32_t getRemainingDistance() {
	uint32_t reaminingDistance = 0;
	if (getMaximumTravelDistance() > getTravelDistance()) {
		reaminingDistance = getMaximumTravelDistance() - getTravelDistance();
	}
	return reaminingDistance;
}

uint32_t getFastAllwedDistance() {
	uint32_t fastAllowedDistance = getMaximumTravelDistance();

	while (fastAllowedDistance > TRAVEL_SLOW_REQUIRED_DISTANCE) {
		fastAllowedDistance -= 1000;
	}
	if ((currentSlowRequiredDistance > 0)
			&& (live_obj.state == PROGRAM_RUN_STATE
					|| live_obj.state == TEST_RUN_STATE)) {
		uint32_t slowRequiredDistance = currentSlowRequiredDistance
				+ TRAVEL_DELTA_DISTANCE;

		while (fastAllowedDistance > slowRequiredDistance) {
			fastAllowedDistance -= 1000;
		}
	}

	return fastAllowedDistance;
}

void setMotorSpeed(uint32_t timePeriodInMsec, MOVEMENT_DIRECTION direction) {


	uint32_t frequencyInMHz = getSystemFrequencyInMHz();
	//multiply by 32 because of frequency, 6400 is motor per rotation required pulses
	uint32_t timePeriod = (timePeriodInMsec * 1000 * frequencyInMHz) / 6400;
	if (timePeriod < frequencyInMHz) {
		timePeriod = frequencyInMHz;
	}

	STEPPER_MOTOR_PWM_TIMER.Instance->ARR = timePeriod - 1;

	TIM_OC_InitTypeDef sConfigOC = { 0 };
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = (90 * timePeriod) / 100; //timePeriod - frequencyInMHz;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&STEPPER_MOTOR_PWM_TIMER, &sConfigOC,
			STEPPER_MOTOR_PWM_PULSE_PIN) != HAL_OK) {
		//				Error_Handler();
	}

	set_controller_output(STEPPER_MOTOR_DIR_OP,
			direction == BACKWARD_DIRECTION);
	set_controller_output(STEPPER_MOTOR_EN_OP, true);
	HAL_TIM_PWM_Start(&STEPPER_MOTOR_PWM_TIMER, STEPPER_MOTOR_PWM_PULSE_PIN);
	//	computeExpectedPulseTime(rpm);
	live_obj.movement_direction = direction;
	live_obj.eachRotationTimePeriod = timePeriodInMsec;
}

void set_motor_movement(MOVEMENT_DIRECTION direction) {

	uint32_t currentMaximumForce = 0 ;
	if(direction != NO_DIRECTION){
		if(
				is_ready(&live_obj.hx711[WH_CHANNEL_1])){
			float live_force = measure_actual_weight(&live_obj.hx711[WH_CHANNEL_1]);
			if(live_force >= 0){
				live_obj.live_force = live_force;
			}
		}
		if(live_obj.operation == OP_MAINTAIN_FORCE){
			if (is_ready(&live_obj.hx711[WH_CHANNEL_2])){
				float calibration_force = measure_actual_weight(&live_obj.hx711[WH_CHANNEL_2]);
				if(calibration_force > 0){
					live_obj.calibration_force = calibration_force;
				}
			}
		}else{
			live_obj.calibration_force = 0;
		}
	}

	if(live_obj.live_force > live_obj.calibration_force){
		currentMaximumForce = live_obj.live_force;
	}else{
		currentMaximumForce = live_obj.calibration_force;
	}


	if (live_obj.movement_direction == direction
			&& direction != NO_DIRECTION) {
		set_controller_output(STEPPER_MOTOR_DIR_OP,
				direction == BACKWARD_DIRECTION);

		//		if(live_obj.movement_direction == BACKWARD_DIRECTION
		//				&& live_obj.movingPulseCount < DEFAULT_NUMBER_OF_PULSES_PER_ROTATION/2 ){
		//			 if (live_obj.eachRotationTimePeriod
		//							!= DEFAULT_STARTUP_SPEED_MINIMUM_ROTATION_TIME
		//							&& isDelayActive(live_obj.motor_startup_delay) == false){
		//					setMotorSpeed(DEFAULT_STARTUP_SPEED_MINIMUM_ROTATION_TIME, direction);
		//			 }
		//		}
		//		else
		if (live_obj.eachRotationTimePeriod
				== live_obj.requiredSpeedRotationTime) {
			if(currentMaximumForce > MINIMUM_SPEED_IF_BELOW_THIS_WEIGHT &&
					get_abs_diffrence(currentMaximumForce, MINIMUM_SPEED_IF_BELOW_THIS_WEIGHT) >= MINUM_WEIGHT_ALLOWED_DEVIATION){
				setMotorSpeed(configurationObj.slowSpeedRotationTime, direction);
			}
		}else{
			if(currentMaximumForce < MINIMUM_SPEED_IF_BELOW_THIS_WEIGHT
					&& isDelayActive(live_obj.motor_startup_delay) == false){
				setMotorSpeed(live_obj.requiredSpeedRotationTime, direction);
			}
		}

	} else {
		set_controller_output(STEPPER_MOTOR_EN_OP, false);
	}

	if (live_obj.movement_direction != direction
			|| (live_obj.movement_direction == NO_DIRECTION
					&& get_controller_output(STEPPER_MOTOR_EN_OP))) {

		set_controller_output(STEPPER_MOTOR_EN_OP, false);
		set_controller_output(STEPPER_MOTOR_DIR_OP,
				direction == BACKWARD_DIRECTION);
		HAL_TIM_PWM_Stop(&STEPPER_MOTOR_PWM_TIMER, STEPPER_MOTOR_PWM_PULSE_PIN);

		if (direction != NO_DIRECTION) {
			setMotorSpeed(configurationObj.slowSpeedRotationTime, direction);
			live_obj.movingPulseCount = 0;
			live_obj.resetPulseCount = true;
//			live_obj.travelPulseCount = 0;
			if(direction == BACKWARD_DIRECTION){
				live_obj.motor_startup_delay = setDelayInMSecs(2*DEFAULT_MOTOR_STARTUP_DELAY);
			}else{
				live_obj.forwardCount = 0;
				live_obj.motor_startup_delay = setDelayInMSecs(DEFAULT_MOTOR_STARTUP_DELAY);
			}
		}
		live_obj.movement_direction = direction;
	}
	live_obj.travelDistance = getTravelDistance();
}

uint8_t isTestCancelled() {
	return live_obj.is_test_canceled;
}

void resetTestStatus() {
	live_obj.is_test_canceled = false;
	live_obj.is_test_paused = false;
}

uint8_t waitForMotorHomePosition() {
	live_obj.motor_movement_fail = false;
	uint32_t movementStartTIme = HAL_GetTick();
	set_motor_movement(NO_DIRECTION);
	if ((getHomePositionStatus() == 0)) {

//		live_obj.requiredSpeedRotationTime = configurationObj.fastSpeedRotationTime/2;
		set_motor_movement(BACKWARD_DIRECTION);
		uint32_t travelPulseCount = live_obj.movingPulseCount;

		while ((getHomePositionStatus() == 0)
				&& getDiffrence(movementStartTIme, HAL_GetTick())
				<= BACK_MAXIMUM_MOVEMENT_PULSE_MISS_TIME) {
			if (get_abs_diffrence(travelPulseCount,
					live_obj.movingPulseCount) >= MINIMUM_PULSE_MOVEMENT_REQUIRED) {
				travelPulseCount = live_obj.movingPulseCount;
				movementStartTIme = HAL_GetTick();
			}
			set_motor_movement(BACKWARD_DIRECTION);
			if(is_ready(&live_obj.hx711[WH_CHANNEL_1])){
				live_obj.live_force = measure_weight(&live_obj.hx711[WH_CHANNEL_1]);
			}

			live_obj.travelDistance = getTravelDistance();
			if (
					isTestCancelled()
					|| live_obj.movement_direction != BACKWARD_DIRECTION) {
				set_motor_movement(NO_DIRECTION);
				break;
			}
			check_usb_communication();
			while(live_obj.is_test_paused && isTestCancelled() == false){
				check_usb_communication();
				set_motor_movement(NO_DIRECTION);
				travelPulseCount = live_obj.movingPulseCount;
				movementStartTIme = HAL_GetTick();
			}
		}

		if (getHomePositionStatus() == 0) {
			if (getDiffrence(movementStartTIme,
					HAL_GetTick()) > BACK_MAXIMUM_MOVEMENT_PULSE_MISS_TIME) {
				set_motor_movement(NO_DIRECTION);
				live_obj.motor_movement_fail = true;
			}
		}

//		live_obj.requiredSpeedRotationTime = configurationObj.fastSpeedRotationTime;
	}
	if(getHomePositionStatus() && live_obj.is_home_sense_detected_once == false){
		live_obj.is_home_sense_detected_once = true;
		tare_weight(&live_obj.hx711[WH_CHANNEL_1]);
		tare_weight(&live_obj.hx711[WH_CHANNEL_2]);
	}
	set_motor_movement(NO_DIRECTION);
	return getHomePositionStatus();
}

uint8_t waitForMotorEndPosition() {
	live_obj.motor_movement_fail = false;
	if ((getEndPositionStatus() == 0)) {

		uint32_t movementStartTIme = HAL_GetTick();
		set_motor_movement(FORWARD_DIRECTION);
		uint32_t travelPulseCount = live_obj.movingPulseCount;

		while ((getEndPositionStatus() == 0)
				&& getDiffrence(movementStartTIme, HAL_GetTick())
				<= MAXIMUM_MOVEMENT_PULSE_MISS_TIME) {
			if (get_abs_diffrence(travelPulseCount,
					live_obj.movingPulseCount) >= MINIMUM_PULSE_MOVEMENT_REQUIRED) {
				travelPulseCount = live_obj.movingPulseCount;
				movementStartTIme = HAL_GetTick();
			}
			set_motor_movement(FORWARD_DIRECTION);
			if(is_ready(&live_obj.hx711[WH_CHANNEL_1])){
				live_obj.live_force = measure_weight(&live_obj.hx711[WH_CHANNEL_1]);
			}
			live_obj.travelDistance = getTravelDistance();
			if ((live_obj.live_force != INVALID_LOAD_SENSE_VALUE
					&& (live_obj.live_force > END_DETECTION_FOECE_DETECTION))
					|| isTestCancelled()
					|| live_obj.movement_direction != FORWARD_DIRECTION) {
				set_motor_movement(NO_DIRECTION);
				break;
			}
			check_usb_communication();
			while(live_obj.is_test_paused && isTestCancelled() == false){
				check_usb_communication();
				set_motor_movement(NO_DIRECTION);
				travelPulseCount = live_obj.movingPulseCount;
				movementStartTIme = HAL_GetTick();
			}
		}

		if (getEndPositionStatus() == 0) {
			if (getDiffrence(movementStartTIme,
					HAL_GetTick()) > MAXIMUM_MOVEMENT_PULSE_MISS_TIME) {
				set_motor_movement(NO_DIRECTION);
				live_obj.motor_movement_fail = true;
			}
		}

	}
	set_motor_movement(NO_DIRECTION);
	return getEndPositionStatus();
}

TEST_STATUS maintain_weight(){

#define MAX_FORCE_ADJUST_MENT_TIME	1200000UL

	TEST_STATUS test_status = TEST_COMPLETED;
	set_motor_movement(NO_DIRECTION);

#define FORCE_ALLOWED_DEVEATION		1000
#define FORCE_MAINTAINANCE_TIME		1000
	live_obj.maintainance_time = 0;
	uint32_t maintainace_timeout	= 0;


	uint32_t travelPulseCount = live_obj.movingPulseCount;
	uint32_t movementStartTIme = HAL_GetTick();
	live_obj.executionTime = 0;
	live_obj.startTime = HAL_GetTick();
	set_motor_movement(NO_DIRECTION);


	while (isTestCancelled() == false){
		live_obj.executionTime = HAL_GetTick() - live_obj.startTime;
		live_obj.startTime = HAL_GetTick();

		if(live_obj.movement_direction == NO_DIRECTION){
			if(is_ready(&live_obj.hx711[WH_CHANNEL_1])){
				float live_force = measure_actual_weight(&live_obj.hx711[WH_CHANNEL_1]);
				if(live_force >= 0){
					live_obj.live_force = live_force;
				}
			}
			if (is_ready(&live_obj.hx711[WH_CHANNEL_2])){
				float calibration_force = measure_actual_weight(&live_obj.hx711[WH_CHANNEL_2]);
				if(calibration_force > 0){
					live_obj.calibration_force = calibration_force;
				}
			}
		}
		live_obj.travelDistance = getTravelDistance();

		check_usb_communication();
		maintainace_timeout += live_obj.executionTime;


		if (get_abs_diffrence(travelPulseCount,
				live_obj.movingPulseCount) >= MINIMUM_PULSE_MOVEMENT_REQUIRED) {
			travelPulseCount = live_obj.movingPulseCount;
			movementStartTIme = HAL_GetTick();
		}

		if( (get_abs_diffrence(live_obj.required_force, live_obj.live_force) < FORCE_ALLOWED_DEVEATION)
				&& ((live_obj.movement_direction == NO_DIRECTION)
						|| (live_obj.live_force >= live_obj.required_force))
				&&
				(live_obj.required_force > FORCE_ALLOWED_DEVEATION ||
						get_abs_diffrence(live_obj.required_force, live_obj.live_force) < FORCE_ALLOWED_DEVEATION)
		){

			set_motor_movement(NO_DIRECTION);
			configurationObj.slowSpeedRotationTime = DEFAULT_MAINTAIN_FORCE_SPEED_MINIMUM_ROTATION_TIME;

			live_obj.maintainance_time += live_obj.executionTime;

			if(live_obj.maintainance_time >= FORCE_MAINTAINANCE_TIME){
				test_status = TEST_COMPLETED;
				break;
			}

		}else{

			if(configurationObj.slowSpeedRotationTime == DEFAULT_MAINTAIN_FORCE_SPEED_MINIMUM_ROTATION_TIME
				&&	get_abs_diffrence(live_obj.required_force, live_obj.calibration_force) >= RECOVER_SLOW_SPPED_IF_FORCE_DIFF_ABOVE){
				configurationObj.slowSpeedRotationTime = DEFAULT_MAINTAIN_FORCE_SPEED_MINIMUM_ROTATION_TIME/2;
			}

			live_obj.maintainance_time = 0;
			if(getDiffrence(movementStartTIme, HAL_GetTick())
					>= (2*MAXIMUM_MOVEMENT_PULSE_MISS_TIME)
					&& (live_obj.calibration_force == 0
							&& live_obj.live_force == 0
					)){
				set_motor_movement(NO_DIRECTION);
				test_status = TEST_UNABLE_TO_MOVE;
				break;
			}

			if(maintainace_timeout >= MAX_FORCE_ADJUST_MENT_TIME){
				test_status = TEST_FAILED;
				break;
			}

			if(live_obj.calibration_force > live_obj.required_force){
				if(getHomePositionStatus()){
					set_motor_movement(NO_DIRECTION);
				}else{
					set_motor_movement(BACKWARD_DIRECTION);
				}
			}else{
				set_motor_movement(FORWARD_DIRECTION);
			}

		}
	}
	if(isTestCancelled() == true){
		test_status = TEST_CANCELED;
	}
	live_obj.lastLive_force = live_obj.live_force;
	live_obj.lastCalibration_force = live_obj.calibration_force;
	configurationObj.slowSpeedRotationTime = DEFAULT_SLOW_SPEED_MINIMUM_ROTATION_TIME;

	live_obj.travelDistance = getTravelDistance();
	set_motor_movement(NO_DIRECTION);
	return test_status;
}

void ReleaseLiveForce(){
	set_motor_movement(NO_DIRECTION);
	uint16_t waitDelay = 50;
	while(waitDelay > 0){
		--waitDelay;
		application_delay(1);
		check_usb_communication();
	}
	live_obj.calibration_force = 0;
	waitDelay = 5000;
	while(waitDelay >= 10
			&&
			(measure_weight(&live_obj.hx711[WH_CHANNEL_2]) > (MINIMIM_DESIRED_FORCE_FOR_LENGTH/2)
								||
					measure_weight(&live_obj.hx711[WH_CHANNEL_1]) > (MINIMIM_DESIRED_FORCE_FOR_LENGTH/2))
			){
		set_motor_movement(BACKWARD_DIRECTION);
		waitDelay -= 10;
		application_delay(10);
		check_usb_communication();
	}

}

TEST_STATUS processTabletTest(uint32_t *measured_value,
		uint32_t *remainingDistance, uint32_t *finalforce,
		uint8_t selected_test, uint8_t startUpDelayInSec) {

	TEST_STATUS test_status = TEST_COMPLETED;
	presentWeight = 0;
	finalWeight = 0;
	*measured_value = 0;
	*remainingDistance = 0;
	*finalforce = 0;

	resetTestStatus();
	MovingAvg_t ma;
	MA_Init(&ma);

#if(1)
	//	waitForMotorHomePosition();
	set_motor_movement(NO_DIRECTION);
	//	if (getHomePositionStatus())
	{

#if(ENABLE_MOTOR_MOVEMENT_MESSAGES == 1)
		DisplayPrintWithAlignment(MOVEMENT_INFO_LINE, 5, LEFT_ALIGNMENT, "At Home Position....");
#endif
#define PULSE_MOVEMENT_CHECK_TIME	2000
#define PULSE_MOVEMENT_MIN_REQUIRED	20
		DelayTimer_t pulseTestTime  = setDelayInMSecs(PULSE_MOVEMENT_CHECK_TIME);
		uint32_t lastPulseCount = getTravelPulseCount();
		while (isTestCancelled() == false && selected_test != NO_TEST
				&& selected_test != CANCEL_TEST) {

			check_usb_communication();

			while(isTestCancelled() == false && (live_obj.is_test_paused)){
				check_usb_communication();
				set_motor_movement(NO_DIRECTION);
			}

			start_time_loop = HAL_GetTick();
			if(isTestCancelled() == false){
				set_motor_movement(FORWARD_DIRECTION);
			}else{
				live_obj.live_force = measure_weight(&live_obj.hx711[WH_CHANNEL_1]);
			}
			live_obj.travelDistance = getTravelDistance();
			movementTimeLoop = HAL_GetTick() - start_time_loop;


			//			SCPI_Printf("w:%d,%d,t:%d,c:%d\r\n",
			//					(int)live_obj.live_force, (int)weight_2, (int)live_obj.travelDistance,
			//					(int)getTravelPulseCount());


			if(isTestCancelled() == false && (selected_test != HARDNESS_TEST) &&
					live_obj.live_force >= MINIMIM_DESIRED_FORCE_FOR_LENGTH){
				live_obj.live_force = measure_weight(&live_obj.hx711[WH_CHANNEL_1]);
			}

			if (live_obj.live_force > 0) {
				presentWeight = live_obj.live_force;
				//				presentWeight = MA_Update(&ma, live_obj.live_force);
			}

			if (presentWeight >= configurationObj.maximum_weight) {
				presentWeight = configurationObj.maximum_weight;
			}

			if (live_obj.travelDistance >= MINIMIM_TRAEL_DISTANCE_FOR_LENGTH
					&& presentWeight >= finalWeight) {
				finalWeight = presentWeight;
			}
			if(isDelayActive(pulseTestTime) == false){
				pulseTestTime  = setDelayInMSecs(PULSE_MOVEMENT_CHECK_TIME);
				if(get_abs_diffrence(lastPulseCount, getTravelPulseCount()) > PULSE_MOVEMENT_MIN_REQUIRED){
					lastPulseCount = getTravelPulseCount();
				}else{
					set_motor_movement(NO_DIRECTION);
					*measured_value = 0;
					test_status = TEST_FAILED;
					break;
				}
			}
			if (isTestCancelled()
					//|| getTravelDistance() > maximumAllowedTravelDistance
					|| presentWeight >= configurationObj.maximum_weight) {
				set_motor_movement(NO_DIRECTION);
				if(isTestCancelled() == false
						&& presentWeight >= configurationObj.maximum_weight){
					test_status = TEST_OUT_OF_WEIGHT;
				}else{
					test_status = isTestCancelled() ? TEST_CANCELED : TEST_FAILED;
				}
				break;
			} else if (selected_test == HARDNESS_TEST && getRemainingDistance() < 100){
				set_motor_movement(NO_DIRECTION);
				*measured_value = 0;
				test_status = TEST_TABLET_BROKEN;
				break;
			}
			else if (selected_test == THICKNESS_TEST&&
					finalWeight >= MINIMIM_DESIRED_FORCE_FOR_LENGTH &&
					live_obj.travelDistance >= MINIMIM_TRAEL_DISTANCE_FOR_LENGTH) {
				set_motor_movement(NO_DIRECTION);
				*measured_value = getRemainingDistance();

				break;
			} else if (selected_test == DIAMETER_TEST&&
					finalWeight >= MINIMIM_DESIRED_FORCE_FOR_LENGTH &&
					getTravelDistance() >= MINIMIM_TRAEL_DISTANCE_FOR_LENGTH) {
				set_motor_movement(NO_DIRECTION);
				*measured_value = getRemainingDistance();
				break;
			} else if (selected_test == WIDTH_TEST&&
					finalWeight >= MINIMIM_DESIRED_FORCE_FOR_LENGTH &&
					getTravelDistance() >= MINIMIM_TRAEL_DISTANCE_FOR_LENGTH) {
				set_motor_movement(NO_DIRECTION);
				*measured_value = getRemainingDistance();
				break;
			} else if (
					(finalWeight >= MINIMIM_DESIRED_FORCE_FOR_LENGTH) &&
					((presentWeight + BREAK_DESIRED_DROP_FORCE) < finalWeight
							|| (presentWeight >= configurationObj.maximum_weight))
							&& getTravelDistance() >= MINIMIM_TRAEL_DISTANCE_FOR_LENGTH) {
				if (selected_test == HARDNESS_TEST) {
					*measured_value = finalWeight;
				}else{
					test_status = TEST_TABLET_BROKEN	;
				}
				break;
			}
		}

//		set_motor_movement(NO_DIRECTION);
//		application_delay(500);

		live_obj.lastTravelPulseCount = getTravelPulseCount();
		live_obj.lastErrorPulseCount = live_obj.errorPulseCount;

//		uint16_t waitDelay = 50;
//		while(waitDelay > 0){
//			--waitDelay;
//			application_delay(1);
//			check_usb_communication();
//		}
//
//		waitDelay = 1000;
//		while(waitDelay >= 10 && measure_weight(&live_obj.hx711[WH_CHANNEL_1]) > (MINIMIM_DESIRED_FORCE_FOR_LENGTH/2)){
//			set_motor_movement(BACKWARD_DIRECTION);
//			waitDelay -= 10;
//			application_delay(10);
//			check_usb_communication();
//		}


		set_motor_movement(NO_DIRECTION);
		*finalforce = finalWeight;
		*remainingDistance = getRemainingDistance();
		if (test_status == TEST_FAILED) {
			live_obj.tablet_not_found = true;
		}

	}
	//	else {
	//		test_status = TEST_CANCELED;
	//	}
#endif

	set_motor_movement(NO_DIRECTION);
	return test_status;
}
