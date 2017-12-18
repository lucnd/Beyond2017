/**
* \file    ConfigDataAckMsg.h
* \brief     Declaration of ProvisioningApp
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       wonsik93.park
* \date    2015.09.16
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/
#ifndef _PROVISIONING_CONFIGDATA_ACK_H_
#define _PROVISIONING_CONFIGDATA_ACK_H_

#include "ServiceDataBase.h"

class ConfigDataAckMsg : public ServiceDataBase
{
public:
    ConfigDataAckMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager);
    ~ConfigDataAckMsg(){}
    void makeMessage(svc::NGTPUplinkCore::genericParameters genericParameter, svc::Acknowledge ack);
private:
};
#endif // _PROVISIONING_CONFIGDATA_ACK_H_
