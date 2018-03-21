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
    virtual void handleEvent(uint32_t ev);
    virtual void handleTimerEvent(int timerId);
    virtual void doSpecialModeHandler(int32_t what, const sp<sl::Message>& message);

    void demoModeStart();
    void demoModeStop();

    void checkBcallStatus();
    void checkEcallStatus();

    void sendResponse(int32_t what, const sp<sl::Message>& message);
    void registerSVTpostReceiver();
    void demoModeClockReset();

    void disableBCall();
    void disableECall();

    bool turnOnDemoMode(uint8_t timeUnit, int32_t timeValue);
    bool turnOffDemoMode();

    int byteToInt1(byte data[], int idx);
    uint8_t getSVTConfig(const char* name);

    void lockPowerMode();
    void releasePoweMode();

private:
    DemoModeTimerSet    m_TimerSet;
    PowerLock*          mp_PowerLock;
    int32_t             m_RunningTime;
    uint8_t             m_TimeUnit;
    bool                m_pSimCheck;
    bool                m_CheckPower;
    bool                m_SVTRegOnDemo;
    bool                m_SVTAlertStatus;

protected:
    virtual void initializeProcess();
    void setDemoStatus(DemoModeStatus status);
    void setWifiStatus(WiFiStatus status);
    DemoModeStatus getDemoStatus();

    DemoModeStatus  m_DemoModeStatus;
    WiFiStatus      getWifiStatus();
    WiFiStatus      m_WifiStatus;
};

#endif // DEMOMODEPROCESS_H
