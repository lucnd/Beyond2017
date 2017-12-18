/**
* \file    StatusUpdateAckMsg.cpp
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

#include "StatusUpdateAckMsg.h"

StatusUpdateAckMsg::StatusUpdateAckMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager)
: ServiceDataBase(ngtpManager, configManager)
{
}

void StatusUpdateAckMsg::makeMessage(tuSts_parameters *tuStsParam, svc::Acknowledge ack)
{
    setAppAck(ack);
    setAppAckRequired(false);
    setMessageType(svc::acknowledge);
    setTestFlag(false);

    omitUplink();
    omitTuSts();

    setTuStsPower(tuStsParam->power);

    omitDiag();

    setTuStsDiag(tuStsParam->diag);
    omitBatt();
    setTuStsBatteryPrimaryVoltx10(tuStsParam->battParam.primaryVoltx10);
    setTuStsBatteryPrimaryChargePerc(tuStsParam->battParam.primaryChargePerc);
    setTuStsBatterySecondaryVoltx10(tuStsParam->battParam.secondaryVoltx10);
    setTuStsBattery();
    setTuStsMobilePhoneConnected(tuStsParam->mobilePhoneConnected);
    setTuStsSleepCycleTime(tuStsParam->sleepCyclesStartTime);
    setTuStsConfigVersion(tuStsParam->configVersion);
    setTuStsActivation(tuStsParam->activation);
    setTuStsUsesExternalGnss(svc::isFalse);
    setTuSts();

    // error code
    setErrorCodes();

    setUplink();
    setDatacore();
}
