#ifndef DEMOMODETIMER_H
#define DEMOMODETIMER_H

#include "SpecialModeBaseProcess.h"

#ifdef  G_TEST
class DemoModeTimer
#else
class DemoModeTimer : public TimerTimeoutHandler
#endif
{
public:
    DemoModeTimer(SpecialModeBaseProcess& rProcess): mr_Process(rProcess){}
    ~DemoModeTimer(){}
#ifndef G_TEST
    virtual void handlerFunction(int timerId);
#endif

#ifdef  G_TEST
public:
    SpecialModeBaseProcess& mr_Process;
#else
private:
    SpecialModeBaseProcess& mr_Process;
#endif
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

#ifdef  G_TEST
public:
    bool m_InitFlag;
    Timer* mp_DemoModeTimerSet[2];
#else
private:
    bool m_InitFlag;
    Timer* mp_DemoModeTimerSet[2];
#endif
};

#endif // DEMOMODETIMER_H
