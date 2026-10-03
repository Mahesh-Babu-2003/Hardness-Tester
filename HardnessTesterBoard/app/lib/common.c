#include "common.h"


uint64_t get_abs_diffrence(uint64_t val1, uint64_t val2){
	if(val1 >= val2){
		return (val1-val2);
	}else{
		return (val2-val1);
	}
}

uint32_t getDiffrence(uint32_t min, uint32_t max){
    if(max  >= min){
        return (max-min);
    }else{
        return (UINT32_MAX - max + min);
    }
}
uint32_t getu16_Diffrence(uint16_t min, uint16_t max){
    if(max  >= min){
        return (max-min);
    }else{
        return (UINT16_MAX - max + min);
    }
}

uint16_t getAverageOfSamples(uint16_t* samples, uint16_t sampleCount){
	uint16_t ix = 0;
	uint64_t value = 0;
	if(sampleCount > 0){
		for(ix = 0; ix < sampleCount; ++ix){
			value += samples[ix];
		}
		value = value/sampleCount;
	}
		return value;
}

uint16_t getHighestOfSamples(uint16_t* samples, uint16_t sampleCount){
		uint16_t ix = 0;
		uint16_t value = 0;

	for(ix = 0; ix < sampleCount; ++ix){
		if(value < samples[ix]){
			value = samples[ix];
		}
	}

	return value;
}

uint16_t getPeekToPeekOfSamples(uint16_t* samples, uint16_t sampleCount){
		uint16_t ix = 0;
		uint16_t max_value = 0;
		uint16_t min_value = 0xFFFFU;

	for(ix = 0; ix < sampleCount; ++ix){
		if(max_value < samples[ix]){
			max_value = samples[ix];
		}
		if(min_value > samples[ix]){
			min_value = samples[ix];
		}
	}
	if(min_value == 0xFFFFU){
		min_value = 0;
	}

	return (max_value - min_value);
}


bool find_mean_from_peak_to_peak(uint16_t *samples, uint16_t sampleCount, uint16_t* meanValue){
	uint16_t ix = 0;
	uint16_t peakCount  = 0;
	bool result = false;
	uint16_t peak1_ix = 0, peak2_ix = 0;
	uint64_t sumValue = 0;
	bool found = false;

	if (sampleCount > 2) {
		for (ix = 0; ix < sampleCount - 2; ++ix) {
			if ((found == false) && (samples[ix] > samples[ix+1]) && (samples[ix+1] > samples[ix+2])) {
				if(peakCount == 0){
					peak1_ix = ix;
					++peakCount;
					found = true;
				}else if((peakCount == 1) && ((ix - peak1_ix) > 4) ){
					peak2_ix = ix;
					++peakCount;
					break;
				}else{
					peak1_ix = ix;
					found = true;
				}
			}else{
				found = false;
			}
		}

		if(peakCount == 2){
			uint16_t count = 0;
			for(ix = peak1_ix; ix <= peak2_ix; ++ix){
				sumValue += samples[ix];
				++count;
			}
			*meanValue = sumValue/count;
			result = true;
		}
	}

	return result;
}


void fill_sin_values(uint16_t* buffer, uint16_t sample_count, uint16_t max_value){
	for(uint16_t ix = 0; ix < sample_count; ++ix){
		//(sin(x*2*PI/sample_count) + 1)*((0xFFF + 1)/2)
		buffer[ix] = (sin(ix*2*PI/sample_count) + 1)*(max_value/2);
//		buffer[ix] = 2048;
	}
}

void fill_triangle_values(uint16_t* buffer, uint16_t sample_count, uint16_t max_value){
	uint16_t step_value = max_value/sample_count;
	for(uint16_t ix = 0; ix < sample_count; ++ix){
		buffer[ix] = ix*step_value;
		//		buffer[ix] = 2048;
	}
}


void printTobase (long n, uint8_t base, uint8_t* destination)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];

  *s = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--s = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  strcpy((char*)destination, s);
}


