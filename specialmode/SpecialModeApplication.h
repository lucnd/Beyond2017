/**
* \file    SpecialModeApplication.h
* \brief     Declaration of SpecialModeApplication
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

#if !defined(EA_EC6F4E74_43E1_4d17_A016_59EA016542A2__INCLUDED_)
#define EA_EC6F4E74_43E1_4d17_A016_59EA016542A2__INCLUDED_

#include "Typedef.h"

#include "Handler.h"
#include "Message.h"
#include "SLLooper.h"

#include "SpecialModeServicesManager.h"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"

#include "ISpecialModeAppInterface.h"
#include "SpecialModeBaseProcess.h"

class SpecialModeApplication : public ISpecialModeAppInterface, public Application
{

public:
    SpecialModeApplication();
    virtual ~SpecialModeApplication();

    virtual void do_SpecialModeHandler(uint32_t what, const sp<sl::Message>& message);
    virtual void onCreate();
    virtual void onDestroy();

    virtual void onPostReceived(const sp<Post>& post);
    void release_SpecialModeProcess();
    virtual uint32_t get_SpecialModeType();
    virtual void set_SpecialModeType(uint32_t SpecialModeType);
    char* getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value, bool sync_now);
    void setPropertyInt(const char* name, const int32_t i_value, bool sync_now);


private:
    sp<SpecialModeHandler> m_Handler;
    sp<SLLooper> m_Looper;
    SpecialModeReceiverManager* mp_ReceiverMgr;
    SpecialModeBaseProcess* mp_SpecialModeProcess;
    bool m_AppAlive;
    bool m_ProvisioningFlag;
    /**
     * SpecialModeCall type attribute
     */
    static uint32_t m_SpecialModeType;
    sp<SpecialModeServicesManager> m_ServicesMgr;

    void handleEvent(uint32_t ev);
    void Initialize_SpecialModeProcess();
    void handleEventProvisioning();
    void handleEventUnprovisioning();
    bool provision_SpecialMode();
    void setProvisioningFlag(bool flag);
    bool unprovision_SpecialMode();
};
#endif // !defined(EA_EC6F4E74_43E1_4d17_A016_59EA016542A2__INCLUDED_)

