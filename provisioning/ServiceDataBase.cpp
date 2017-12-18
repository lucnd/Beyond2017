/**
* \file    ServiceDataBase.cpp
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

#include "ServiceDataBase.h"
#define LOG_TAG "ProvisioningApp"
#include "Log.h"

ServiceDataBase::ServiceDataBase(sp<INGTPManagerService> ngtpManager, sp<IConfigurationManagerService> configManager)
{
    m_ngtpManager = ngtpManager;
    m_configManager = configManager;
    mRetryTimeID = 0;
}

void ServiceDataBase::omitAppAck()
{
    serviceData.omit_appAck();
}

void ServiceDataBase::setAppAck(svc::Acknowledge ack)
{
    serviceData.set_appAck(ack);
}

void ServiceDataBase::setMessageType(svc::ServiceMessageType msgType)
{
    serviceData.set_messageType(msgType);
}

void ServiceDataBase::setAppAckRequired(bool value)
{
    serviceData.set_appAckRequired(value);
}

void ServiceDataBase::setTestFlag(bool value)
{
    serviceData.set_testFlag(value);
}

void ServiceDataBase::setTuStsPower(svc::TUPowerSource power)
{
    tuSts.set_power(power);
}

void ServiceDataBase::setTuSts()
{
    uplink.set_tuStatus(tuSts);
}

void ServiceDataBase::setGenericParameters(svc::NGTPUplinkCore::genericParameters genericParameter)
{
    uplink.set_genericParameters(genericParameter);
}

void ServiceDataBase::setUplink()
{
    data_core.set_uplink(uplink);
}

void ServiceDataBase::setDatacore()
{
    serviceData.set_serviceDataCoreMessage(data_core);
}

void ServiceDataBase::setErrorCodes()
{
    svc::NGTPUplinkCore::errorCodes errCode;
    uplink.set_errorCodes(errCode);
}

void ServiceDataBase::setTuStsDiag(diag_parameters diagParameter)
{
    diag.set_speaker(diagParameter.speaker);
    diag.set_mic(diagParameter.mic);
    diag.set_extHandsfree(diagParameter.extHandsfree);
    diag.set_handset(diagParameter.handset);
    diag.set_gsmModem(diagParameter.gsmModem);
    diag.set_gsmExtAntenna(diagParameter.gsmExtAntenna);
    diag.set_extPower(diagParameter.extPower);
    diag.set_intPower(diagParameter.intPower);
    diag.set_gnss(diagParameter.gnss);
    diag.set_gnssAntenna(diagParameter.gnssAntenna);
    diag.set_can(diagParameter.can);
    diag.set_buttons(diagParameter.buttons);
    diag.set_crashInput(diagParameter.crashInput);
    diag.set_intRtc(diagParameter.intRtc);
    diag.set_daysSinceGnssFix(diagParameter.daysSinceGnssFix);

    tuSts.set_diagnostics(diag);
}

void ServiceDataBase::setTuStsBatteryPrimaryVoltx10(uint32_t value)
{
    batt.set_primaryVoltx10(value);
}

void ServiceDataBase::setTuStsBatteryPrimaryChargePerc(uint32_t value)
{
    batt.set_primaryChargePerc(value);
}

void ServiceDataBase::setTuStsBatterySecondaryVoltx10(uint32_t value)
{
    batt.set_secondaryVoltx10(value);
}

void ServiceDataBase::setTuStsMobilePhoneConnected(svc::BOOLEXT value)
{
    tuSts.set_mobilePhoneConnected(value);
}

void ServiceDataBase::setTuStsBattery()
{
    tuSts.set_battery(batt);
}

void ServiceDataBase::setTuStsSleepCycleTime(uint32_t sec)
{
    svc::TimeStamp sleepCycleTime;
    sleepCycleTime.set_seconds(sec); // current time?? ,
    tuSts.set_sleepCyclesStartTime(sleepCycleTime);
}

void ServiceDataBase::setTuStsConfigVersion(uint32_t version)
{
    tuSts.set_configVersion(version);
}

void ServiceDataBase::setTuStsActivation(svc::TuActivationStatus activation)
{
    tuSts.set_activation(activation);
}

void ServiceDataBase::setTuStsUsesExternalGnss(svc::BOOLEXT value)
{
    tuSts.set_usesExternalGnss(value);
}

void ServiceDataBase::setHwVersion(OssString hwVersion)
{
    if(hwVersion.length() == 0)
        tuSts.omit_hwVersion();
    else
        tuSts.set_hwVersion(hwVersion);
}

void ServiceDataBase::setSwVersionMain(OssString swVersionMain)
{
    if(swVersionMain.length() == 0)
        tuSts.omit_swVersionMain();
    else
        tuSts.set_swVersionMain(swVersionMain);
}

void ServiceDataBase::setSwVersionSecondary(OssString swVersionSecondary)
{
    if(swVersionSecondary.length() == 0)
        tuSts.omit_swVersionSecondary();
    else
        tuSts.set_swVersionSecondary(swVersionSecondary);
}

void ServiceDataBase::setSwVersionConfiguration(OssString swVersionConfiguration)
{
    if(swVersionConfiguration.length() == 0)
        tuSts.omit_swVersionConfiguration();
    else
        tuSts.set_swVersionConfiguration(swVersionConfiguration);
}

void ServiceDataBase::setSerialNumber(OssString serialNumber)
{
    if(serialNumber.length() == 0)
        tuSts.omit_serialNumber();
    else
        tuSts.set_serialNumber(serialNumber);
}

void ServiceDataBase::setIMEI(OssString imei)
{
    if(imei.length() == 0)
        tuSts.omit_imei();
    else
        tuSts.set_imei(imei);
}

void ServiceDataBase::setIsowmi(OssString isowmi)
{
    vin.set_isowmi(isowmi);
}

void ServiceDataBase::setIsovds(OssString isovds)
{
    vin.set_isovds(isovds);
}

void ServiceDataBase::setIsovisModelyear(OssString isovisModelyear)
{
    vin.set_isovisModelyear(isovisModelyear);
}

void ServiceDataBase::setIsovisSeqPlant(OssString isovisSeqPlant)
{
    vin.set_isovisSeqPlant(isovisSeqPlant);
}

void ServiceDataBase::setVin()
{
    uplink.set_vin(vin);
}

void ServiceDataBase::setDefaultVin()
{
    vin.omit_isowmi();
    vin.omit_isovds();
    vin.omit_isovisModelyear();
}

uint32_t ServiceDataBase::encode_sendToTSDP(bool isAck, InternalNGTPData *ngtp)
{
    svc::NGTPServiceData_PDU svcDataPDU;
#ifdef  TSDP_SVC_3_3
    svc::tsdp_svc_3_Control ctl;
#else
    svc::tsdp_svc_Control ctl;
#endif
    PEREncodedBuffer encodedSvcData;

    ctl.setEncodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setEncodingRules(OSS_PER_UNALIGNED);
    svcDataPDU.set_data (serviceData);
    if ( svcDataPDU.encode(ctl, encodedSvcData) !=0 ) {
        LOGE("NGTPServiceData_PDU encode fails  :%s", ctl.getErrorMsg());
        return 0;
    }

    //print xml
    encodedSvcData.print_xml (ctl, 1);

    //InternalNGTPData
    InternalNGTPData internalNGTPData;

    // serviceType
    internalNGTPData.serviceType = ngtp->serviceType;
    internalNGTPData.messageType = dspt::upServiceData;
    internalNGTPData.encodedSize  = encodedSvcData.get_data_size();
    internalNGTPData.dsptAckRequired = ngtp->dsptAckRequired;
    internalNGTPData.statelessNGTPmessage = ngtp->statelessNGTPmessage;

    internalNGTPData.creationTime_seconds = ngtp->creationTime_seconds;
    internalNGTPData.creationTime_millis = ngtp->creationTime_millis;
    internalNGTPData.serialVID = ngtp->serialVID;
    internalNGTPData.transport = 0; // UDP

    LOGV("encode_sendToTSDP: seconds = %d, millis = %d", internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);
    if(isAck)
    {
        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 2, "NGTPUDPMaxRetryForAck");
        if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = NGTP_ACK_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(CONFIG_XML_PATH, 2, "NGTPUDPDelayForAckMs");
        if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = NGTP_ACK_RETRY_DELAY;

        internalNGTPData.timeoutCheck = 0;
    }
    else
    {
        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 2, "NGTPUDPMaxRetry");
        if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = NGTP_REQ_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(CONFIG_XML_PATH, 2, "NGTPUDPDelay");
        if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = NGTP_REQ_RETRY_DELAY;
        internalNGTPData.timeoutCheck = 0;
    }

    memcpy(internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
    uint32_t timeID = m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
    return timeID;
}

void ServiceDataBase::omitUplink()
{
    uplink.omit_positionExtension();
    uplink.omit_vehicleStatus();
    uplink.omit_extendedVehicleStatus();
    uplink.omit_dashboardStatus();
    uplink.omit_electricVehicleStatus();
    uplink.omit_climateStatus();
    uplink.omit_tuStatus();
    uplink.omit_pearlChain();
    uplink.omit_lastGPSTimeStamp();
    uplink.omit_vin();
    uplink.omit_navigationInformation();
    uplink.omit_memoryStatus();
    uplink.omit_vehiclePropulsionType();
    uplink.omit_crashData();
    uplink.omit_remoteOperationResponse();
    uplink.omit_genericText();
    uplink.omit_genericParameters();
    uplink.omit_trigger();
    uplink.omit_alerts();
    uplink.omit_trackpoints();
    uplink.omit_journeys();
    uplink.omit_msdInfo();
    uplink.omit_debugInfo();
    uplink.omit_errorCodes();
}

void ServiceDataBase::omitTuSts()
{
    tuSts.omit_power();
    tuSts.omit_diagnostics();
    tuSts.omit_battery();
    tuSts.omit_sleepCyclesStartTime();
    tuSts.omit_hwVersion();
    tuSts.omit_swVersionMain();
    tuSts.omit_swVersionSecondary();
    tuSts.omit_swVersionConfiguration();
    tuSts.omit_serialNumber();
    tuSts.omit_imei();
}

void ServiceDataBase::omitDiag()
{
    diag.omit_troubleCodes();
}

void ServiceDataBase::omitBatt()
{
    batt.omit_primaryVoltx10();
    batt.omit_primaryChargePerc();
    batt.omit_secondaryVoltx10();
    batt.omit_secondaryChargePerc();
}

uint8_t *ServiceDataBase::getStringFromConfig(const char* name)
{
    sp<ConfigurationBuffer> buf;
    buf = m_configManager->get(CONFIG_XML_PATH, 2, name);
    if(buf != NULL && buf->errorFlag() == E_OK)
        return buf->data();
    else
        return NULL;
}

void ServiceDataBase::setRetryTimeID(uint32_t time)
{
    mRetryTimeID = time;
}