bool isValueWithInBoundaies(uint32_t current_time, uint32_t start_time, uint32_t end_time){
	bool result = false;
	if(start_time < end_time){
		if((current_time >= start_time) && (current_time <= end_time)){
			result = true;
		}
	}else if(start_time > end_time){ 
		if((current_time >= start_time) || (current_time <= end_time)){
			result = true;
		}
	}
	return result;
}


void replace_character(char* data_ptr, uint16_t length, uint8_t old_character, uint8_t new_character){
	uint16_t ix = 0;
	for(ix = 0; ix < length; ++ix){
		if(data_ptr[ix] == old_character){
			data_ptr[ix] = new_character;
		}
	}
}

uint16_t calc_character_count(char* s, uint16_t length, uint8_t data) 
{ 
    // Count variable 
    int res = 0; 
  
    for (int i=0;i<length;i++) {
  
        // checking character in string 
        if (s[i] == data) 
            res++; 
		}
    return res; 
} 

unsigned short crc16(const uint8_t* data_p, uint16_t length) {
	unsigned char x;
	unsigned short crc = 0xFFFF;

	while (length--) {
		x = crc >> 8 ^ *data_p++;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((unsigned short) (x << 12))
				^ ((unsigned short) (x << 5)) ^ ((unsigned short) x);
	}
	return crc;
}

void memcpy_reverse(void* dest, const void* src, uint16_t length){
  uint16_t ix = 0;
  unsigned char* dest_ptr = (unsigned char*)dest;
  unsigned char* src_ptr = (unsigned char*)src;
  while(length > 0){
    --length;
    dest_ptr[ix] = src_ptr[length];
    ++ix;
  }
}

void bytes_to_value(void* dest, const void* src, uint8_t length)
{
  memcpy_reverse(dest, src, length);//little endian to big endian
}

void value_to_bytes(void* dest, const void* src, uint8_t length)
{
  memcpy_reverse(dest, src, length); //big endian to little endian
}

void unsigned_value_to_hex(uint64_t value, uint8_t* hexbuf, uint8_t length){
  if(length  > sizeof(value)){
    length = sizeof(value);
  }
  value_to_bytes(hexbuf, &value, length);
}

void signed_value_to_hex(int64_t value, uint8_t* hexbuf, uint8_t length){
  if(length  > sizeof(value)){
    length = sizeof(value);
  }
  value_to_bytes(hexbuf, &value, length);
}

void long8byteshex(uint64_t value, uint8_t* hexbuf){
  value_to_bytes(hexbuf, &value, sizeof(value));
}

void long4byteshex(uint32_t value, uint8_t* hexbuf)
{
  value_to_bytes(hexbuf, &value, sizeof(value));
}

void long2byteshex(uint16_t value, uint8_t* hexbuf)
{
  value_to_bytes(hexbuf, &value, sizeof(value));
}


int64_t bytes_to_singed_value(const void* src, uint8_t length)
{
  int64_t value = 0;
  if(length  > sizeof(value)){
    length = sizeof(value);
  }
  bytes_to_value(&value, src, length);//little endian to big endian
  return value;
}

uint64_t bytes_to_unsinged_value(const void* src, uint8_t length)
{
  uint64_t value = 0;
  if(length  > sizeof(value)){
    length = sizeof(value);
  }
  bytes_to_value(&value, src, length);//little endian to big endian
  return value;
}


bool validate_time(uint8_t* buffer, bool is_reverse){
	if(is_reverse == true){
		//sec && min && hr
		return ((buffer[0] <= 59) && (buffer[1] <= 59) && (buffer[2] <= 23)) ;
	}else{
		//hr && min && sec
		return ((buffer[0] <= 23) && (buffer[1] <= 59) && (buffer[2] <= 59));
	}
}

bool validate_date(uint8_t* buffer, bool is_reverse){
	if(is_reverse == true){
		//date && month && year
		return (buffer[0] <= 31) && (buffer[0] > 0) && (buffer[1] > 0) && (buffer[1] <= 12) && (buffer[2] < 100);
	}else{
		//year && month && date
		return (buffer[0] < 100) && (buffer[1] > 0) && (buffer[1] <= 12)  && (buffer[2] > 0) && (buffer[2] <= 31);
	}
}

