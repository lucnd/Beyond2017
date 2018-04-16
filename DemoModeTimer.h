#ifndef DEMOMODETIMER_H
#define DEMOMODETIMER_H

#include "SpecialModeBaseProcess.h"

class DemoModeTimer : public TimerTimeoutHandler
{
public:
    DemoModeTimer(SpecialModeBaseProcess& rProcess): mr_Process(rProcess){}
    ~DemoModeTimer(){}
    virtual void handlerFunction(int timerId);
    SpecialModeBaseProcess& mr_Process;

};

class DemoModeTimerSet
{
public:

    static const uint32_t TIMER_DEMOMODE_ACTIVE = 0;
    static const uint32_t TIMER_SVT_ANSWER= 1;

    DemoModeTimerSet();
    virtual ~DemoModeTimerSet();
    void initialize(TimerTimeoutHandler* pTimerHandler);
    void release();
    bool startTimer(uint32_t index, uint32_t startTimeout, uint32_t cyclicTimeout=0);
    bool stopTimer(uint32_t index);
    bool m_InitFlag;
    Timer* mp_DemoModeTimerSet[2];
};

#endif // DEMOMODETIMER_H
