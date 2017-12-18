/**
* \file    ConfigDataAckMsg.cpp
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

#include "ConfigDataAckMsg.h"

ConfigDataAckMsg::ConfigDataAckMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager)
: ServiceDataBase(ngtpManager, configManager)
{
}

void ConfigDataAckMsg::makeMessage(svc::NGTPUplinkCore::genericParameters genericParameter, svc::Acknowledge ack)
{
    setAppAck(ack);
    setAppAckRequired(false);
    setMessageType(svc::acknowledge);
    setTestFlag(false);

    omitUplink();

    // generic parameter
    setGenericParameters(genericParameter);

    // error code
    setErrorCodes();

    setUplink();
    setDatacore();
}