bool validate_date_time(uint8_t* buffer, bool is_reverse){
	if(is_reverse == true){
		//sec && min && hr && date && month && year
		return (validate_time(buffer, is_reverse) && validate_date(&buffer[3], is_reverse));
	}else{
		//year && month && date && hr && min && sec
		return (validate_date (buffer, is_reverse) && validate_time(&buffer[3], is_reverse));
	}
}

uint32_t getTimestamp(int year, int month, int date, int hour, int min, int sec){

	time_t rawtime = 0;
	struct tm *timeinfo = localtime(&rawtime);
	timeinfo->tm_year = 100 + year % 100;
	if(month > 0){
		timeinfo->tm_mon = month - 1;
	}else{
		timeinfo->tm_mon = month;
	}
	timeinfo->tm_mday = date;

	timeinfo->tm_hour = hour;
	timeinfo->tm_min = min;
	timeinfo->tm_sec = sec;

	return mktime(timeinfo);
}

unsigned short ModBus_CRC16( unsigned char * Buffer, unsigned short Length )
{
    /* ModBus_CRC16 Calculatd CRC16 with polynome 0xA001 and init value 0xFFFF
Input *Buffer - pointer on data
Input Lenght - number byte in buffer
Output - calculated CRC16
*/
    unsigned int cur_crc;
    cur_crc=0xFFFF;
    do
    {
        unsigned int i = 8;
        cur_crc = cur_crc ^ *Buffer++;
        do
        {
            if (0x0001 & cur_crc)
            {
                cur_crc >>= 1;
                cur_crc ^= 0xA001;
            }
            else
            {
                cur_crc >>= 1;
            }
        }
        while (--i);
    }
    while (--Length);
    return cur_crc;
}


void convertUpperCaseToLower(unsigned char* dest, unsigned char* src, uint16_t length){
  for(uint16_t ix = 0; ix < length; ++ix){
    if(src[ix] >= 'A' && src[ix] <= 'Z'){
      dest[ix] = src[ix] + 0x20;
    }else{
      dest[ix] = src[ix];
    }
  }
}

void convertLowerCaseToUpper(unsigned char* dest, unsigned char* src, uint16_t length){
  for(uint16_t ix = 0; ix < length; ++ix){
    if(src[ix] >= 'a' && src[ix] <= 'z'){
      dest[ix] = src[ix] - 0x20;
    }else{
      dest[ix] = src[ix];
    }
  }
}

unsigned char char2hex(unsigned char convChar)
{
  if(convChar >= '0' && convChar <= '9') convChar -= 0x30;
  else if(convChar >= 'a' && convChar <= 'f') convChar = convChar - 'a'+10;
  else if(convChar >= 'A' && convChar <= 'F') convChar = convChar - 'A'+10;
  return convChar;
}

uint8_t isHexaNumericCharacter(uint8_t byte){
	bool result = true;
  if((byte >= '0') && (byte <= '9')){
  }else if((byte >= 'a') && (byte <= 'f')){
  }else if((byte >= 'A') && (byte <= 'F')){
  }else{
	  result = false;
  }
  return result;
}

uint8_t isAlphaNumericCharacter(uint8_t byte){
	bool result = true;
  if((byte >= '0') && (byte <= '9')){
  }else if((byte >= 'a') && (byte <= 'z')){
  }else if((byte >= 'A') && (byte <= 'Z')){
  }else{
	  result = false;
  }
  return result;
}

uint8_t isSpecialCharacter(uint8_t byte){
	uint8_t result = true;
	switch (byte) {
	case '-':
		break;
	case '/':
		break;
	case '*':
		break;
	case '#':
		break;
	case ',':
		break;
	case '.':
		break;
	case ' ':
		break;
	case '(':
		break;
	case ')':
		break;
	default:
		result = false;
		break;
	}

	return result;
}


uint8_t convertCharToDecimal(uint8_t byte){
  if((byte >= '0') && (byte <= '9')){
    byte = byte - '0';
  }else if((byte >= 'a') && (byte <= 'f')){
    byte = 10 + byte - 'a';
  }else if((byte >= 'A') && (byte <= 'F')){
    byte = 10 + byte - 'A';
  }
  return byte;
}

