/**
* \file    SpecialModeBaseProcess.h
* \brief     Declaration of SpecialModeBaseProcess
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

#if !defined(EA_7607BBE0_F502_4e13_B13F_454F4E3CAF3D__INCLUDED_)
#define EA_7607BBE0_F502_4e13_B13F_454F4E3CAF3D__INCLUDED_

#include "Log.h"
#include "SpecialModeServicesManager.h"
#include "ISpecialModeAppInterface.h"
#include "SpecialModeData_type.h"


class SpecialModeBaseProcess
{
private:


public:
    SpecialModeBaseProcess();
    void initialize(sp<SpecialModeServicesManager> servicesMgr, ISpecialModeAppInterface* pApp, sp<Handler> handler);
    virtual ~SpecialModeBaseProcess();
    virtual void do_SpecialModeHandler(int32_t what, const sp<sl::Message>& message) = 0;
    virtual void handleEvent(uint32_t ev) = 0;
    virtual void handleTimerEvent(int timerId) = 0;

protected:
    sp<Handler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    ISpecialModeAppInterface* mp_Application;
    sp<TelephonyManager::TsimLock> m_TsimLock;
    sp<TelephonyManager::PsimLock> m_psimLock;
    TimerTimeoutHandler *mp_DemoModeTimer;
    bool m_request_acquire_tSim;
    bool acquireTSimLock();
    void activateTSIM();
    virtual void initializeProcess() = 0;
    bool releaseTSimLock();

    bool acquirePSimLock();
    bool releasePSimLock();
    char* getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value);
    void setPropertyInt(const char* name, const int32_t i_value);
};

#endif // !defined(EA_7607BBE0_F502_4e13_B13F_454F4E3CAF3D__INCLUDED_)
