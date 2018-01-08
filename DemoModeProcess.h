#ifndef DEMOMODEPROCESS_H
#define DEMOMODEPROCESS_H

#include "SpecialModeDataType.h"
#include "SpecialModeBaseProcess.h"
#include "DemoModeTimer.h"

#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"

#include <utils/atoi.h>
#include <time.h>

class DemoModeProcess : public SpecialModeBaseProcess
{
public:
    DemoModeProcess();
    virtual ~DemoModeProcess();
    virtual void handleEvent(uint32_t eventId);
    virtual void handleTimerEvent(int timerId);
    virtual void do_SpecialModeHandler(int32_t what, sp<sl::Message>& messages);

    void DemoModeStart();
    void DemoModeStop();
    bool TurnOnDemoMode(uint8_t timeUnit, int32_t m_time);
    bool TurnOffDemoMode();

    // utilities
    int byteToInt1(byte data[], int idx);
    void demoModeClockReset();
    void sendResponse(int32_t what, const sp<sl::Message>& messages);

private:
    DemoModeTimerSet m_TimerSet;
    int32_t mRunningTime;
    int32_t mTimeUnit;

protected:
    virtual void initializeProcess();
    DEMO_MODE_STATUS mDemoModeStatus;
    DEMO_MODE_STATUS getDemoModeStatus();
    void setDemoModeStatus(DEMO_MODE_STATUS status);
};

#endif // DEMOMODEPROCESS_H