uint8_t convertHexStringByteToDecimal(const uint8_t* buffer){
  return ((convertCharToDecimal(buffer[0]) << 4) + convertCharToDecimal(buffer[1]));
}

void getValueToString(int64_t value, char* buffer, uint8_t length){
    uint8_t temp_buffer[32] = { 0 };
    uint8_t ix = 0, dIx = 0;
    memset(buffer, 0, length);
    if(value < 0){
        buffer[dIx++] = '-';
        value = value * -1;
    }

    do{
        temp_buffer[ix++] = value % 10 + '0';
        value = value/10;
    }while(value > 0);
    temp_buffer[ix] = 0;

    if(ix > 0){
        do{
            buffer[dIx++] =  temp_buffer[ix - 1];
            --ix;
        }while(ix > 0);
    }
    buffer[dIx] = 0;
}

uint64_t convertHexStringBytesToDecimal(const uint8_t* buffer, uint8_t byteCnt){
  uint64_t value = 0;
  uint8_t ix = 0;
  while(byteCnt > 1){
    byteCnt -= 2;
    value = (value*256) + ((convertCharToDecimal(buffer[ix]) << 4) + convertCharToDecimal(buffer[ix+1]));
    ix += 2;
  }
  return value;
}

uint64_t convertASCIIStringBytesToDecimal(const uint8_t* buffer, uint8_t byteCnt){
  uint64_t value = 0;
  uint8_t ix = 0;
  while(byteCnt > 0 && (isAlphaNumericCharacter(buffer[ix]) == true)){
    --byteCnt;
    value = (value * 10) + convertCharToDecimal(buffer[ix]);
    ++ix;
  }
  return value;
}

void convertHexStringBytesToHexBytes(uint8_t* hex_buffer, const uint8_t* hex_string_buffer, uint8_t byteCnt){
  uint8_t ix = 0;
  while(byteCnt > 1){
    byteCnt -= 2;

    //((convertCharToDecimal(buffer[ix]) << 4) + convertCharToDecimal(buffer[ix+1]));
    //ix += 2;

    hex_buffer[ix] = convertHexStringByteToDecimal(&hex_string_buffer[ix*2]);
    ix += 1;
  }
}


uint16_t calculate_crc(uint8_t* data, int Length)
{
  uint16_t checksum=0xFFFF,Genpoly=0xA001;//,iBufferIndex,iBitIndex =0;
  uint16_t cntIndex,i=0;
  while (Length--) // for each data byte in string
  {
    checksum = checksum ^ (uint8_t)data[i];
    for (cntIndex = 0; cntIndex < 8; cntIndex++)  //for each of the 8 bits
    {
      if ((checksum & 0x0001) == 1)
        checksum = (checksum >> 1) ^ Genpoly;
      else
        (checksum = checksum >> 1);
    }
    i++;
  }
  return checksum;
}

uint8_t validate_checksum(uint8_t *data, uint16_t length, uint16_t actual_crc){
  return (calculate_crc(data, length) == actual_crc);
}

int32_t convertStringToValue(char* str) {
    return strtol(str, NULL, 10);
}

unsigned short convertToFloat(char* data, float *result) {
    unsigned short byteCnt = 0;
    bool isDecimalPt = false;
    unsigned char numberOfDecimals = 0;

    float val = 0;
    while (((*data >= '0') && (*data <= '9')) || (*data == '.')) {
        if (*data == '.') {
            isDecimalPt = true;
        } else {
            if (numberOfDecimals < MAX_VALUE_PRECISION) {
                if (isDecimalPt) {
                    ++numberOfDecimals;
                }
                val = val * 10 + *data - '0';
            }
        }
        ++data;
        ++byteCnt;
    }
    while (numberOfDecimals > 0) {
        val = val / 10.0;
        --numberOfDecimals;
    }
    *result = val;
    return byteCnt;
}


