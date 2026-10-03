#ifndef DELAYTIMER_H
#define DELAYTIMER_H
#include "common.h"
#define DelayTimer_t    uint64_t
#define  TIMER_INTERRUPT_INTERVAL   10

extern DelayTimer_t tick_count;

extern DelayTimer_t getCurrentTickCount();
extern DelayTimer_t setDelayInMSecs(DelayTimer_t addDelay);
extern DelayTimer_t setDelayInSecs(DelayTimer_t addDelay);
extern DelayTimer_t setDelayInMins(DelayTimer_t addDelay);
extern DelayTimer_t getDelayInMSecs(DelayTimer_t delayVar);
extern DelayTimer_t getDelayInSecs(DelayTimer_t delayVar);
extern DelayTimer_t getDelayInMins(DelayTimer_t delayVar);
extern bool isDelayActive(DelayTimer_t delayVar);

#endif // DELAYTIMER_H
