#include "delaytimer.h"

DelayTimer_t tick_count = 0;
#define MAX_DELAY_ALLOWED   600000UL //10mins

//returns in msecs
DelayTimer_t getCurrentTickCount(){
    return tick_count;
}

DelayTimer_t setDelayInMSecs(DelayTimer_t addDelay){

    if(addDelay > 0 && addDelay <= TIMER_INTERRUPT_INTERVAL){
        addDelay = TIMER_INTERRUPT_INTERVAL;
    }
    return (getCurrentTickCount() + addDelay);
}

DelayTimer_t setDelayInSecs(DelayTimer_t addDelay){
    addDelay = addDelay *1000;
    return setDelayInMSecs(addDelay);
}

//returns in msecs
DelayTimer_t setDelayInMins(DelayTimer_t addDelay){
    addDelay = addDelay * 60;
    return setDelayInSecs(addDelay);
}

DelayTimer_t getDelayInMSecs(DelayTimer_t delayVar){
    DelayTimer_t curTickCount = getCurrentTickCount();
    if(delayVar > curTickCount){
        delayVar = delayVar - curTickCount;
    }
    else{
        delayVar = 0;
    }
    return delayVar;
}

DelayTimer_t getDelayInSecs(DelayTimer_t delayVar){
    return getDelayInMSecs(delayVar)/1000;
}

DelayTimer_t getDelayInMins(DelayTimer_t delayVar){
    return getDelayInSecs(delayVar)/60;
}

bool isDelayActive(DelayTimer_t delayVar){
    DelayTimer_t currentTick = getCurrentTickCount() ; 
    return (delayVar > currentTick); //&& (get_abs_diffrence(delayVar, currentTick) < MAX_DELAY_ALLOWED);
}