//starts from 0th feild_id
uint16_t getNthFeild(uint8_t* src_buffer, uint8_t* dest_buffer, uint8_t separator,
                                            uint8_t feild_id, uint16_t src_length, uint16_t dest_length){

    memset(dest_buffer, 0x0, dest_length);
    uint8_t feildCnt = 0;
    uint16_t src_ix= 0, dest_ix = 0;
    bool is_feild_started = false;

    if(feild_id == 0){
        is_feild_started = true;
    }

    while(src_ix < src_length){

        if(src_buffer[src_ix] == separator){
            ++feildCnt;
            //moved to next feild
            if(is_feild_started == true){
                break;
            }
            if(feildCnt == feild_id){
                is_feild_started = true;
                ++src_ix;
            }
        }

        if(is_feild_started == true){
            if( src_ix >= src_length ||  src_buffer[src_ix] == separator){
                break;
            }
            dest_buffer[dest_ix++] = src_buffer[src_ix];
            if((dest_length > 0) && (dest_ix >= dest_length)){
                break;
            }
        }
        ++src_ix;
    }
        if((is_feild_started == true) && (dest_ix < dest_length)){
            dest_buffer[dest_ix] = '\0';
        }
    return dest_ix;
}


char get_segment_by_index(char *data_in, uint8_t index, char *segment, uint8_t max_len) {
    char *p = NULL, *q = NULL;
    uint8_t i = 0;

    p = data_in;
    q = data_in;

    while (i < index) {
        if ((q == NULL) || (*q == '\0')) {
            break;
        }
        if (*q == ',') {
            ++i;
        }
        ++q;
    }
    p = q;

    if ((i < index) || (q == NULL)) {
        return -1; // Segment is not exist;
    } else {
        while (*q != ',') {
            if ((q == NULL) || (*q == '\0')) {
                break;
            }
            ++q;
        }
        uint8_t dest_len = q - p;
        if ((*p == ',') && (dest_len == 1)) {
            return 0; // Segment is empty which is between ',' and ',' or ',' and '*'.
        } else if (dest_len < max_len) {
            memset(segment, 0, max_len);
            memcpy(segment, p, dest_len);
            return dest_len;
        } else {
            return -2; // Segment is longer than max_len!
        }
    }
    return -100; // Unknow error!
}

/* This function swaps and modifies the content of inp
 *
 * Returns: a pointer to the output value (inp) */
void * swapbytes(void *inp, size_t len)
{
    unsigned int i;
    unsigned char *in=(unsigned char *)inp,tmp;

    for(i=0;i<len/2;i++) {
        tmp=*(in+i);
        *(in+i)=*(in+len-i-1);
        *(in+len-i-1)=tmp;
    }

    return inp;
}

/* This function doesn't swap and doesn't modify the content
 * of inp, the output is computed on out.
 *
 * Returns: a pointer to the output value (out) */
void * swapbytesout(void *inp, void *out, size_t len)
{
    unsigned int i;
    unsigned char *o=(unsigned char *)out;
    unsigned char *in=(unsigned char *)inp;

    for(i=0;i<len;i++) {
        *(o+len-i-1)=*(in+i);
    }

    return out;
}

uint32_t get_used_record_count(uint32_t read_index, uint32_t write_index, uint32_t max_records_count){
	uint32_t available_records_count = 0;
	if(read_index < max_records_count && write_index < max_records_count){
		if( write_index >= read_index){
			available_records_count = write_index - read_index;
		}else{
			available_records_count = max_records_count - (read_index - write_index) ;
		}
	}
	return available_records_count;
}

uint32_t get_unused_record_count(uint32_t read_index, uint32_t write_index, uint32_t max_records_count){
	uint32_t available_records_count = 0;

	if(max_records_count > 1){
		available_records_count = max_records_count - get_used_record_count(read_index, write_index, max_records_count) - 1; //used for index
	}

	return available_records_count;
}

uint32_t getNextLocationID(uint32_t record_id, uint32_t noOfRecords){
	++record_id;
	if(record_id >= noOfRecords){
		record_id = 0;
	}
	return record_id;
}


