/*
 * application_structures.h
 *
 *  Created on: Dec 9, 2025
 *      Author: aj
 */

#ifndef APPLICATION_STRUCTURES_H_
#define APPLICATION_STRUCTURES_H_
#include "platform_specific.h"
#include "HX711.h"
#include "flash.h"
#include "serial_api.h"


#define BUZZER_MAJOR_NUMBER_OF_TIMES		3
#define BUZZER_NORMAL_NUMBER_OF_TIMES		3
#define INVALID_NUMBER 					0xFFFFFFFFUL
#define NO_BACKUP_LOCATION				0xFFFFFFFFU
#define SYNC_MAX_FAILURE_ATTEMPTS		2
#define CONFIGURATION_SIZE				sizeof(THT_Configuration)

#define CONFIGURATION_ADDRESS			FLASH_CONFIGUARATION_ADDRESS
#define CONFIGURATION_BKUP_ADDRESS		FLASH_CONFIGUARATION_BKUP_ADDRESS

//#define DEFAULT_LOAD_CELL_SACLE_FACTOR		107.316
#define DEFAULT_LOAD_CELL_SACLE_FACTOR		-66.463024 //-131.360001

#define KEYPAD_PRESS_BUZZER_TIME_MSEC		50

#define END_DETECTION_FOECE_DETECTION		30000
#define RECOVER_SLOW_SPPED_IF_FORCE_DIFF_ABOVE	2000 //700 //in gms
#define MINIMIM_DESIRED_FORCE_FOR_LENGTH	500 //700 //in gms
#define BREAK_DESIRED_DROP_FORCE			1000 //300 //in gms
#define MINIMIM_TRAEL_DISTANCE_FOR_LENGTH	2000 //10000 //in
#define MAXIMUM_MOVEMENT_PULSE_MISS_TIME	1000UL //in msec
#define BACK_MAXIMUM_MOVEMENT_PULSE_MISS_TIME	2000UL //in msec
#define EVENT_CONFIRMATION_TIME				10
#define MINIMUM_PULSE_MOVEMENT_REQUIRED		10

#define MINIMUM_WIDTH_DETECTION				1
#define MAXIMUM_WEIGHT						50000 //50000 //60KG
#define KG_TO_GRAMS							1000

#define MINIMUM_SPEED_IF_BELOW_THIS_WEIGHT		200
#define MINUM_WEIGHT_ALLOWED_DEVIATION			200

#define DEFAULT_MOTOR_STARTUP_DELAY					250UL

#define DEFAULT_NUMBER_OF_PULSES_PER_ROTATION		50UL
#define DEFAULT_NUMBER_OF_MMS_PER_ROTATION			2000UL //in mm*1000

#define CALIBRATION_REFRENCE_DISTANCE				40000UL
//#define DEFAULT_MAX_TRAVEL_DISTANCE				68500UL
//#define DEFAULT_MAX_NUMBER_OF_PULSES				3456UL
#define DEFAULT_MAX_TRAVEL_DISTANCE					66600UL //in mm*1000
#define DEFAULT_MAX_NUMBER_OF_PULSES				1665UL

//2946
//#define DEFAULT_MAX_TRAVEL_DISTANCE					70000UL
//#define DEFAULT_MAX_NUMBER_OF_PULSES				3530UL


#define DEFAULT_STARTUP_SPEED_MINIMUM_ROTATION_TIME	800UL //600UL
#define DEFAULT_SLOW_SPEED_MINIMUM_ROTATION_TIME	6400UL //600UL
#define DEFAULT_MAINTAIN_FORCE_SPEED_MINIMUM_ROTATION_TIME	54000UL //52000UL //64000UL
#define DEFAULT_FAST_SPEED_MINIMUM_ROTATION_TIME	200UL //250UL //300UL //336UL


#define MOVING_AVERAGE_SAMPLE_COUNT			5
#define MOTOR_BACKWARD_FAST_MOVEMENT_DELAY	1000

#define FAN_ON_TEMP      28.0f
#define FAN_OFF_TEMP     25.0f

#define NTC_BETA         3950.0f
#define NTC_R25          10000.0f
#define SERIES_RESISTOR  10000.0f

#define ADC_MAX_VALUE    4095.0f
typedef enum{
	WH_CHANNEL_1 = 0, WH_CHANNEL_2, MAX_WH_CHANNELS
}WEIGHT_CHANNELS;

typedef enum{
	NO_TEST, WEIGHT_TEST, THICKNESS_TEST, WIDTH_TEST, DIAMETER_TEST, HARDNESS_TEST, MAX_ALLOWED_TESTS
}TEST_TYPE;
#define CANCEL_TEST	MAX_ALLOWED_TESTS

typedef enum{
	TEST_COMPLETED, TEST_GOING_ON, TEST_PAUSED, TEST_CANCELED, TEST_FAILED,
	TEST_COMM_TIMEOUT, TEST_UNABLE_TO_MOVE,
	TEST_OUT_OF_WEIGHT, TEST_TABLET_BROKEN
}TEST_STATUS;

typedef enum{
	KGF_KP_UNIT = 0, NEWTON_UNIT, KPOUND_UNIT, MAX_HARDNESS_UNITS //POUNDS_UNIT,
}THT_HARDNESS_UNIT;

typedef enum{
	WEIGHT_KG_UNIT = 0, MAX_WEIGHT_UNITS
}THT_WEIGHT_UNIT;

typedef enum{
	MM_UNIT = 0, CM_UNIT, MAX_LENGTH_UNITS //POUNDS_UNIT,
}THT_LENGTH_UNIT;

typedef enum{
    NO_DIRECTION = 0, FORWARD_DIRECTION, BACKWARD_DIRECTION, MAX_DIRECTIONS
}MOVEMENT_DIRECTION;

