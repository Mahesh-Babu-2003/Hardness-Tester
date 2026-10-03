#include "scpi.h"
uint8_t scpi_buffer[512];
/*********************************************************
 * Command Table
 *********************************************************/
typedef struct {
	const char *cmd;
	void (*handler)(char *args);
} SCPI_Command;

static void scpi_get_direction(char *args);
static void scpi_set_direction(char *args);
static void scpi_set_required_direction(char *args);
static void scpi_set_operation(char *args);
static void scpi_calibration_operation(char *args);
static void scpi_idn(char *args);
void scpi_get_status(char *args);
static void scpi_process_lims(char *args);

static const SCPI_Command scpi_table[] = {
		{"IDN?",        scpi_idn},
		{"DIR?",   scpi_get_direction},
		{"DIR",    scpi_set_direction},
		{"RDIR",    scpi_set_required_direction},
		{"OPERATION",    scpi_set_operation},
		{"CALIBRATION",    scpi_calibration_operation},
		{"STAT?",   scpi_get_status},
		{"LIMS",   scpi_process_lims},
};

#define SCPI_CMD_COUNT (sizeof(scpi_table)/sizeof(SCPI_Command))

const char COMMAND_END_BYTE[]	= {"\r\n"};
const char SUCCESSFUL_CMD[] 	= {"<OK\r\n"};
const char SUCCESSFUL[] 	= {"<OK"};
const char ERROR_CMD[] 			= {"<ERR"};

typedef struct {
	const char *description;
} INFO_STRING;

const INFO_STRING DIRECTION_STRING[MAX_DIRECTIONS] = {
		{"NO"},
		{"FWD"},
		{"BWD"},
};


const INFO_STRING HARDNESS_OPERATIONS[OP_MAXIMUM] = {
		{"NO"},
		{"HOME"},
		{"END"},
		{"TARE"},
		{"RST:WEIGHT"},
		{"INIT:WEIGHT"},
		{"MEASURE:WEIGHT"},
		{"MEASURE:WIDTH"},
		{"MEASURE:FORCE"},
		{"TEST:CANCEL"},
		{"TEST:PAUSE"},
		{"MEASURE:WIDTH_LOOP"},
		{"MAINTAIN:FORCE"},
		{"SELF_TEST:LOAD_CELL"},
		{"SELF_TEST:MOTOR"},
		{"CONFIGURATION:SAVE"},
};

char communication_buffer[COMMUNCATION_BUFFER_LENGTH] = { 0 };
uint8_t data_started = false, data_length = 0;
/*********************************************************
 * Universal SCPI Output
 *********************************************************/
void SCPI_Send(const char *msg)
{
	uint32_t start = HAL_GetTick();
	WriteAsyncronusByteStreamTo_UART(&gstUART_TransceiverList[USB_COMM_UART],
			(const uint8_t*) msg,
			strlen(msg));
	//	while (HAL_UART_GetState(gstUART_TransceiverList[USB_COMM_UART].huart) != HAL_UART_STATE_READY)
	//	{
	//		if ((HAL_GetTick() - start) > (strlen(msg))) // 100 ms timeout
	//		{
	//			// handle timeout
	//			break;
	//		}
	//	}
}

void SCPI_Printf(const char *format, ... )
{
#if(PRINTF_DEBUG_SUPPORT)
	va_list args;
	va_start(args, format);
	//  volatile uint16_t len;
	/*convert into string at buff[0] of length iw*/
	//  len =
	vsprintf((char*)scpi_buffer, format, args);
	va_end(args);

	SCPI_Send((const char*)scpi_buffer);
#endif
}

/*********************************************************
 * Helpers
 *********************************************************/
static void trim(char *s)
{
	while (*s == ' ' || *s == '\t') s++;
}

static int str_ieq(const char *a, const char *b)
{
	return strcasecmp(a, b) == 0;
}

/*********************************************************
 * Parse channel format:  {@1}
 *********************************************************/
//static int parse_channel(const char *s)
//{
//    int ch = 0;
//    if (sscanf(s, "{@%d}", &ch) == 1)
//        return ch;
//    return -1;
//}

uint8_t parse_direction(const char* s, MOVEMENT_DIRECTION* direction){
	uint8_t valid_direction = false;
	for(uint8_t ix = 0; ix < MAX_DIRECTIONS; ++ix){
		if(strcmp(s, DIRECTION_STRING[ix].description) == 0){
			valid_direction = true;
			*direction = ix;
			break;
		}
	}
	return valid_direction;
}

