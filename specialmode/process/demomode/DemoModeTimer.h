/**
* \file    DemoModeTimer.h
* \brief     Declaration of DemoModeTimer
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.13
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#if !defined(EA_6516176F_5F4D_44be_A1E5_7A180F0F7D36__INCLUDED_)
#define EA_6516176F_5F4D_44be_A1E5_7A180F0F7D36__INCLUDED_

#include "SpecialModeBaseProcess.h"

class DemoModeTimer : public TimerTimeoutHandler
{

public:
    DemoModeTimer(SpecialModeBaseProcess& rProcess): mr_Process(rProcess){};
    virtual ~DemoModeTimer(){};
    virtual void handlerFunction(int timerId);

private:
    SpecialModeBaseProcess & mr_Process;

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

private:
    bool m_InitFlag;
    Timer* mp_DemoModeTimerSet[2];
};

#endif // !defined(EA_6516176F_5F4D_44be_A1E5_7A180F0F7D36__INCLUDED_)