typedef enum{
	LOAD_SENSE_OK = 0, LOAD_SENSE_NOT_FOUND, LOAD_SENSE_OUT_OF_REACH
}LOAD_SENSOR_STATUS;

typedef enum{
	START_UP_SCREEN_STATE = 0,
	DIAGNOSTICS_STATE,
	PROGRAM_RUN_STATE, TEST_RUN_STATE, PROGRAM_RESUME_STATE,
	CALIBRATION_STATE,
	USER_MANAGEMENT_STATE
}Application_State;


typedef enum{
	OP_NO_STATE, OP_GO_TO_HOME, OP_GO_TO_END, OP_TARE, OP_RST_WEIGHT,
	OP_INIT_WEIGHT, OP_MEASURE_WEIGHT, OP_MEASURE_WIDTH, OP_MEASURE_FORCE,
	OP_TEST_CANCEL, OP_TEST_PAUSE,
	OP_TEST_LOOP, OP_MAINTAIN_FORCE,
	OP_SELF_TEST_LOAD_CELL,OP_SELF_TEST_MOTOR,
	OP_SAVE_CONFIGURATION,
	OP_MAXIMUM
}HardnessOperations;

typedef struct __attribute__((__packed__)){
	uint16_t version;

	uint8_t weighing_scale_model;

	uint8_t width_reference;
	uint8_t hardness_reference;
	uint8_t hardness_options;

//	UserDetail user[NUMBER_OF_USERS];
	float scaleFactor[MAX_WH_CHANNELS];

	//Configurable from front end
	uint32_t baud_rate;
	uint32_t external_printer_baud_rate;
	uint32_t movement_duration;
	uint32_t home_to_end_distance;
	uint32_t home_to_end_pulseCount;
	uint32_t minimum_width;
	uint32_t maximum_weight;

	uint32_t slowSpeedRotationTime;
	uint32_t fastSpeedRotationTime;

	uint32_t reserved;
	uint16_t crc;
}THT_Configuration;

typedef struct __attribute__((__packed__)){
	union {
		struct {
			uint8_t hardness :1;
			uint8_t thickness :1;
			uint8_t diameter :1;
			uint8_t width :1;
			uint8_t weight :1;
		};
		uint16_t settings;
	};
	uint8_t weighing_scale_model;
	uint8_t quantity;
	uint8_t delayInSec; //in seconds
	uint16_t crc;
}TabletProgram;

typedef struct __attribute__((__packed__)){
	uint32_t hardness;
	uint32_t thickness;
	uint32_t diameter;
	uint32_t width;
	uint32_t weight;
	uint16_t crc;
}TabletRecord;

typedef struct{
	RTC_DATE_TIME_DOW live_date_time;

	Application_State state;
	Application_State prevState;

	int32_t live_samples[MOVING_AVERAGE_SAMPLE_COUNT];
	uint8_t latestSampleIx;
	uint8_t totalSampleCount;

	int32_t calibration_force;
	int32_t live_force;
	int32_t previous_force;


	union{
		struct{
			uint8_t home_sense_not_found:1;
			uint8_t end_sense_not_found:1;
			uint8_t position_sense_not_found:1;
			uint8_t sensor_not_found:1;
			uint8_t tablet_not_found:1;
			uint8_t motor_movement_fail:1;
		};
		uint16_t error_status;
	};
	uint16_t error_bit_status;

	union{
		struct{
			uint8_t tpm_paper_not_found:1;
			uint8_t tpm_not_found:1;
		};
		uint16_t warning_status;
	};

	union{
		struct{
			uint8_t home_sense_detected:1;
			uint8_t end_sense_detected:1;
			uint8_t dop_sense_detected:1;
		};
		uint16_t sensor_status;
	};


	union{
		struct{
			uint8_t restart_system:1;
			uint8_t is_test_canceled:1;
			uint8_t is_test_paused:1;
			uint8_t is_self_test_going_on:1;
			uint8_t is_method_run_going_on:1;
			uint8_t is_home_sense_detected_once:1;
		};
		uint16_t statusBits;
	};

	uint8_t lastShownErrorBit;
	uint8_t homeDetectionCount;;


	TabletProgram testObj;
	TabletRecord record;

	uint8_t completedSamplesCount;

	hx711_t hx711[MAX_WH_CHANNELS];

	uint32_t travelDistance;
	uint32_t travelPulseCount;
	MOVEMENT_DIRECTION movement_direction;


	DelayTimer_t movement_delay;
	DelayTimer_t home_sense_detection_delay;
	DelayTimer_t end_sense_detection_delay;
	DelayTimer_t dop_sense_detection_delay;
	DelayTimer_t motor_startup_delay;
	DelayTimer_t pkt_delay;

	uint32_t selected_calibration_weight;
	uint32_t requiredSpeedRotationTime;
	uint32_t eachRotationTimePeriod;
	uint32_t pulseGapTime;
	uint8_t resetPulseCount;
	uint32_t errorPulseCount;
	uint32_t movingPulseCount;
	uint32_t forwardCount;

	HardnessOperations operation;
	uint32_t required_force;

	TEST_STATUS lastStatus;
	uint32_t lastTravelPulseCount;
	uint32_t lastErrorPulseCount;
	uint32_t lastMeasuredValue;
	uint32_t lastMeasuredForce;
	uint32_t lastRemainingDistance;
	int32_t lastCalibration_force;
	int32_t lastLive_force;

	uint32_t startTime;
	uint32_t executionTime;
	uint32_t maintainance_time;

}ApplicationLiveStatus;

#endif /* APPLICATION_STRUCTURES_H_ */