uint8_t parse_operations(const char* s, HardnessOperations* operation){
	uint8_t valid_direction = false;
	for(uint8_t ix = 0; ix < OP_MAXIMUM; ++ix){
		if(strcmp(s, HARDNESS_OPERATIONS[ix].description) == 0){
			valid_direction = true;
			*operation = ix;
			break;
		}
	}
	return valid_direction;
}

/*********************************************************
 * IDN Command
 *********************************************************/
static void scpi_idn(char *args)
{
	SCPI_Send("<idn Hardness Tester,MODEL-X,1.0\r\n");
}

static void scpi_calibration_operation(char *args){

	if (!args) {
		return;
		SCPI_Printf("%s: Missing argument\r\n", ERROR_CMD);
	}
	trim(args);
	char* operation = args;
	char *calculatd_or_max_distance = strchr(args, ',');
	char *actual_or_pulse_count = NULL;
	if (calculatd_or_max_distance != NULL) {
		*calculatd_or_max_distance = 0;
		++calculatd_or_max_distance;
		actual_or_pulse_count = strchr(calculatd_or_max_distance, ',');
	}
	if (actual_or_pulse_count != NULL) {
		*actual_or_pulse_count = 0;
		++actual_or_pulse_count;
	}
	if(actual_or_pulse_count != NULL && calculatd_or_max_distance != NULL){
		uint32_t calulated_value = 0, actual_value = 0;
		calulated_value = atoi(calculatd_or_max_distance);
		actual_value = atoi(actual_or_pulse_count);
		if(str_ieq("refweight", operation)){
			calulated_value =  measure_weight(&live_obj.hx711[WH_CHANNEL_2]);
			float scaleFactor =
					(configurationObj.scaleFactor[WH_CHANNEL_2] * calulated_value)/actual_value	;
			if(get_abs_diffrence(abs(scaleFactor), abs(DEFAULT_LOAD_CELL_SACLE_FACTOR)) < 25){
				configurationObj.scaleFactor[WH_CHANNEL_2] = scaleFactor;
				live_obj.hx711[WH_CHANNEL_2].Ascale = scaleFactor;
				//init_weight(WH_CHANNEL_2, &live_obj.hx711[WH_CHANNEL_2], configurationObj.scaleFactor[WH_CHANNEL_2]);
				save_application_configuration();
			}else{
				SCPI_Printf("%s: Invalid data\r\n", ERROR_CMD);
			}
		}else if(str_ieq("weight", operation)){
			float scaleFactor =
					(configurationObj.scaleFactor[WH_CHANNEL_1] * calulated_value)/actual_value	;
			if(get_abs_diffrence(abs(scaleFactor), abs(DEFAULT_LOAD_CELL_SACLE_FACTOR)) < 25){
				configurationObj.scaleFactor[WH_CHANNEL_1] = scaleFactor;
				init_weight(WH_CHANNEL_1, &live_obj.hx711[WH_CHANNEL_1], configurationObj.scaleFactor[WH_CHANNEL_1]);
				save_application_configuration();
			}else{
				SCPI_Printf("%s: Invalid data\r\n", ERROR_CMD);
			}
		}else if(str_ieq("width", operation)){
			uint32_t max_distance = 0, pulse_count = 0;
			max_distance = atoi(calculatd_or_max_distance);
			pulse_count = atoi(actual_or_pulse_count);
			float expected_pulse =
					(max_distance * DEFAULT_NUMBER_OF_PULSES_PER_ROTATION)/	DEFAULT_NUMBER_OF_MMS_PER_ROTATION;
			if(get_abs_diffrence(expected_pulse, pulse_count) < 100){

				configurationObj.home_to_end_pulseCount = pulse_count;
				configurationObj.home_to_end_distance = max_distance;

				save_application_configuration();
			}else{
				SCPI_Printf("%s: Invalid data\r\n", ERROR_CMD);
			}
		}else{

		}
	}else{
		SCPI_Printf("%s: Invalid data\r\n", ERROR_CMD);
	}
}

