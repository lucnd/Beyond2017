/**
* \file    ConfigRequestMsg.h
* \brief     Implementation of ProvisioningApp
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

#ifndef PROVISIONING_CONFIGREQUEST_H_
#define PROVISIONING_CONFIGREQUEST_H_

#include "ServiceDataBase.h"

class ConfigRequestMsg : public ServiceDataBase
{
public:
    ConfigRequestMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager);
    ~ConfigRequestMsg(){}
    void makeMessage(tuSts_parameters *tuSts);
private:
};
#endif // PROVISIONING_CONFIGREQUEST_H_
