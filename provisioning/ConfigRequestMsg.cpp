/**
* \file    ConfigRequestMsg.cpp
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

#include "ConfigRequestMsg.h"

ConfigRequestMsg::ConfigRequestMsg(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager)
: ServiceDataBase(ngtpManager, configManager)
{
}

void ConfigRequestMsg::makeMessage(tuSts_parameters *tuStsParam)
{
    omitAppAck();
    setAppAckRequired(true);
    setMessageType(svc::dataRequest);
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

    // generic parameters
    svc::NGTPUplinkCore::genericParameters genericParameters;
    svc::GenericParameter component;
    svc::GenericParameterKey key;
    svc::GenericParameterValue value;

    uint8_t *dealerString;
    // DealerAndActivationCode1
    key.set_id(DEALER_ACTIVATION_KEY1);
    dealerString = getStringFromConfig(DEALER_ACTIVATION_NAME1);
    if(dealerString != NULL)
        value.set_stringVal((const char*)dealerString);
    else
        value.set_stringVal("0");
    component.set_key(key);
    component.set_value(value);
    genericParameters.prepend(component);

    // DealerAndActivationCode2
    key.set_id(DEALER_ACTIVATION_KEY2);
    dealerString = getStringFromConfig(DEALER_ACTIVATION_NAME2);
    if(dealerString != NULL)
        value.set_stringVal((const char*)dealerString);
    else
        value.set_stringVal("0");
    component.set_key(key);
    component.set_value(value);
    genericParameters.prepend(component);

    setGenericParameters(genericParameters);

    setUplink();
    setDatacore();
}
