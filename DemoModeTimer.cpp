#define LOG_TAG "SpecialModeApplication"
#include "DemoModeTimer.h"

DemoModeTimerSet::DemoModeTimerSet() : m_InitFlag(false),mp_DemoModeTimerSet{0}{
    LOGI("## DemoModeTimerSet created!");
}


DemoModeTimerSet::~DemoModeTimerSet(){
    LOGI("## DemoModeTimerSet closed!");
}

void DemoModeTimerSet::initialize(TimerTimeoutHandler* pTimerHandler)
{
    if(pTimerHandler == nullptr)
    {
        return;
    }
    LOGI("##DemoModeTimerSet DemoModeTimerSet initialize()");

#ifndef G_TEST
    mp_DemoModeTimerSet[0] = new Timer(pTimerHandler, TIMER_DEMOMODE_ACTIVE);
    mp_DemoModeTimerSet[1] = new Timer(pTimerHandler, TIMER_SVT_ANSWER);
#endif

    m_InitFlag  = true;
}

void DemoModeTimerSet::release()
{
    LOGI("##DemoModeTimerSet DemoModeTimerSet release()");
    if(m_InitFlag == false) {
        return;
    }
#ifndef G_TEST
    delete mp_DemoModeTimerSet[0];
    delete mp_DemoModeTimerSet[1];
#endif
}

bool DemoModeTimerSet::startTimer(uint32_t index, uint32_t startTimeout, uint32_t cyclicTimeout){
    LOGI("DemoModeTimerSet startTimer(), index[%d], timeout[%d sec] ====================", index, startTimeout);

#ifndef G_TEST
    mp_DemoModeTimerSet[index]->setDuration(startTimeout, cyclicTimeout);
    mp_DemoModeTimerSet[index]->start();
#endif

    return true;
}

bool DemoModeTimerSet::stopTimer(uint32_t index){
    LOGI("DemoModeTimerSet %s(), index[%d] ", __func__, index);

#ifndef G_TEST
    mp_DemoModeTimerSet[index]->stop();
#endif

    return true;
}

#ifndef G_TEST
void DemoModeTimer::handlerFunction(int timerId){
    LOGI("##DemoModeTimer %s(), timerId[%d]", __func__, timerId);
    mr_Process.handleTimerEvent(timerId);
}
#endif