static void scpi_set_operation(char *args){

	if (!args) {
		return;
		SCPI_Printf("%s: Missing argument\r\n", ERROR_CMD);
	}
	trim(args);
	HardnessOperations operation;
	char *nextData = strchr(args, ',');
	if (nextData != NULL) {
		*nextData = 0;
		nextData++;
	}

	if(parse_operations(args, &operation)){
		if(operation == OP_TEST_CANCEL){
			live_obj.is_test_canceled = true;
			SCPI_Send(SUCCESSFUL_CMD);
		}else if(operation == OP_TEST_PAUSE){
			live_obj.is_test_paused = !live_obj.is_test_paused;
			SCPI_Send(SUCCESSFUL_CMD);
		}else if(live_obj.operation ==  OP_NO_STATE){
			if(operation == OP_MAINTAIN_FORCE){
				if(nextData != NULL){ // now at {@1}
					trim(nextData);
					live_obj.required_force = atoi(nextData);

					live_obj.operation = operation;
					SCPI_Send(SUCCESSFUL_CMD);
				}else{
					SCPI_Printf("%s: Invalid operation\r\n", ERROR_CMD);
				}
			}else{
				live_obj.operation = operation;
				SCPI_Send(SUCCESSFUL_CMD);
			}

		}else{
			SCPI_Printf("%s: Operation Goingon\r\n", ERROR_CMD);
		}
	}else{
		char reply[40];
		SCPI_Printf("%s: Invalid operation\r\n", ERROR_CMD);
	}
}

static void scpi_set_required_direction(char *args)
{
	if (!args) {
		SCPI_Printf("%s: Missing argument\r\n", ERROR_CMD);
		return;
	}
	trim(args);
	MOVEMENT_DIRECTION direction;
	if(parse_direction(args, &direction)){
		set_motor_movement(direction);
		SCPI_Send(SUCCESSFUL_CMD);
	}else{
		SCPI_Printf("%s: Invalid direction\r\n", ERROR_CMD);
	}
}

static void scpi_process_lims(char *args){

	if (!args) {
		SCPI_Printf("%s: Missing data\r\n", ERROR_CMD);
		return;
	}
	trim(args);
	uint8_t decode[1024] = { 0 };
	uint16_t decoded_length = base64_decode(args, decode);
	if(decoded_length > 0){
		HAL_UART_Transmit(&LIMES_USART, decode, decoded_length, decoded_length);
	}
}

static void scpi_set_direction(char *args)
{
	if (!args) {
		SCPI_Printf("%s: Missing argument\r\n", ERROR_CMD);
		return;
	}
	trim(args);
	MOVEMENT_DIRECTION direction;
	if(parse_direction(args, &direction)){
		set_motor_movement(direction);
		SCPI_Send(SUCCESSFUL_CMD);
	}else{
		SCPI_Printf("%s: Invalid direction\r\n", ERROR_CMD);
	}


	//    // Syntax is: value,{@ch}
	//    // Example:   3.3,{@1}
	//
	//    char *comma = strchr(args, ',');
	//    if (!comma) {
	//        SCPI_Send("ERR: Missing channel\r\n");
	//        return;
	//    }
	//
	//    *comma = 0;      // separate value
	//    float value = strtof(args, NULL);
	//
	//    comma++;         // now at {@1}
	//    trim(comma);
	//
	//    int ch = parse_channel(comma);
	////    if (ch < 1 || ch > MAX_CHANNELS) {
	////        SCPI_Send("ERR: Invalid channel\r\n");
	////        return;
	////    }
	////
	////    source_voltage[ch - 1] = value;
	//
	//    char reply[40];
	//    sprintf(reply, "OK (CH%d)\r\n", ch);
	//    SCPI_Send(reply);
}


/*********************************************************
 * Handler: SOUR:VOLT? {@1}
 *********************************************************/
static void scpi_get_direction(char *args)
{
	char out[32];
	sprintf(out, "<DIR %s\r\n", DIRECTION_STRING[live_obj.movement_direction].description);
	SCPI_Send(out);

	//    if (!args) {
	//        SCPI_Send("ERR: Missing channel\r\n");
	//        return;
	//    }
	//
	//    trim(args);

	//    int ch = parse_channel(args);
	//    if (ch < 1 || ch > MAX_CHANNELS) {
	//        SCPI_Send("ERR: Invalid channel\r\n");
	//        return;
	//    }
	//
	//    char out[32];
	//    sprintf(out, "%0.4f\r\n", source_voltage[ch - 1]);
	//    SCPI_Send(out);
}


void scpi_get_status(char *args){
	SCPI_Printf("<STAT %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
			(int)live_obj.operation,
			(int)live_obj.movement_direction,
			(int)getTravelPulseCount(),
			(int)live_obj.lastTravelPulseCount,
			(int)live_obj.movingPulseCount,
			(int)live_obj.errorPulseCount,
			(int)live_obj.home_sense_detected,
			(int)live_obj.end_sense_detected,
			(int)live_obj.dop_sense_detected,
			(int)live_obj.warning_status,
			(int)live_obj.error_status,
			(int)live_obj.statusBits,
			(int)getTravelDistance(),
			(int)getRemainingDistance(),
			(int)getMaximumTravelDistance(),
			(int)live_obj.live_force,
			(int)live_obj.calibration_force,
			(int)live_obj.lastStatus,
			(int)live_obj.lastMeasuredValue,
			(int)live_obj.lastRemainingDistance,
			(int)live_obj.lastMeasuredForce
	);
}
/*********************************************************
 * Main Parser
 *********************************************************/