uint32_t get_previous_record_index(uint32_t current_record_index, uint32_t each_record_size, uint32_t available_memory){
	uint32_t maximum_available_records = available_memory/each_record_size;
	if(current_record_index > 0){
		--current_record_index;
	}else{
		current_record_index = maximum_available_records - 1;
	}
	return current_record_index;
}

uint32_t get_next_record_index(uint32_t current_record_index, uint32_t each_record_size, uint32_t available_memory){
	uint32_t maximum_available_records = available_memory/each_record_size;
	++current_record_index;
	if(current_record_index >= maximum_available_records){
		current_record_index = 0;
	}
	return current_record_index;
}

uint32_t get_index_location_address(uint32_t base_location_address, uint32_t current_record_index, uint32_t record_size){
	return base_location_address + (current_record_index * record_size);
}
uint32_t getLocationAddress(uint32_t start_address, uint32_t record_id, uint32_t record_size){
	return start_address + (record_id*record_size);
}

//void print_in_hex(uint8_t *buffer, uint8_t len) {
//	memset(temp_buffer, 0, sizeof(temp_buffer));
//	uint32_t temp_ix = 0;
//	for (uint8_t ix = 0; ix < len; ++ix) {
//		temp_ix += sprintf((char*) &temp_buffer[temp_ix], " %02x", buffer[ix]);
//	}
//}


uint16_t get_avg_Of_uint16(uint16_t* buffer, uint16_t total_sample_count, uint16_t cluster_sample_count, uint16_t cluster_offset){
	uint64_t average = 0;
	if(cluster_sample_count == 0){
		cluster_sample_count = 1;
	}
	if(total_sample_count > 0){
		for(uint16_t ix = 0; (ix+cluster_offset) < total_sample_count; ix += cluster_sample_count){
			average += buffer[ix + cluster_offset];
		}
		average = (average * cluster_sample_count)/total_sample_count;
	}
	return average;
}

float get_value_based_on_percentage(float full_value, float percentage){
	return (full_value*percentage)/100.0f;
}

double compute_rms_with_cluster(uint16_t *samples, uint16_t sampleCount, uint16_t mean_adc,
		uint16_t cluster_sample_count, uint16_t cluster_offset){
	uint16_t ix = 0;
	uint64_t voltage = 0;
	double vrms = 0;
	if(cluster_sample_count == 0){
		cluster_sample_count = 1;
	}
	if (sampleCount > 0) {
		for (ix = 0; ix < sampleCount; ix = ix + cluster_sample_count) {
			uint32_t diffrence = get_abs_diffrence(samples[ix+cluster_offset], mean_adc);
			voltage += diffrence * diffrence;
		}
		voltage = (voltage * cluster_sample_count) / sampleCount;
		vrms = sqrt(voltage);
	}
	return vrms;
}

double compute_rms_with_samples(uint16_t *samples, uint16_t sampleCount, uint16_t mean_adc) {
	return compute_rms_with_cluster(samples, sampleCount, mean_adc, 1, 0);
}


int convertDateTimeToString(char* buffer, Custom_Date_Time* currentDateTime){
	return sprintf(buffer, "%02d/%02d/20%02d %02d:%02d:%02d",
	    			currentDateTime->date, currentDateTime->month, currentDateTime->year,
					currentDateTime->hour, currentDateTime->min, currentDateTime->sec);
}

int convertRTCDateTimeNowToString(char* buffer, RTC_DATE_TIME_DOW* currentDateTime){
	return sprintf(buffer, "%02d/%02d/20%02d %02d:%02d:%02d",
	    			currentDateTime->date, currentDateTime->month, currentDateTime->year,
					currentDateTime->hour, currentDateTime->min, currentDateTime->sec);
}

