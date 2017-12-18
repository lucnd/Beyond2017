/**
* \file    SpecialModeHandler.h
* \brief     Declaration of SpecialModeHandler
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

#if !defined(EA_08CAF961_8E5D_4e9d_BBD2_5FB07B896B9A__INCLUDED_)
#define EA_08CAF961_8E5D_4e9d_BBD2_5FB07B896B9A__INCLUDED_

#include "SpecialModeData_type.h"
#include "ISpecialModeAppInterface.h"

enum SpecialModeHandlerMessageID{
    RECV_MSG_SPECIALMODE_PROVISIONING   = 0,
    RECV_MSG_SPECIALMODE_UNPROVISIONING = 1,
    RECV_MSG_FROM_DEBUG_COMMAND         = 2,
    RECV_MSG_FROM_DIAGNOSTICS_DATA      = 3,
    RECV_MSG_FROM_SLDD                  = 4,

    RECV_MSG_FROM_TELEPHONY_PSIM_STATE_CHANGED        = 10,
    RECV_MSG_FROM_TELEPHONY_ACTIVESIM_STATE_CHANGED   = 11,
    RECV_MSG_FROM_TELEPHONY_PSIM_LOCK_STATE_CHANGED   = 12,
    RECV_MSG_FROM_WIFI_ON        = 13,
    RECV_MSG_FROM_WIFI_OFF       = 14,

    RECV_MSG_FROM_CONFIG = 15,

    RECV_MSG_DEMOMODE_START             = 100,
    RECV_MSG_DEMOMODE_STOP              = 101
};

class SpecialModeHandler : public sl::Handler
{
private:
    ISpecialModeAppInterface& mr_Application;

public:
    SpecialModeHandler(sp<SLLooper>& looper, ISpecialModeAppInterface& service)
        : Handler(looper),mr_Application(service){};
    virtual ~SpecialModeHandler();

    virtual void handleMessage(const sp<sl::Message>& message);
};
#endif // !defined(EA_08CAF961_8E5D_4e9d_BBD2_5FB07B896B9A__INCLUDED_)
