/*
 * common.h
 *
 *  Created on: Jul 24, 2020
 *      Author: VGS TECHNOLOGIES
 */

#ifndef CUSTOM_INC_COMMON_H_
#define CUSTOM_INC_COMMON_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "CRC.h"
//#include "printf.h"

#define	STM32_CONTROLLER_TARGET			1
#define	RENESAS_CONTROLLER_TARGET		2
#define	TI_CONTROLLER_TARGET			3
#define	PIC_CONTROLLER_TARGET			4

#define TARGET_CONTROLLER				STM32_CONTROLLER_TARGET

#if(TARGET_CONTROLLER == STM32_CONTROLLER_TARGET)
#define __delay_ms		HAL_Delay
#define _delay_ms		HAL_Delay
#define delay_ms		HAL_Delay
#else
typedef enum
{
	GPIO_PIN_RESET = 0U,
	GPIO_PIN_SET
} GPIO_PinState;

typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03,
  HAL_NOT_FOUND  = 0xFF,
} HAL_StatusTypeDef;

#endif

#define PIN_ACTIVE_LOW	0
#define PIN_ACTIVE_HIGH	1

#define ENABLE_OP			1
#define DISABLE_OP			0

#define PIN_RESET 	GPIO_PIN_RESET
#define PIN_SET 	GPIO_PIN_SET

#define PSI_TO_MMHG 			51.7149f
#define BAR_TO_MMHG 			750.062f

#define PI	3.1415926
#define SIZE_OF_CRC						sizeof(uint16_t)
#define MAX_VALUE_PRECISION 3

#define DECIMAL_TO_BCD(X)      ( ( (X/10) << 4 ) + (X%10) )
#define BCD_TO_DECIMAL(X)      ( ( (X>>4) * 10) + ( X & 0x0F ) )

#define VOLTAGE_TO_MILLI_VOLTS      	1000U

#define NUMBER_OF_USECS_PER_MIN					60000000UL
#define NUMBER_OF_MILLI_SECONDS_PER_SEC      	1000U
#define HUNDREAD_MSEC_TO_MSEC					100
#define HALF_SEC_TO_MSEC						500
#define SEC_TO_MSEC								1000UL
#define NUMBER_OF_MSECS_PER_MIN       	60000U
#define NUMBER_OF_SECONDS_PER_MIN       60U
#define NUMBER_OF_MINS_PER_HOUR         60U
#define NUMBER_OF_SECONDS_PER_HOUR      3600U
#define NUMBER_OF_HOURS_PER_DAY         24U
#define NUMBER_OF_MINUTES_PER_DAY       (NUMBER_OF_HOURS_PER_DAY * NUMBER_OF_MINS_PER_HOUR)
#define NUMBER_OF_DAYS_PER_WEEK         7U
#define NUMBER_OF_MONTHS_PER_YEAR       12U
#define MAXIMUM_MINUTES_PER_MONTH       (31U * NUMBER_OF_MINUTES_PER_DAY)

#define RTC_DATE_TIME_LENGTH            6      //YY/MM/DD HH:MM:SS
#define RTC_DATE_LENGTH                 3      //YY/MM/DD
#define RTC_TIME_LENGTH                 3      //HH:MM:SS

#define RTC_DRIFT_ALLOWED_TIME		600UL //drift allowed time
#define APP_START_YEAR				2000
#define RTC_BASE_YEAR				1900

#define NUMBER_OF_SEC_PER_DAY		86400
#define DATE_TIME_OPTIONAL_VALUE        0xFFU
typedef enum{


  DATE_TIME_YEAR_INDEX = 0, DATE_TIME_MONTH_INDEX = 1, DATE_TIME_DATE_INDEX = 2,
  DATE_TIME_HOUR_INDEX = 3, DATE_TIME_MINUTE_INDEX = 4, DATE_TIME_SECOND_INDEX = 5,

  DATE_YEAR_INDEX = 0, DATE_MONTH_INDEX = 1, DATE_DATE_INDEX = 2, DATE_DAY_INDEX = 3,

  TIME_HOUR_INDEX = 0, TIME_MINUTE_INDEX = 1, TIME_SECOND_INDEX = 2, TIME_MILLI_SECOND_INDEX = 3,
}DATE_TIME_INDEX;

typedef enum{
  SUN_DAY = 0, MON_DAY, TUES_DAY, WEDNES_DAY, THURS_DAY, FRI_DAY, SATUR_DAY, WEEK_DAY_COUNT
}WEEK_DAY;

typedef struct __attribute__((__packed__)) {
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} Custom_Date_Time;

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t dayOfWeek;
    uint8_t date;
    uint8_t month;
    uint8_t year;
}RTC_DATE_TIME_DOW;

typedef uint8_t  RTC_DATE_TIME[RTC_DATE_TIME_LENGTH];
typedef uint8_t  RTC_DATE[RTC_DATE_LENGTH];
typedef uint8_t  RTC_TIME[RTC_TIME_LENGTH];


bool validate_date(uint8_t* buffer, bool is_reverse);
bool validate_date_time(uint8_t* buffer, bool is_reverse);
bool validate_time(uint8_t* buffer, bool is_reverse);
uint32_t getTimestamp(int year, int month, int date, int hour, int min, int sec);

uint64_t get_abs_diffrence(uint64_t val1, uint64_t val2);
uint16_t getHighestOfSamples(uint16_t* samples, uint16_t sampleCount);
uint16_t getPeekToPeekOfSamples(uint16_t* samples, uint16_t sampleCount);
uint16_t getAverageOfSamples(uint16_t* samples, uint16_t sampleCount);

