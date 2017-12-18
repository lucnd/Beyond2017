/**
* \file    FTUpdateMsg.h
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

#ifndef _PROVISIONING_FTUPDATE_H_
#define _PROVISIONING_FTUPDATE_H_

#include "ServiceDataBase.h"

class FTUpdateMsg : public ServiceDataBase
{
public:
    FTUpdateMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager);
    ~FTUpdateMsg(){}
    void makeMessage(tuSts_parameters *tuSts, vin_parameters vin);
private:
};
#endif // _PROVISIONING_FTUPDATE_H_
