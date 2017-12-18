/**
* \file    NGTPDecodeMsg.h
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

#ifndef _PROVISIONING_NGTPDECODE_H_
#define _PROVISIONING_NGTPDECODE_H_

#include "services/NGTPManagerService/NGTPManagerService.h"
#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"
#include "services/NGTPManagerService/InternalNGTPData.h"
#include "ConfigurationManagerService/ConfigurationManagerService.h"
#include "ConfigurationManagerService/IConfigurationManagerService.h"
#include "DiagManagerService/IDiagManagerService.h"
#include "Provisioning_Types.h"

class NGTPDecodeMsg
{
public:
    NGTPDecodeMsg();
    ~NGTPDecodeMsg(){}
    bool decodeMessage(uint8_t *buffer, uint32_t size);
    void decodeConfiguration(svc::NGTPDownlinkCore *pDownlink);
    void decodeCountries(svc::TuConfiguration *configParameter);
    void decodeAddresses(svc::CountryConfigurationBlock *countryConfig, sp<CountryDBBuffer> new_item);
    void decodeGenericParameters(svc::NGTPDownlinkCore *downlink);
    void decodeRemoteOperations(svc::NGTPDownlinkCore *downlink);
    void initializeConfigAckGenericParameter();
    void initializeConfigMgrGenericParameter();
    svc::NGTPUplinkCore::genericParameters getGenericParameter() { return mGenericParameterForConfigAck; };
    svc::NGTPUplinkCore::genericParameters getGenericParameterForConfigMgr() { return mGenericParameterForConfigMgr; };
    bool getIsConfiguration() { return bIsConfiguration; };
    bool getIsGenericParams() { return bIsGenericParams; };
    bool getIsPoiDataSet() { return bIsPoiDataSet; };
    bool getIsRemoteOperations() { return bIsRemoteOperations; };
    bool getIsGenericText() { return bIsGenericText; };
    bool getIsErrorCodes() { return bIsErrorCodes; };
    svc::ServiceMessageType getMessageType() { return mMessageType; };
    svc::Acknowledge getAckowledge() { return mAppAck; };
    uint32_t getEquipmentID() { return mEquipmentID; };
    svc::RemoteOperationName getRemoteOperationName() { return mRemoteOperation; };
private:
    svc::NGTPUplinkCore::genericParameters mGenericParameterForConfigAck;
    svc::NGTPUplinkCore::genericParameters mGenericParameterForConfigMgr;
    bool bIsConfiguration;
    bool bIsGenericParams;
    bool bIsPoiDataSet;
    bool bIsRemoteOperations;
    bool bIsGenericText;
    bool bIsErrorCodes;
    svc::ServiceMessageType mMessageType;
    svc::Acknowledge mAppAck;
    uint32_t mEquipmentID;
    svc::RemoteOperationName mRemoteOperation;
    sp<IConfigurationManagerService> m_configManager;
    sp<IDiagManagerService> m_diagManager;
};
#endif // _PROVISIONING_NGTPDECODE_H_
