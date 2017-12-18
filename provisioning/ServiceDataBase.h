/**
* \file    ServiceDataBase.h
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

#ifndef _PROVISIONING_SERVICE_DATABASE_H_
#define _PROVISIONING_SERVICE_DATABASE_H_

#include "services/NGTPManagerService/NGTPManagerService.h"
#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"
#include "services/NGTPManagerService/InternalNGTPData.h"
#include "ConfigurationManagerService/ConfigurationManagerService.h"
#include "ConfigurationManagerService/IConfigurationManagerService.h"
#include "Provisioning_Types.h"

class ServiceDataBase
{
public:
    ServiceDataBase(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager);
    ~ServiceDataBase(){}
    void omitUplink();
    void omitTuSts();
    void omitDiag();
    void omitBatt();
    void omitAppAck();
    void setAppAck(svc::Acknowledge ack);
    void setMessageType(svc::ServiceMessageType msgType);
    void setAppAckRequired(bool value);
    void setTestFlag(bool value);
    void setTuStsPower(svc::TUPowerSource power);
    void setTuStsDiag(diag_parameters diagParameters);
    void setTuStsBatteryPrimaryVoltx10(uint32_t value);
    void setTuStsBatteryPrimaryChargePerc(uint32_t value);
    void setTuStsBatterySecondaryVoltx10(uint32_t value);
    void setTuStsBattery();
    void setTuStsMobilePhoneConnected(svc::BOOLEXT value);
    void setTuStsSleepCycleTime(uint32_t sec);
    void setTuStsConfigVersion(uint32_t version);
    void setTuStsActivation(svc::TuActivationStatus activation);
    void setTuStsUsesExternalGnss(svc::BOOLEXT value);
    void setHwVersion(OssString);
    void setSwVersionMain(OssString);
    void setSwVersionSecondary(OssString);
    void setSwVersionConfiguration(OssString);
    void setSerialNumber(OssString);
    void setIMEI(OssString);
    void setTuSts();
    void setIsowmi(OssString);
    void setIsovds(OssString);
    void setIsovisModelyear(OssString);
    void setIsovisSeqPlant(OssString);
    void setVin();
    void setDefaultVin();
    void setGenericParameters(svc::NGTPUplinkCore::genericParameters genericParameter);
    void setUplink();
    void setDatacore();
    uint32_t encode_sendToTSDP(bool isAck, InternalNGTPData *internalNGTPData);
    void setErrorCodes();
    uint8_t *getStringFromConfig(const char* name);
    uint32_t getRetryTimeID() { return mRetryTimeID; };
    void setRetryTimeID(uint32_t time);

private:
    svc::NGTPServiceData serviceData;
    svc::NGTPCore data_core;
    svc::NGTPUplinkCore uplink;
    svc::TelematicUnitStatus tuSts;
    svc::TUDiagnosticData diag;
    svc::TUBackupBatteryStatus batt;
    svc::TimeStamp sleepCycleTime;
    svc::NGTPUplinkCore::genericParameters genericParameters;
    svc::GenericParameter component;
    svc::GenericParameterKey key;
    svc::GenericParameterValue value;
    svc::VIN vin;
    sp<INGTPManagerService> m_ngtpManager;
    sp<IConfigurationManagerService> m_configManager;
    uint32_t mRetryTimeID;
};

#endif // _PROVISIONING_SERVICE_DATABASE_H_

