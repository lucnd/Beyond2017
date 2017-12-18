/**
* \file    DemoModeProcess.h
* \brief     Declaration of DemoModeProcess
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

#if !defined(EA_904251EB_8C75_42e4_B344_C0B505825133__INCLUDED_)
#define EA_904251EB_8C75_42e4_B344_C0B505825133__INCLUDED_

#include "SpecialModeData_type.h"
#include "SpecialModeBaseProcess.h"
#include "DemoModeTimer.h"
#include "Handler.h"
#include "Message.h"

#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"

#include "atoi.h"
#include <time.h>

class DemoModeProcess : public SpecialModeBaseProcess
{

public:
    DemoModeProcess();
    virtual ~DemoModeProcess();

    void do_KeepPower();
    void do_releaseKeepPower();
    virtual void handleEvent(uint32_t ev);
    virtual void handleTimerEvent(int timerId);
    virtual void do_SpecialModeHandler(int32_t what, const sp<sl::Message>& message);
    void DemoModeStart();
    bool TurnOnDemoMode(uint8_t timeUnit, int32_t m_time);
    void DemoModeStop();
    bool TurnOffDemoMode();
    void sendResponse(int32_t what, const sp<sl::Message>& message);

    void activatePSIM();
    void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot);
    void onActiveSimStateChanged(int32_t activeSim);
    void onPsimLockStateChanged(bool locked);

    int byteToInt1(byte data[], int idx);
    void checkEBcallStatus();

    void registerSVTpostReceiver();
    void demoModeClockReset();
    void setDisableEbcallStatus();
    uint8_t getSVTConfig(const char* name);

protected:
    virtual void initializeProcess();

    void setDemoStatus(DEMO_MODE_STATUS status);
    DEMO_MODE_STATUS getDemoStatus();
    DEMO_MODE_STATUS mDemoModeStatus;

    void setWifiStatus(WCONNECT_STATUS status);
    WCONNECT_STATUS getWifiStatus();
    WCONNECT_STATUS mWifiStatus;

private:
    DemoModeTimerSet m_TimerSet;
    KeepPower* mp_KeepPower;

    sp<TelephonyManager::PsimLock> m_psimLock;
    bool m_pSimCheck;

    bool mCheckPower;
    int32_t mRunningTime;
    uint8_t mTimeUnit;

    bool bSVTRegOnDemo;
    bool bSVTAlertStatus;

};

#endif // !defined(EA_904251EB_8C75_42e4_B344_C0B505825133__INCLUDED_)
