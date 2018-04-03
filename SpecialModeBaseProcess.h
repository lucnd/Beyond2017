#ifndef SPECIALMODEBASEPROCESS_H
#define SPECIALMODEBASEPROCESS_H

#include "SpecialModeServicesManager.h"
#include "ISpecialModeApplication.h"
#include "SpecialModeDataType.h"
#include <string>
#include "Log.h"

class SpecialModeBaseProcess
{

public:
    SpecialModeBaseProcess();
    void initialize(sp<SpecialModeServicesManager> servicesMgr,ISpecialModeApplication* pApp);
    virtual ~SpecialModeBaseProcess();
    virtual void doSpecialModeHandler(const sp<sl::Message>& message) = 0;
    virtual void handleTimerEvent(int timerId) = 0;

#ifdef G_TEST
    sl::Handler*                         m_Handler;
#else
    sp<sl::Handler>                     m_Handler;
#endif
    sp<SpecialModeServicesManager>      m_ServicesMgr;
    ISpecialModeApplication*            mp_Application;
    TimerTimeoutHandler*                mp_DemoModeTimer;
    virtual void initializeProcess()    = 0;

    //char* getPropertyWrap(const char* name);
    std::string getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value);
    void setPropertyInt(const char* name, const int32_t i_value);
};


#endif // SPECIALMODEBASEPROCESS_H
