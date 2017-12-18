/**
* \file    NGTPDecodeMsg.cpp
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

#include "NGTPDecodeMsg.h"
#define LOG_TAG "ProvisioningApp"
#include <binder/IServiceManager.h>
#include "Log.h"

#include "services/DiagManagerService/DiagManagerService.h"

NGTPDecodeMsg::NGTPDecodeMsg()
{
    bIsConfiguration = false;
    bIsGenericParams = false;
    bIsPoiDataSet = false;
    bIsRemoteOperations = false;
    bIsGenericText = false;
    bIsErrorCodes = false;
    mMessageType = svc::dataRequest;
    mAppAck = svc::ack;
    mEquipmentID = 0;
    mRemoteOperation = svc::rotNameEnd;
}

bool NGTPDecodeMsg::decodeMessage(uint8_t *buffer, uint32_t size)
{
#ifdef  TSDP_SVC_3_3
    svc::tsdp_svc_3_Control ctl;
#else
    svc::tsdp_svc_Control ctl;
#endif
    ctl.setDecodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);

    if(size > 0)
    {
        //decode for validation
        PEREncodedBuffer encodedSvcData;
        encodedSvcData.set_buffer(size, (char*)buffer);
        svc::NGTPServiceData_PDU  outPDU;
        outPDU.decode ( ctl, encodedSvcData);

        svc::NGTPServiceData *pServiceData;
        svc::NGTPDownlinkCore *pDownlink;
        pServiceData = outPDU.get_data();

        if(pServiceData == NULL)
        {
            LOGE("ServiceData is NULL, Decoding FAILED");
            return false;
        }
        else
        {
            mMessageType = pServiceData->get_messageType();
            if(pServiceData->appAck_is_present())
            {
                mAppAck = *(pServiceData->get_appAck());
                LOGV("appAck = %d", mAppAck);
            }
            LOGV("messagetype = %d", mMessageType);

            if(pServiceData->serviceDataCoreMessage_is_present())
            {
                pDownlink = pServiceData->get_serviceDataCoreMessage()->get_downlink();
                bIsConfiguration = pDownlink->configuration_is_present();
                bIsGenericParams = pDownlink->genericParams_is_present();
                bIsPoiDataSet = pDownlink->poiDataSet_is_present();
                bIsRemoteOperations = pDownlink->remoteOperations_is_present();
                bIsGenericText = pDownlink->genericText_is_present();
                bIsErrorCodes = pDownlink->errorCodes_is_present();

                if(bIsConfiguration)
                {
                    decodeConfiguration(pDownlink);
                }
                if(bIsGenericParams)
                {
                    decodeGenericParameters(pDownlink);
                }
                if(bIsRemoteOperations)
                {
                    decodeRemoteOperations(pDownlink);
                }
            }
            return true;
        }
    }
    else {
        LOGE("size is 0: decoding fail");
        return false;
    }
}

void NGTPDecodeMsg::initializeConfigAckGenericParameter()
{
    // removing generic parameter list
    while(!mGenericParameterForConfigAck.empty())
    {
       mGenericParameterForConfigAck.remove_front();
    }
}

void NGTPDecodeMsg::initializeConfigMgrGenericParameter()
{
    // removing generic parameter list
    while(!mGenericParameterForConfigMgr.empty())
    {
       mGenericParameterForConfigMgr.remove_front();
    }
}

void NGTPDecodeMsg::decodeRemoteOperations(svc::NGTPDownlinkCore *downlink)
{
    svc::RemoteOperationRequestList *pRemoteOperations;
    pRemoteOperations = downlink->get_remoteOperations();

    LOGV("  ==== Decoding RemoteOperations ====");
    mRemoteOperation = *(pRemoteOperations->get_name()); // Remote Operation Name
    LOGV("      Name = %d", mRemoteOperation);

    int32_t *pOccurs; // occurs
    pOccurs = pRemoteOperations->get_occurs();
    LOGV("      Occurs = %d", *pOccurs);

    svc::MajMinVersion16 operationScriptVersion; // Operation Script Version
    operationScriptVersion = pRemoteOperations->get_operationScriptVersion();
    uint32_t majorVersion = operationScriptVersion.get_majorVersion(); // major Version
    uint32_t minorVersion = operationScriptVersion.get_minorVersion(); // minor Version
    LOGV("      majorVersion = %d, minorVersion = %d", majorVersion, minorVersion);

    svc::RemoteOperationTrigger executionTrigger; // execution Trigger
    executionTrigger = pRemoteOperations->get_executionTrigger();
    LOGV("      executionTrigger selection = %d", executionTrigger.get_selection());
    uint8_t *pNow;  // Now
    pNow = (uint8_t*)executionTrigger.get_now();
    LOGV("      now = %d", *pNow);

    svc::RemoteOperationRequestList::remoteOperations remoteOperationList;    // remote Operations
    remoteOperationList = pRemoteOperations->get_remoteOperations();

    svc::RemoteOperationRequest *component;
    OssIndex node = remoteOperationList.first();
    while(node!=NULL)
    {
        component = remoteOperationList.at(node);
        svc::SimpleRemoteOperation operationDesc; // Operation Desc
        operationDesc = component->get_operationDesc();

        OssString componentString;  // component
        componentString = operationDesc.get_component();
        LOGV("      component = %s", componentString.get_buffer());

        svc::RemoteOperationType operationType; // operation type
        operationType = operationDesc.get_operationType();
        LOGV("      operationType = %d", operationType);

        OssString *pState; // state
        pState = operationDesc.get_state();
        LOGV("      state = %s", pState->get_buffer());

        ossBoolean stopIfFail; // stopIfFail
        stopIfFail = component->get_stopIfFail();
        LOGV("      stopIfFail = %d", stopIfFail);

        ossBoolean waitForPreviousCommand;
        waitForPreviousCommand = component->get_waitForPreviousCommand();
        LOGV("      waitForPreviousCommand = %d", waitForPreviousCommand);

        OSS_UINT32 operationTimeoutSec;
        operationTimeoutSec = component->get_operationTimeoutSec();
        LOGV("      operationTimeoutSec = %d", operationTimeoutSec);

        OSS_UINT32 periodSecx10;
        periodSecx10 = component->get_periodSecx10();
        LOGV("      periodSecx10 = %d", periodSecx10);

        OSS_INT32 occurs;
        occurs = component->get_occurs();
        LOGV("      occurs = %d", occurs);

        OSS_UINT32 initialWaitTimeSecx10;
        initialWaitTimeSecx10 = component->get_initialWaitTimeSecx10();
        LOGV("      initialWaitTimeSecx10 = %d", initialWaitTimeSecx10);

        svc::VehicleStateType *pRequiredVehicleStates;
        pRequiredVehicleStates = component->get_requiredVehicleStates();
        LOGV("      pRequiredVehicleStates = %d", *pRequiredVehicleStates);

        node = remoteOperationList.next(node);
    }

    LOGV("==== All Generic parameters are received ==== ");
    // printing all generic parameters
    node = mGenericParameterForConfigMgr.first();
    svc::GenericParameter *genComponent;
    while(node!=NULL)
    {
        genComponent = mGenericParameterForConfigMgr.at(node);
        uint32_t *key = genComponent->get_key().get_id();
        uint32_t *value;
        ossBoolean *boolValue;
        uint32_t selection = genComponent->get_value().get_selection();
        switch(selection)
        {
            case svc::GenericParameterValue::uint32Val_chosen :
                value = genComponent->get_value().get_uint32Val();
                LOGV("      key = 0x%x, value = %d", *key, *value);
                break;
            case svc::GenericParameterValue::boolVal_chosen :
                boolValue = genComponent->get_value().get_boolVal();
                LOGV("      key = 0x%x, value = %d", *key, *boolValue);
                break;
            default:
                LOGV("      unknown chosen...");
                break;
        }
        node = mGenericParameterForConfigMgr.next(node);
    }

}

void NGTPDecodeMsg::decodeGenericParameters(svc::NGTPDownlinkCore *downlink)
{
    svc::NGTPDownlinkCore::genericParams *genericParameter;
    svc::GenericParameter *component;
    genericParameter = downlink->get_genericParams();
    OssIndex node = genericParameter->first();
    svc::GenericParameter ackcomponent;
    svc::GenericParameterKey ackkey;
    svc::GenericParameterValue ackvalue;
    LOGV("  ==== Decoding GenericParameters ====");

    while(node != NULL) {
        component = genericParameter->at(node);
        uint32_t *key = component->get_key().get_id();
        uint32_t selection = component->get_value().get_selection();
        uint32_t *value;
        ossBoolean *boolValue;
        OssString *stringValue;
        ackkey.set_id(*key);
        bool bSave = true;
#ifndef APPMODE_JL_NOTSUPPORT
        if(*key == APPMODEJL_KEY && selection == svc::GenericParameterValue::uint8Val_chosen
            && *(component->get_value().get_uint8Val()) != 0 && *(component->get_value().get_uint8Val()) != 1 ){
            ackvalue.set_setFailedValueOutOfRange(NULL); // not supported value
            bSave = false;
        }
        else
            ackvalue.set_setOk(NULL);
#else
        ackvalue.set_setOk(NULL);
#endif
        ackcomponent.set_key(ackkey);
        ackcomponent.set_value(ackvalue);
        mGenericParameterForConfigAck.prepend(ackcomponent);

        switch(selection) {
            case svc::GenericParameterValue::uint8Val_chosen :
                value = component->get_value().get_uint8Val();
                ackvalue.set_uint32Val(*value);
                LOGV("      key = %u, value = %d", *key, *value);
                break;
            case svc::GenericParameterValue::uint16Val_chosen :
                value = component->get_value().get_uint16Val();
                ackvalue.set_uint32Val(*value);
                LOGV("      key = %u, value = %d", *key, *value);
                break;
            case svc::GenericParameterValue::uint32Val_chosen :
                value = component->get_value().get_uint32Val();
                ackvalue.set_uint32Val(*value);
                LOGV("      key = %u, value = %u", *key, *value);
                break;
            case svc::GenericParameterValue::boolVal_chosen :
                boolValue = component->get_value().get_boolVal();
                ackvalue.set_boolVal(*boolValue);
                LOGV("      key = %u, value = %d", *key, *boolValue);
                break;
            case svc::GenericParameterValue::stringVal_chosen :
                stringValue = component->get_value().get_stringVal();
                ackvalue.set_stringVal(*stringValue);
                LOGV("      key = %u, value = %s", *key, stringValue->get_buffer());
                break;
            default:
                LOGV("      unknown (%d) chosen...", selection);
                break;
        }
        if(*key == EQUIPMENTID_KEY) {
            if(selection == svc::GenericParameterValue::uint32Val_chosen) {
                mEquipmentID = *value;
                LOGV("Factory Test Done,,, equipmentID = %u", mEquipmentID);
            }
            else {
                mEquipmentID = 0;
                LOGV("Factory Test Done,,, but wrong value is received, set equipmentID to 0");
            }
        }
        ackcomponent.set_value(ackvalue);
        OssIndex configNode = mGenericParameterForConfigMgr.first();
        svc::GenericParameter *genComponent;
        while(configNode!=NULL) {
            genComponent = mGenericParameterForConfigMgr.at(configNode);
            uint32_t *oldKey = genComponent->get_key().get_id();
            if(*oldKey == *key) {
                LOGV("key [%d] is already saved...", *oldKey);
                bSave = false;
                break;
            }
            configNode = mGenericParameterForConfigMgr.next(configNode);
        }
        if(bSave)
            mGenericParameterForConfigMgr.prepend(ackcomponent);
        node = genericParameter->next(node);
    }
}

void NGTPDecodeMsg::decodeConfiguration(svc::NGTPDownlinkCore *pDownlink)
{
    svc::TuConfiguration *configParameter;
    configParameter = pDownlink->get_configuration();
    LOGV("==== Decoding Configuration ====");
    uint32_t configVersion = configParameter->get_configVersion();
    LOGV("configVersion = %d", configVersion);
    m_configManager = interface_cast<IConfigurationManagerService>(defaultServiceManager()->getService(String16(ConfigurationManagerService::getServiceName())));
    m_diagManager = interface_cast<IDiagManagerService>(defaultServiceManager()->getService(String16(DiagManagerService::getServiceName())));
    uint32_t oldValue;
    sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 2, "configVersion");
    if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
    {
        buf->data(&oldValue);
        if(oldValue == configVersion) // same configVersion
            LOGV("same configVersion");
        else if(oldValue < configVersion)// update this value
        {
            LOGV("update configVersion");
            sp<ConfigurationBuffer> pBuffer= new ConfigurationBuffer;
            pBuffer->setTo((uint32_t)configVersion);
            error_t err = m_configManager->change_set(CONFIG_XML_PATH, 2, "configVersion", pBuffer);
            if(err == E_ERROR)
                LOGE("set configVersion to configMgr error ");

            LOGV("Software_Incompatibility DTC normal occurred");
            m_diagManager->SendDTCResult(Internal_Software_Incompatibility, DTC_NORMAL_OF_C300);
        }
        else {
            LOGV("Software_Incompatibility DTC failure occurred");
            m_diagManager->SendDTCResult(Internal_Software_Incompatibility, DTC_FAULT_OF_C300);
        }
    }
    decodeCountries(configParameter);
}

void NGTPDecodeMsg::decodeCountries(svc::TuConfiguration *configParameter)
{
    svc::TuConfiguration::countries countriesList;
    svc::CountryConfigurationBlock *component;
    countriesList = configParameter->get_countries();
    OssIndex node = countriesList.first();
    LOGV("  ==== Decoding Countries ====");
    sp<CountryDBBuffer> new_item = new CountryDBBuffer();
    while(node!=NULL)
    {
        component = countriesList.at(node);
        uint32_t validCountryCode = component->get_validCountryCode();
        uint8_t activated = component->get_activated();
        LOGV("  validCountryCode = %d", validCountryCode);
        LOGV("  activated = %d", activated);
        svc::ApplicationConfiguration applicationConfig = component->get_applicationConfig();
        uint8_t eCall_requestCscNumber = applicationConfig.get_eCall_requestCscNumber();
        uint8_t eCall_psapType = applicationConfig.get_eCall_psapType();
        LOGV("  eCall_requestCscNumber = %d", eCall_requestCscNumber);
        LOGV("  eCall_psapType = %d", eCall_psapType);

        new_item->setMcc(validCountryCode);
        new_item->setActive(activated);
        new_item->setType(CountryDBBuffer::GSM_GPRS);
        new_item->setECallRequestCscNumber(eCall_requestCscNumber);
        new_item->setEcallPsapType(eCall_psapType);
        new_item->setCountryItemsSize(4);
        decodeAddresses(component, new_item);
        error_t confirm = m_configManager->set_countryDB(new_item);
        if(confirm == E_ERROR)
            LOGV("set_countryDB ERROR");
        else
            LOGV("set_countryDB SUCCESS");
        node = countriesList.next(node);
    }
}

void NGTPDecodeMsg::decodeAddresses(svc::CountryConfigurationBlock *countryConfig, sp<CountryDBBuffer> new_item)
{
    svc::CountryConfigurationBlock::addresses addressesList;
    addressesList = countryConfig->get_addresses();
    svc::AddressBlock *component;
    OssIndex node = addressesList.first();
    LOGV("      ==== Decoding AddressBlock ====");
    uint8_t count = 1;
    sp<Buffer> temp_buf;
    while(node!=NULL)
    {
        temp_buf = new Buffer();
        component = addressesList.at(node);
        uint32_t addressId = component->get_addressId();
        svc::AddressBlock::addressUsage address_Usage = component->get_addressUsage();
        LOGV("      addressId = %d, addressUsage = %d", addressId, address_Usage);

        svc::AddressType address = component->get_address();
        uint32_t selection = address.get_selection();

        svc::CommunicationAddress *absolute;
        svc::Address absoluteAddress;
        switch(selection)
        {
            case svc::AddressType::absolute_chosen :
            {
                LOGV("          ==== Decoding AddressType ====");
                LOGV("          AddressType is absolute");
                absolute = address.get_absolute();
                svc::NetworkType networkID = absolute->get_networkID();
                svc::CommunicationAddress::validityRange validity_Range = absolute->get_validityRange();
                LOGV("          networkId = %d, validityRange = %d", networkID, validity_Range);
                absoluteAddress = absolute->get_address();
                if(absoluteAddress.get_selection() == svc::Address::address_bcd_chosen)
                {
                    LOGV("          absolute address type is bcd");
                    svc::Address::address_bcd *address_bcd = absoluteAddress.get_address_bcd();
                    OssIndex node = address_bcd->first();
                    uint32_t *component;
                    char bcdAddresses[21] = {0,};
                    char bcdAddr[3] = {0,};
                    while(node!=NULL)
                    {
                        component = address_bcd->at(node);
                        node = address_bcd->next(node);
                        if(*component == 10) { // process +
                            sprintf(bcdAddr, "+");
                            strcat(bcdAddresses, bcdAddr);
                        }
                        else if(*component == 11) { // process *
                            sprintf(bcdAddr, "*");
                            strcat(bcdAddresses, bcdAddr);
                        }
                        else if(*component == 12) { // process #
                            sprintf(bcdAddr, "#");
                            strcat(bcdAddresses, bcdAddr);
                        }
                        else if(*component >=0 && *component <= 9) {
                            sprintf(bcdAddr, "%d", *component);
                            strcat(bcdAddresses, bcdAddr);
                        }
                        else
                            LOGE("invalid component");
                    }
                    LOGV("          bcdAddresses = %s", bcdAddresses);
                    new_item->setCountryItemIndexMap(count, addressId);
                    temp_buf->setSize(strlen(bcdAddresses)+1);
                    temp_buf->setTo(bcdAddresses, strlen(bcdAddresses)+1);
                    new_item->setCountryItem(count, temp_buf);
                    count++;
                }
                else if(absoluteAddress.get_selection() == svc::Address::address_ascii_chosen)
                {
                    LOGV("          absolute address type is ascii");
                }
                else
                {
                }
                break;
            }
            case svc::AddressType::reference_id_chosen :
                LOGV("          AddressType is reference_id_chosen");
                break;
            default:
                break;
        }
        node = addressesList.next(node);
    }
}