bool isValidDateTime(RTC_DATE_TIME_DOW rtcObj){
    bool result = false;
    if(rtcObj.year == 0 || rtcObj.month == 0
            || rtcObj.month > 12 || rtcObj.date == 0 || rtcObj.date > 31
            || rtcObj.hour >= 24 || rtcObj.min >= 60){

    }else{
        uint8_t month_dates[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if(rtcObj.date <= month_dates[rtcObj.month-1]){
            if(rtcObj.month == 2){
                if(rtcObj.year % 4 == 0 && rtcObj.date <= 29){
                    result = true;
                }else if(rtcObj.year % 4 != 0 && rtcObj.date <= 28){
                    result = true;
                }
            }else{
                result = true;
            }
        }
    }

    return result;
}

void printToFloat (long n, uint8_t* destination, uint8_t decimal_point){
	printTobaseWithPadding(n, 10, destination, 0, decimal_point);
}
void printTobaseWithPadding (long n, uint8_t base, uint8_t* destination, uint8_t padding_bytes, uint8_t decimal_point)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];
  uint8_t current_decimal_place = 0;
  *s = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
	++current_decimal_place;
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--s = c < 10 ? c + '0' : c + 'A' - 10;
    if(current_decimal_place == decimal_point){
    	*--s = '.';
    }
    if(padding_bytes){
    	--padding_bytes;
    }
  } while(n || (current_decimal_place <= decimal_point && decimal_point > 0));

  while(padding_bytes > 0){
	  --padding_bytes;
	  *--s = '0';
  }

  strcpy((char*)destination, s);
}


static const char base64_table[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int base64_encode(uint8_t *input, int input_len, char *output)
{
    int i = 0, j = 0;

    while (i < input_len)
    {
        uint32_t octet_a = i < input_len ? input[i++] : 0;
        uint32_t octet_b = i < input_len ? input[i++] : 0;
        uint32_t octet_c = i < input_len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        output[j++] = base64_table[(triple >> 18) & 0x3F];
        output[j++] = base64_table[(triple >> 12) & 0x3F];
        output[j++] = base64_table[(triple >> 6) & 0x3F];
        output[j++] = base64_table[(triple) & 0x3F];
    }

    int mod = input_len % 3;
    if (mod)
    {
        output[j - 1] = '=';
        if (mod == 1)
            output[j - 2] = '=';
    }

    output[j] = '\0';
    return j;
}


static const int8_t b64_index[256] = {
    ['A']=0,['B']=1,['C']=2,['D']=3,['E']=4,['F']=5,['G']=6,['H']=7,
    ['I']=8,['J']=9,['K']=10,['L']=11,['M']=12,['N']=13,['O']=14,['P']=15,
    ['Q']=16,['R']=17,['S']=18,['T']=19,['U']=20,['V']=21,['W']=22,['X']=23,
    ['Y']=24,['Z']=25,
    ['a']=26,['b']=27,['c']=28,['d']=29,['e']=30,['f']=31,['g']=32,['h']=33,
    ['i']=34,['j']=35,['k']=36,['l']=37,['m']=38,['n']=39,['o']=40,['p']=41,
    ['q']=42,['r']=43,['s']=44,['t']=45,['u']=46,['v']=47,['w']=48,['x']=49,
    ['y']=50,['z']=51,
    ['0']=52,['1']=53,['2']=54,['3']=55,['4']=56,['5']=57,['6']=58,
    ['7']=59,['8']=60,['9']=61,
    ['+']=62,['/']=63
};

int base64_decode(char *input, uint8_t *output)
{
    int len = strlen(input);
    int i = 0, j = 0;

    while (i < len)
    {
        uint32_t sextet_a = input[i] == '=' ? 0 & i++ : b64_index[(uint8_t)input[i++]];
        uint32_t sextet_b = input[i] == '=' ? 0 & i++ : b64_index[(uint8_t)input[i++]];
        uint32_t sextet_c = input[i] == '=' ? 0 & i++ : b64_index[(uint8_t)input[i++]];
        uint32_t sextet_d = input[i] == '=' ? 0 & i++ : b64_index[(uint8_t)input[i++]];

        uint32_t triple = (sextet_a << 18) |
                          (sextet_b << 12) |
                          (sextet_c << 6) |
                          sextet_d;

        if (input[i-2] != '=')
            output[j++] = (triple >> 16) & 0xFF;
        if (input[i-1] != '=')
            output[j++] = (triple >> 8) & 0xFF;
        if (input[i] != '=')
            output[j++] = triple & 0xFF;
    }

    return j;
}
