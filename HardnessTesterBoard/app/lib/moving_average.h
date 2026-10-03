/*
 * moving_average.h
 *
 *  Created on: Dec 16, 2025
 *      Author: aj
 */

#ifndef LIB_MOVING_AVERAGE_H_
#define LIB_MOVING_AVERAGE_H_
#include <stdint.h>
#define MA_SIZE 2

typedef struct
{
    uint64_t sum;
    uint32_t buf[MA_SIZE];
    uint16_t  idx;
    uint16_t  count;
} MovingAvg_t;


#endif /* LIB_MOVING_AVERAGE_H_ */