void SCPI_Parse(char *input)
{
	trim(input);

	// Split COMMAND and ARGS
	char *args = strchr(input, ' ');
	if (args) {
		*args = '\0';
		args++;
	}

	for (int i = 0; i < SCPI_CMD_COUNT; i++) {
		if (str_ieq(input, scpi_table[i].cmd)) {
			scpi_table[i].handler(args);
			return;
		}
	}

	SCPI_Printf("%s: %s Unknown Command\r\n", input, ERROR_CMD);
}


void resetCommunication() {
	data_started = false;
	memset(communication_buffer, 0, sizeof(communication_buffer));
	data_length = 0;
}

uint8_t process_usb_communication_byte(uint8_t byte) {
	uint8_t status = false;

	if (byte == COMMAND_START_BYTE) {
		resetCommunication();
		data_started = true;
	} else if (data_started == true) {
		if (data_length >= sizeof(communication_buffer)) {
			resetCommunication();
		} else if (byte == COMMAND_END_BYTE_1 || byte == COMMAND_END_BYTE_2) {
			communication_buffer[data_length] = '\0';
			if (data_length >= COMMAND_MIN_LEN) {
				SCPI_Parse((char*) communication_buffer);
			}
			status = true;
			resetCommunication();
		} else {
			communication_buffer[data_length] = byte;
			++data_length;
		}
	}

	return status;
}



void process_loadcell_sensor_selftest(){
	//	show_animation(line, 0, CENTER_ALIGNMENT,  "LOAD SENSOR TEST GOING ON", startup_delay);

	live_obj.sensor_not_found =	wait_ready(&live_obj.hx711[WH_CHANNEL_1]) == false;

	volatile float temp32 = measure_actual_weight(&live_obj.hx711[WH_CHANNEL_1]);

	if(live_obj.sensor_not_found){
		//DisplayPrintWithAlignment(line, 0, CENTER_ALIGNMENT,  "LOAD SESNSOR FAIL");
		//buzzer_cycles(BUZZER_BEEP_COUNT, BUZZER_BEEP_TIME);
		SCPI_Printf("%s LOAD SESNSOR FAIL\r\n", ERROR_CMD);
	}else{
		//DisplayPrintWithAlignment(line, 0, CENTER_ALIGNMENT,  "LOAD SESNSOR OK");
		SCPI_Printf("%s LOAD SESNSOR OK\r\n", SUCCESSFUL);
	}
}


void process_motor_selftest(){
	//	char message[] = {"MOTOR TEST GOING ON "};
	//	show_animation(line, 0, CENTER_ALIGNMENT,  message, startup_delay);

	live_obj.travelPulseCount = configurationObj.home_to_end_pulseCount;
	live_obj.home_sense_not_found = waitForMotorHomePosition() == false;

	application_delay(100);

	if(live_obj.home_sense_not_found == false){
		uint16_t travelPulseCount = getTravelPulseCount();
		set_motor_movement(FORWARD_DIRECTION);
		uint32_t startupTick = HAL_GetTick();
		do{
			application_delay(1);
			set_motor_movement(FORWARD_DIRECTION);
			check_usb_communication();
		}while((HAL_GetTick() - startupTick) < 2000 && isTestCancelled() == false);
		//	show_animation(line, 0, CENTER_ALIGNMENT,  message, FORWARD_DIRECTION_SELF_TEST_TIME);
		set_motor_movement(NO_DIRECTION);
		live_obj.position_sense_not_found  = getTravelPulseCount()  == travelPulseCount;
		waitForMotorHomePosition();
	}
	set_motor_movement(NO_DIRECTION);

	application_delay(100);

	if(live_obj.position_sense_not_found || live_obj.home_sense_not_found){
		//		DisplayPrintWithAlignment(line, 0, CENTER_ALIGNMENT,  "MOTOR MOVEMENT FAIL");
		//		buzzer_cycles(BUZZER_BEEP_COUNT, BUZZER_BEEP_TIME);
		SCPI_Printf("%s MOTOR MOVEMENT FAIL\r\n", ERROR_CMD);
	}else{
		//		DisplayPrintWithAlignment(line, 0, CENTER_ALIGNMENT,  "MOTOR MOVEMENT OK");
		SCPI_Printf("%s MOTOR MOVEMENT OK\r\n", SUCCESSFUL);
	}
}
