/*
 * This file contains example functions to be implemented in your main.c file.
 * The init_weight initialized the HX711 module with the desired gain and is used
 * for the tare weight. The function measure_weight can then be used to obtain
 * the weight measured by each load cell (Channel A and B). 
 *
 *  Created on: 28 may 2023
 *      Author: PCov3r
 */

#include <HX711.h>
#include "application.h"

#define SAMPLE_AVG_COUNT				2
#define TARE_SAMPLE_AVG_COUNT			10
#define CALIBRATION_SAMPLE_AVG_COUNT	10
/**
 * @brief  Weight Initialization Function
 * @retval None
 */
void init_weight(uint8_t channel, hx711_t *hx711, float channel_a_scale) {

	if(channel == 0){
		/* Initialize the hx711 sensors */
		hx711_init(hx711, HX711_CLK_1_OP_GPIO_Port, HX711_CLK_1_OP_Pin, HX711_DATA_1_IP_GPIO_Port,
				HX711_DATA_1_IP_Pin);
	}else{
		hx711_init(hx711, HX711_CLK_2_OP_GPIO_Port, HX711_CLK_2_OP_Pin, HX711_DATA_2_IP_GPIO_Port,
				HX711_DATA_2_IP_Pin);
	}

	/* Configure gain for each channel (see datasheet for details) */
	set_gain(hx711, 128, 32);

	/* Set HX711 scaling factor (see README for procedure) */
	//	set_scale(hx711, -44.25, -10.98);
	set_scale(hx711, channel_a_scale, 1);

	/* Tare weight */
	//	tare_all(hx711, 10);
	tare(hx711, TARE_SAMPLE_AVG_COUNT, CHANNEL_A);

}

void tare_weight(hx711_t *hx711){
	tare(hx711, TARE_SAMPLE_AVG_COUNT, CHANNEL_A);
}

void reset_weight(hx711_t *hx711){
	set_scale(hx711, 1, 1);
	tare(hx711, TARE_SAMPLE_AVG_COUNT, CHANNEL_A);
}

/**
 * @brief  Weight Measuring Function
 * @retval the weight measured for each associated channel
 */

float measure_actual_weight(hx711_t *hx711){
	return get_weight(hx711, SAMPLE_AVG_COUNT, CHANNEL_A);
}
float measure_weight(hx711_t *hx711) {
	float weightA = 0;
	//	long weightB = 0;

	// Measure the weight for channel A
	weightA = get_weight(hx711, SAMPLE_AVG_COUNT, CHANNEL_A);
	// Weight cannot be negative
	weightA = (weightA < 0) ? 0 : weightA;

	//	// Measure the weight for channel B
	//	weightB = get_weight(&hx711, 10, CHANNEL_B);
	//	// Weight cannot be negative
	//	weightB = (weightB < 0) ? 0 : weightB;

	return weightA;
}

float measure_calibration_weight(hx711_t *hx711) {
	float weightA = 0;
	//	long weightB = 0;

	// Measure the weight for channel A
	weightA = get_weight(hx711, CALIBRATION_SAMPLE_AVG_COUNT, CHANNEL_A);
	// Weight cannot be negative
	weightA = (weightA < 0) ? 0 : weightA;

	//	// Measure the weight for channel B
	//	weightB = get_weight(&hx711, 10, CHANNEL_B);
	//	// Weight cannot be negative
	//	weightB = (weightB < 0) ? 0 : weightB;

	return weightA;
}
//-131360, -130931, -67179
//-131.360001 - scale
// previous sacle factor - 107.316002

#if(0)
float calibrate_weight(hx711_t* hx711, long actual_weight) {
	long caliculate_weight = 0;
	float channel_a_scale = 0;
	while (actual_weight > 0) {
		if (wait_for_confirmation(CALIBRATION_MESSAGES_POSITION, 0,
				"REMOVE WEIGHTS AND PRESS ENTER", ENTER_KEY) && wait_ready(hx711)) {
			set_scale(hx711, 1, 1);
			tare(hx711, TARE_SAMPLE_AVG_COUNT, CHANNEL_A);
			if(wait_for_confirmation(CALIBRATION_MESSAGES_POSITION, 0,
					"APPLY WEIGHT AND PRESS ENTER", ENTER_KEY) == ENTER_KEY){
				caliculate_weight = get_weight(hx711, CALIBRATION_SAMPLE_AVG_COUNT, CHANNEL_A);
				if(caliculate_weight != INVALID_LOAD_SENSE_VALUE){
					channel_a_scale = (float)caliculate_weight/actual_weight;
					configurationObj.scaleFactor = channel_a_scale;
					save_application_configuration();
					set_scale(hx711, channel_a_scale, 1);
					wait_for_confirmation(CALIBRATION_MESSAGES_POSITION, 0,
							"CALIBRATION PASS AND PRESS ENTER", ENTER_KEY);
					break;
				}else{
					if(wait_for_confirmation(CALIBRATION_MESSAGES_POSITION, 0,
							"CALIBRATION FAIL. ESC TO EXIT", ENTER_KEY) == false){
						break;
					}
				}
			}
		}else{
			break;
		}
		//		waitTillDoorOpen();
	}
	return channel_a_scale;
}
#endif

