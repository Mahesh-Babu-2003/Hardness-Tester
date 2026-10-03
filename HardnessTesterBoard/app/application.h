/*
 * application.h
 *
 *  Created on: Dec 9, 2025
 *      Author: aj
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "application_structures.h"

extern ApplicationLiveStatus live_obj;
extern THT_Configuration configurationObj;

void process_application();
void initialization_application();

void fill_default_configuration();
void init_weight(uint8_t channel, hx711_t *hx711, float channel_a_scale);
void reset_weight(hx711_t *hx711);
void tare_weight(hx711_t *hx711);
float measure_weight(hx711_t *hx711);
float measure_actual_weight(hx711_t *hx711);

void ReleaseLiveForce();

void setSlowRequiredDistance(uint32_t distance);
void set_motor_movement(MOVEMENT_DIRECTION direction);

void resetTestStatus();
uint8_t isTestCancelled();

uint32_t getRemainingDistance();
uint32_t getTravelDistance();
uint32_t getMaximumTravelDistance() ;
uint32_t getTravelPulseCount();


void process_loadcell_sensor_selftest();
void process_motor_selftest();

uint8_t waitForMotorEndPosition();
uint8_t waitForMotorHomePosition();
TEST_STATUS maintain_weight();
TEST_STATUS processTabletTest(uint32_t *measured_value,
		uint32_t *remainingDistance, uint32_t* finalforce,
		uint8_t selected_test, uint8_t startUpDelayInSec) ;

#endif /* APPLICATION_H_ */
