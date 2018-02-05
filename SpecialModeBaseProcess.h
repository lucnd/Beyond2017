#ifndef SPECIALMODEBASEPROCESS_H
#define SPECIALMODEBASEPROCESS_H

#include "SpecialModeServicesManager.h"
#include "ISpecialModeApplication.h"
#include "SpecialModeDataType.h"
#include "Log.h"

class SpecialModeBaseProcess
{

public:
    SpecialModeBaseProcess();
    void initialize(sp<SpecialModeServicesManager> servicesMgr,ISpecialModeApplication* pApp, sp<sl::Handler> handler);
    virtual ~SpecialModeBaseProcess();
    virtual void doSpecialModeHandler(int32_t what, const sp<sl::Message>& message) = 0;
    virtual void handleEvent(uint32_t ev) = 0;
    virtual void handleTimerEvent(int timerId) = 0;

protected:
    sp<sl::Handler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    ISpecialModeApplication* mp_Application;
    TimerTimeoutHandler *mp_DemoModeTimer;
    virtual void initializeProcess() = 0;

    char* getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value);
    void setPropertyInt(const char* name, const int32_t i_value);
};


#endif // SPECIALMODEBASEPROCESS_H
