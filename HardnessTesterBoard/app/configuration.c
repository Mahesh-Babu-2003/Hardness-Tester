#include "application.h"

void fill_default_configuration() {
	memset(&configurationObj, 0, sizeof(configurationObj));

	configurationObj.slowSpeedRotationTime = DEFAULT_SLOW_SPEED_MINIMUM_ROTATION_TIME;
	configurationObj.fastSpeedRotationTime = DEFAULT_FAST_SPEED_MINIMUM_ROTATION_TIME;

	configurationObj.home_to_end_pulseCount = DEFAULT_MAX_NUMBER_OF_PULSES;
	configurationObj.home_to_end_distance = DEFAULT_MAX_TRAVEL_DISTANCE;

	configurationObj.scaleFactor[WH_CHANNEL_1] = DEFAULT_LOAD_CELL_SACLE_FACTOR;
	configurationObj.scaleFactor[WH_CHANNEL_2] = DEFAULT_LOAD_CELL_SACLE_FACTOR; //* 2;


	configurationObj.maximum_weight = MAXIMUM_WEIGHT;
	configurationObj.minimum_width = MINIMUM_WIDTH_DETECTION;


	configurationObj.baud_rate = DEFAULT_BAUD_RATE;

}

uint8_t save_data_to_external_memoey(uint32_t address, uint32_t backup_address,
		uint8_t *data, uint16_t data_size, uint8_t attempts) {
	uint8_t result = true;
	if (attempts == 0) {
		attempts = 1;
	}
	do {
		--attempts;
		result = SaveDataOnFlashPage(address, (uint8_t*) data,
				data_size) == HAL_OK;
		if (backup_address != NO_BACKUP_LOCATION) {
			result |= SaveDataOnFlashPage(backup_address,
					(uint8_t*) data, data_size) == HAL_OK;
		}
	} while (result == false && attempts > 0);
	return result;
}

bool save_data_with_crc(uint32_t address, uint32_t backup_address,
		uint8_t *data, uint16_t data_size, uint8_t attempts) {
	calculate_and_fill_crc(data, data_size);
	return save_data_to_external_memoey(address, backup_address, data,
			data_size, attempts);;
}

bool read_data_with_crc(uint32_t address, uint32_t backup_address,
		uint8_t *data, uint16_t data_size, uint8_t attempts) {
	bool result = false;
	do {
		--attempts;
		result = (LoadDataFromFlashPage(address, data, data_size)
				== HAL_OK);
		if (result) {
			result = validate_crc(data, data_size);
		}
		if (result == false && (backup_address != NO_BACKUP_LOCATION)) {
			result |= (LoadDataFromFlashPage(backup_address,
					(uint8_t*) data, data_size) == HAL_OK);
			if (result) {
				result = validate_crc(data, data_size);
			}
		}
	} while (result == false && attempts > 0);

	return result;
}


uint8_t save_application_configuration() {
	return save_data_with_crc(CONFIGURATION_ADDRESS,
			CONFIGURATION_BKUP_ADDRESS, (uint8_t*) &configurationObj,
			CONFIGURATION_SIZE, SYNC_MAX_FAILURE_ATTEMPTS);
}


void load_configuration() {
	if (read_data_with_crc(CONFIGURATION_ADDRESS, CONFIGURATION_BKUP_ADDRESS,
			(uint8_t*) &configurationObj, CONFIGURATION_SIZE,
			SYNC_MAX_FAILURE_ATTEMPTS) == false) {
		fill_default_configuration();
		save_application_configuration();
	}
}
