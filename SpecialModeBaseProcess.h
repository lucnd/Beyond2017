#ifndef SPECIALMODEBASEPROCESS_H
#define SPECIALMODEBASEPROCESS_H

#include "SpecialModeServicesManager.h"
#include "ISpecialModeApplication.h"
#include "SpecialModeDataType.h"
#include "Log.h"

class SpecialModeBaseProcess
{
public:
    SpecialModeBaseProcess() {}
    virtual ~SpecialModeBaseProcess();
    virtual void do_SpecialModeHandler(int32_t what, sp<sl::Message>& messages) = 0;
    virtual void handleEvent(uint32_t eventId) = 0;
    virtual void handleTimerEvent(int timerId) = 0;

    void initialize(sp<SpecialModeServicesManager> serviceMgr, ISpecialModeApplication* pApp, sp<sl::Handler>& handler);

protected:
    sp<sl::Handler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    ISpecialModeApplication* mp_Application;
//    sp<TelephonyManager::TsimLock> m_TsimLock;
//    sp<TelephonyManager::PsimLock> m_psimLock;
    TimerTimeoutHandler *mp_DemoModeTimer;
    bool m_request_acquire_tSim;
    bool acquireTSimLock();
    void activateTSIM();
    bool releaseTSimLock();
    virtual void initializeProcess() = 0;

//    bool acquirePSimLock();
//    bool releasePSimLock();
    char* getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value);
    void setPropertyInt(const char* name, const int32_t i_value);
};

#endif // SPECIALMODEBASEPROCESS_H
