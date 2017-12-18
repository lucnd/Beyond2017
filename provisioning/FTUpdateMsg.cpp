/**
* \file    FTUpdateMsg.cpp
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

#include "FTUpdateMsg.h"

FTUpdateMsg::FTUpdateMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager)
: ServiceDataBase(ngtpManager, configManager)
{
}

void FTUpdateMsg::makeMessage(tuSts_parameters *tuStsParam, vin_parameters vin)
{
    omitAppAck();
    setAppAckRequired(true);
    setMessageType(svc::update);
    setTestFlag(false);

    omitUplink();
    omitTuSts();

    setTuStsPower(tuStsParam->power);

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

    setHwVersion(tuStsParam->version.hwVersion);
    setSwVersionMain(tuStsParam->version.swVersionMain);
    setSwVersionSecondary(tuStsParam->version.swVersionSecondary);
    setSwVersionConfiguration(tuStsParam->version.swVersionConfiguration);
    setSerialNumber(tuStsParam->version.serialNumber);
    setIMEI(tuStsParam->version.imei);

    setTuSts();

    setIsowmi(vin.isowmi);
    setIsovds(vin.isovds);
    setIsovisModelyear(vin.isovisModelyear);
    setIsovisSeqPlant(vin.isovisSeqPlant);

    OssString defaultSeq(STR_DEFAULT_VIN);
    if(vin.isovisSeqPlant == defaultSeq)
        setDefaultVin();

    setVin();

    setUplink();
    setDatacore();
}
