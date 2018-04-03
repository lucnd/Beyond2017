#ifndef DEMOMODEPROCESS_H
#define DEMOMODEPROCESS_H

#include "SpecialModeDataType.h"
#include "SpecialModeBaseProcess.h"
#include "DemoModeTimer.h"
#include <utils/Message.h>
#include <utils/Handler.h>
#include <utils/atoi.h>
#include <time.h>

#include <services/PowerManagerService/PowerLock.h>

class DemoModeProcess : public SpecialModeBaseProcess
{
public:
    DemoModeProcess();
    virtual ~DemoModeProcess();
    // virtual void handleEvent(uint32_t ev);
    virtual void handleTimerEvent(int timerId);
    virtual void doSpecialModeHandler(const sp<sl::Message>& message);
    void demoModeStart();
    void demoModeStop();
    void sendResponse(int32_t what, const sp<sl::Message>& message);
    void checkBcallStatus();
    void checkEcallStatus();
    void demoModeClockReset();
    void disableBCall();
    void disableECall();
    bool turnOnDemoMode(uint8_t timeUnit, int32_t timeValue);
    bool turnOffDemoMode();
    void lockPowerMode();
    void releasePoweMode();
    void onWiFiStateOn();
    void onWiFiStateOff();
    void onConfigChange(const sp<sl::Message>& message);

#ifdef G_TEST
public:
    DemoModeTimerSet    m_TimerSet;
    bool                m_pSimCheck;
    bool                m_CheckPower;
    bool                m_SVTRegOnDemo;
    bool                m_SVTAlertStatus;
    int32_t             m_RunningTime;
    uint8_t             m_TimeUnit;
    PowerLock*          mp_PowerLock;

public:
    virtual void initializeProcess();
    void setDemoStatus(DemoModeStatus status);
    void setWifiStatus(WiFiStatus status);
    DemoModeStatus getDemoStatus();
    DemoModeStatus m_DemoModeStatus;
    WiFiStatus getWifiStatus();
    WiFiStatus m_WifiStatus;
#else
private:
    DemoModeTimerSet        m_TimerSet;
    bool                    m_pSimCheck;
    bool                    m_CheckPower;
    bool                    m_SVTRegOnDemo;
    bool                    m_SVTAlertStatus;
    int32_t                 m_RunningTime;
    uint8_t                 m_TimeUnit;
    PowerLock*              mp_PowerLock;

protected:
    virtual void initializeProcess();

    void setDemoStatus(DemoModeStatus status);
    void setWifiStatus(WiFiStatus status);
    WiFiStatus getWifiStatus();
    DemoModeStatus getDemoStatus();
    DemoModeStatus      m_DemoModeStatus;
    WiFiStatus          m_WifiStatus;
#endif
};

#endif // DEMOMODEPROCESS_H