double compute_rms_with_cluster(uint16_t *samples, uint16_t sampleCount, uint16_t mean_adc,
		uint16_t cluster_sample_count, uint16_t cluster_offset);
double compute_rms_with_samples(uint16_t *samples, uint16_t sampleCount, uint16_t mean_adc);
bool find_mean_from_peak_to_peak(uint16_t *samples, uint16_t sampleCount, uint16_t* meanValue);


uint16_t getNthFeild(uint8_t* src_buffer, uint8_t* dest_buffer, uint8_t separator,
											uint8_t feild_id, uint16_t src_length, uint16_t dest_length);

void replace_character(char* data_ptr, uint16_t length, uint8_t old_character, uint8_t new_character);
uint16_t calc_character_count(char* s, uint16_t length, uint8_t data) ;


bool isValueWithInBoundaies(uint32_t current_time, uint32_t start_time, uint32_t end_time);

void fill_sin_values(uint16_t* buffer, uint16_t sample_count, uint16_t max_value);
void fill_triangle_values(uint16_t* buffer, uint16_t sample_count, uint16_t max_value);


void printTobase (long n, uint8_t base, uint8_t* destination);
unsigned short crc16(const uint8_t* data_p, uint16_t length) ;

void memcpy_reverse(void* dest, const void* src, uint16_t length);

void bytes_to_value(void* dest, const void* src, uint8_t length);
int64_t bytes_to_singed_value(const void* src, uint8_t length);
uint64_t bytes_to_unsinged_value(const void* src, uint8_t length);

void value_to_bytes(void* dest, const void* src, uint8_t length);
void unsigned_value_to_hex(uint64_t value, uint8_t* hexbuf, uint8_t length);
void signed_value_to_hex(int64_t value, uint8_t* hexbuf, uint8_t length);
void long8byteshex(uint64_t value, uint8_t* hexbuf);
void long4byteshex(uint32_t value, uint8_t* hexbuf);
void long2byteshex(uint16_t value, uint8_t* hexbuf);
unsigned short ModBus_CRC16( unsigned char * Buffer, unsigned short Length );


void getValueToString(int64_t value, char* buffer, uint8_t length);
void convertUpperCaseToLower(unsigned char* dest, unsigned char* src, uint16_t length);
void convertLowerCaseToUpper(unsigned char* dest, unsigned char* src, uint16_t length);
unsigned char char2hex(unsigned char convChar);
uint8_t isAlphaNumericCharacter(uint8_t byte);
uint8_t isHexaNumericCharacter(uint8_t byte);
uint8_t isSpecialCharacter(uint8_t byte);
uint8_t convertCharToDecimal(uint8_t byte);
uint8_t convertHexStringByteToDecimal(const uint8_t* buffer);
uint64_t convertHexStringBytesToDecimal(const uint8_t* buffer, uint8_t byteCnt);
uint64_t convertASCIIStringBytesToDecimal(const uint8_t* buffer, uint8_t byteCnt);
void convertHexStringBytesToHexBytes(uint8_t* hex_buffer, const uint8_t* hex_string_buffer, uint8_t byteCnt);
uint16_t calculate_crc(uint8_t* data, int Length);
uint8_t validate_checksum(uint8_t *data, uint16_t length, uint16_t actual_crc);
int32_t convertStringToValue(char* str);
unsigned short convertToFloat(char* data, float *result);
uint16_t getNthFeild(uint8_t* src_buffer, uint8_t* dest_buffer, uint8_t separator,
                                            uint8_t feild_id, uint16_t src_length, uint16_t dest_length);
uint32_t getDiffrence(uint32_t min, uint32_t max);
uint32_t getu16_Diffrence(uint16_t min, uint16_t max);
char get_segment_by_index(char *data_in, uint8_t index, char *segment, uint8_t max_len);

float get_value_based_on_percentage(float full_value, float percentage);

void * swapbytes(void *inp, size_t len);
void * swapbytesout(void * inp, void *out, size_t len);
void print_in_hex(uint8_t *buffer, uint8_t len) ;

uint32_t get_unused_record_count(uint32_t read_index, uint32_t write_index, uint32_t max_records_count);
uint32_t get_used_record_count(uint32_t read_index, uint32_t write_index, uint32_t max_records_count);
uint32_t get_index_location_address(uint32_t base_location_address, uint32_t current_record_index, uint32_t record_size);
uint32_t get_next_record_index(uint32_t current_record_index, uint32_t each_record_size, uint32_t available_memory);
uint32_t get_previous_record_index(uint32_t current_record_index, uint32_t each_record_size, uint32_t available_memory);
uint32_t getNextLocationID(uint32_t location, uint32_t noOfRecords);
uint32_t getLocationAddress(uint32_t start_address, uint32_t record_id, uint32_t record_size);
uint16_t get_avg_Of_uint16(uint16_t* buffer, uint16_t length, uint16_t cluster_count, uint16_t cluster_offset);

int convertDateTimeToString(char* buffer, Custom_Date_Time* currentDateTime);
int convertRTCDateTimeNowToString(char* buffer, RTC_DATE_TIME_DOW* currentDateTime);
bool isValidDateTime(RTC_DATE_TIME_DOW rtcObj);

void printTobaseWithPadding (long n, uint8_t base, uint8_t* destination, uint8_t padding_bytes, uint8_t decimal_point);
void printToFloat (long n, uint8_t* destination, uint8_t decimal_point);


int base64_decode(char *input, uint8_t *output);
int base64_encode(uint8_t *input, int input_len, char *output);
#endif /* CUSTOM_INC_COMMON_H_ */
