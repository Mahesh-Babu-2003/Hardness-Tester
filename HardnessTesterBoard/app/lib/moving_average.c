#include "moving_average.h"


void MA_Init(MovingAvg_t *ma)
{
    ma->sum   = 0;
    ma->idx   = 0;
    ma->count = 0;
    for (int i = 0; i < MA_SIZE; i++)
        ma->buf[i] = 0;
}

uint32_t MA_Update(MovingAvg_t *ma, uint32_t sample)
{
    ma->sum -= ma->buf[ma->idx];
    ma->buf[ma->idx] = sample;
    ma->sum += sample;

    ma->idx = (ma->idx + 1) % MA_SIZE;

    if (ma->count < MA_SIZE)
        ma->count++;

    return ma->sum / ma->count;   // avoids divide-by-zero at startup
}
