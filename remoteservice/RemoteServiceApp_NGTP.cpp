/**
* \file    RemoteServiceApp_NGTP.cpp
* \brief     Implementation of RemoteServiceApp
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       dongjin1.seo
* \date    2015.12.08
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#define LOG_TAG "RVC"
#include "Log.h"

#include "RemoteServiceApp.h"


#if 0
void RemoteServiceApp::sendDSPTackMsgToTSP(RS_ServiceType remoteServiceType)
{
    LOGV("sendDSPTackMsgToTSP() remoteServiceType:%d ", remoteServiceType);
    //InternalNGTPData

    InternalNGTPData internalNGTPData;

    internalNGTPData.serviceType = getNgtpServiceType(remoteServiceType);
    internalNGTPData.creationTime_seconds = getNgtpTimeSec(remoteServiceType);
    internalNGTPData.creationTime_millis = getNgtpTimeMil(remoteServiceType);
    internalNGTPData.dsptAckRequired = FALSE;
    internalNGTPData.statelessNGTPmessage = TRUE;

    internalNGTPData.messageType = dspt::dsptAck;
    internalNGTPData.tspErrorCode = 0;
    internalNGTPData.encodedSize  = 0;

    internalNGTPData.transport = 0;  //UDP

    uint8_t retryNum;
    sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_RETRY);
    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&retryNum);
        internalNGTPData.retryNum = retryNum;
    }
    else
        internalNGTPData.retryNum = RVC_NGTP_RETRY_CNT;

    uint16_t retryDelay;
    buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_DELAY);
    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&retryDelay);
        internalNGTPData.retryDelay = retryDelay;
    }
    else
        internalNGTPData.retryDelay = RVC_NGTP_RETRY_DELAY;

    internalNGTPData.timeoutCheck = 0;

    LOGI("[SEND NGTP MSG] type:%d second:%d millis:%d", remoteServiceType, internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);
    if(!getSLDDcmdActiveStatus())
         m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));

    LOGV("END sendDSPTackMsgToTSP");

}
#endif

//param1: serviceType: NGTP_ST_RDU/RDL/HBLF/ALOFF
// NGTP uplink dummy for test
void RemoteServiceApp::sendAckMsgToTSP(RS_ServiceType remoteServiceType, svc::ServiceMessageType svcMessageType)
{
    LOGV("sendAckMsgToTSP() remoteServiceType:%d ServiceMessageType:%d ",remoteServiceType, svcMessageType);
    svc::NGTPServiceData serviceData;

    serviceData.set_appAck(svc::ack);
    serviceData.set_appAckRequired(FALSE);
    serviceData.set_messageType (svcMessageType);
    serviceData.set_testFlag (FALSE);

    svc::NGTPServiceData::serviceDataCoreMessage data_core;

    svc::NGTPServiceData::serviceDataCoreMessage::uplink uplink;

    assembleMsgToTSP(&uplink);

    data_core.set_uplink(uplink);

    serviceData.set_serviceDataCoreMessage(data_core);


    //encode
    svc::NGTPServiceData_PDU svcDataPDU;
#ifdef TSDP_SVC_3_3
    svc::tsdp_svc_3_Control ctl;
#else
    svc::tsdp_svc_Control ctl;
#endif
    PEREncodedBuffer encodedSvcData;

    ctl.setEncodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setEncodingRules(OSS_PER_UNALIGNED);
    svcDataPDU.set_data (serviceData);

    if(svcDataPDU.encode(ctl, encodedSvcData) != 0) {
        LOGE("NGTPServiceData_PDU encode fails :%s", ctl.getErrorMsg());
    } else {
        //print xml
        encodedSvcData.print_xml (ctl, 1);

        //InternalNGTPData

        InternalNGTPData internalNGTPData;

        internalNGTPData.serviceType = getNgtpServiceType(remoteServiceType);
        internalNGTPData.creationTime_seconds = getNgtpTimeSec(remoteServiceType);
        internalNGTPData.creationTime_millis = getNgtpTimeMil(remoteServiceType);

        internalNGTPData.dsptAckRequired = TRUE;
        internalNGTPData.statelessNGTPmessage = FALSE;
        internalNGTPData.messageType = dspt::upServiceData;
        internalNGTPData.tspErrorCode = 0;
        internalNGTPData.encodedSize  = encodedSvcData.get_data_size();

        internalNGTPData.transport = 0;  //UDP

        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_RETRY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = RVC_NGTP_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_DELAY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = RVC_NGTP_RETRY_DELAY;

        internalNGTPData.timeoutCheck = 0;

        memcpy( internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
        LOGI("[SEND NGTP MSG] type:%d second:%d millis:%d", remoteServiceType, internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);
        if(getSLDDcmdActiveStatus()) {
            if(svcMessageType == svc::actionResponse)
            setSLDDcmdActiveStatus(false);
        } else {
            m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
        }
    }

    if((remoteServiceType == RS_UNLOCK_VEHICLE) &&
        (svcMessageType == svc::actionResponse) &&
        getRDUTsimLockState())
    {
        if(getRESTsimLockState())
        {
            setRDUTsimLockState(false);
            LOGV("Remote Door Unlock: RDU TSIM Lock state set to false. But RES keep the TSIM Lock state.");
        }
        else
        {
            if(RvcTsimLockRelease())
                setRDUTsimLockState(false);
            else
                LOGE("Remote Door Unlock: TSIM Lock release fail !");
        }
    }

    // TODO: PSIM Lock release when receiving the dspt Ack for the EndRequestMessage. (when terminated RDL NGTP message sequence.)
    if((remoteServiceType == RS_SECURE_VEHICLE) &&
        (svcMessageType == svc::actionResponse) &&
        (TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM) &&
        isTimerOn(MyTimer::TIMER_PSIM_LOCK))
    {
        if(RvcPsimLockRelease())
            stopTimerId(MyTimer::TIMER_PSIM_LOCK);
        else
            LOGE("Remote Door Lock: PSIM Lock release fail !");
    }

    if(svcMessageType == svc::actionResponse)
    {
        RvcKeepPowerRelease();
    }
}




//param1: serviceType: NGTP_ST_RDU/RDL/HBLF/ALOFF, ....
void RemoteServiceApp::sendNackMsgToTSP(RS_ServiceType remoteServiceType)
{
    LOGV("sendNackMsgToTSP()  remoteServiceType:%d ", remoteServiceType);
    svc::NGTPServiceData serviceData;

    serviceData.set_appAck(svc::nack);
    serviceData.set_appAckRequired(FALSE);
    serviceData.set_messageType (svc::actionResponse);
    serviceData.set_testFlag (FALSE);

    svc::NGTPServiceData::serviceDataCoreMessage data_core;

    svc::NGTPServiceData::serviceDataCoreMessage::uplink uplink;

    assembleMsgToTSP(&uplink);


    //error code field
    uplink.set_errorCodes(mErrCode);

    mErrorIdx = 0;
    initNGTPerrorCode();

    data_core.set_uplink(uplink);

    serviceData.set_serviceDataCoreMessage(data_core);

    //encode
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

    if(svcDataPDU.encode(ctl, encodedSvcData) != 0) {
        LOGE("NGTPServiceData_PDU encode fails :%s", ctl.getErrorMsg());
    } else {
        //print xml
        encodedSvcData.print_xml (ctl, 1);

        //InternalNGTPData

        InternalNGTPData internalNGTPData;

        internalNGTPData.serviceType = getNgtpServiceType(remoteServiceType);
        internalNGTPData.creationTime_seconds = getNgtpTimeSec(remoteServiceType);
        internalNGTPData.creationTime_millis = getNgtpTimeMil(remoteServiceType);

        internalNGTPData.dsptAckRequired = TRUE;
        internalNGTPData.statelessNGTPmessage = FALSE;
        internalNGTPData.messageType = dspt::upServiceData;
        internalNGTPData.tspErrorCode = 0;
        internalNGTPData.encodedSize  = encodedSvcData.get_data_size();

        internalNGTPData.transport = 0;  //UDP

        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_RETRY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = RVC_NGTP_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_DELAY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = RVC_NGTP_RETRY_DELAY;

        internalNGTPData.timeoutCheck = 0;

        memcpy( internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
        LOGI("[SEND NGTP MSG] type:%d second:%d millis:%d", remoteServiceType, internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);
        if(getSLDDcmdActiveStatus()) {
            setSLDDcmdActiveStatus(false);
        } else {
            m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
        }
    }

    if((remoteServiceType == RS_UNLOCK_VEHICLE) && getRDUTsimLockState())
    {
        if(getRESTsimLockState())
        {
            setRDUTsimLockState(false);
            LOGV("Remote Door Unlock: RDU TSIM Lock state set to false. But RES keep the TSIM Lock state.");
        }
        else
        {
            if(RvcTsimLockRelease())
                setRDUTsimLockState(false);
            else
                LOGE("Remote Door Unlock: TSIM Lock release fail !");
        }
    }

    // TODO: PSIM Lock release when receiving the dspt Ack for the NackMessage. (when terminated RDL NGTP message sequence.)
    if((remoteServiceType == RS_SECURE_VEHICLE) &&
        (TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM) &&
        isTimerOn(MyTimer::TIMER_PSIM_LOCK))
    {
        if(RvcPsimLockRelease())
            stopTimerId(MyTimer::TIMER_PSIM_LOCK);
        else
            LOGE("Remote Door Lock: PSIM Lock release fail !");
    }

    RvcKeepPowerRelease();
}

void RemoteServiceApp::sendAckMsgToTSP_RISM(RS_ServiceType remoteServiceType, svc::ServiceMessageType svcMessageType, bool appAck, uint8_t errorCode)
{
    LOGV("sendAckMsgToTSP_RISM() remoteServiceType:%d ServiceMessageType:%d errorCode:%d ",remoteServiceType, svcMessageType, errorCode);
    svc::NGTPServiceData serviceData;

    if(false == appAck)
    {
        serviceData.set_appAck(svc::nack);
    }
    else
    {
        serviceData.set_appAck(svc::ack);
    }
    serviceData.set_appAckRequired(FALSE);
    serviceData.set_messageType (svcMessageType);
    serviceData.set_testFlag (FALSE);

    svc::NGTPServiceData::serviceDataCoreMessage data_core;

    svc::NGTPServiceData::serviceDataCoreMessage::uplink uplink;

    assembleMsgToTSP_RISM(&uplink, svcMessageType, appAck, errorCode);

    data_core.set_uplink(uplink);

    serviceData.set_serviceDataCoreMessage(data_core);


    //encode
    svc::NGTPServiceData_PDU svcDataPDU;
    svc::tsdp_svc_Control ctl;
    PEREncodedBuffer encodedSvcData;

    ctl.setEncodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setEncodingRules(OSS_PER_UNALIGNED);
    svcDataPDU.set_data (serviceData);

    if(svcDataPDU.encode(ctl, encodedSvcData) != 0) {
        LOGE("NGTPServiceData_PDU encode fails :%s", ctl.getErrorMsg());
    } else {
        //print xml
        encodedSvcData.print_xml (ctl, 1);

        //InternalNGTPData
        InternalNGTPData internalNGTPData;

        internalNGTPData.serviceType = getNgtpServiceType(RS_RISM_MOVE);
        internalNGTPData.creationTime_seconds = getNgtpTimeSec(RS_RISM_MOVE);
        internalNGTPData.creationTime_millis = getNgtpTimeMil(RS_RISM_MOVE);

        internalNGTPData.dsptAckRequired = TRUE;
        internalNGTPData.statelessNGTPmessage = FALSE;
        internalNGTPData.messageType = dspt::upServiceData;
        internalNGTPData.tspErrorCode = 0;
        internalNGTPData.encodedSize  = encodedSvcData.get_data_size();
        internalNGTPData.transport = 0;
        internalNGTPData.timeoutCheck = 0;

        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_RETRY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = RVC_NGTP_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_DELAY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = RVC_NGTP_RETRY_DELAY;

        memcpy( internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
        LOGI("[SEND NGTP MSG] type:%d second:%d millis:%d", remoteServiceType, internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);
        if(getSLDDcmdActiveStatus()) {
            if(svcMessageType == svc::actionResponse)
            setSLDDcmdActiveStatus(false);
        } else {
            m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
        }
    }

    if(svcMessageType == svc::actionResponse)
    {
        //RISM request CAN signal clear
//        resetSeatMoveReq();
        RvcKeepPowerRelease();
    }
}

void RemoteServiceApp::sendNackMsgToTSP_RISM(RS_ServiceType remoteServiceType, uint8_t errorCode)
{
    LOGV("sendNackMsgToTSP_RISM()  remoteServiceType:%d errorCode:%d", remoteServiceType, errorCode);
    svc::NGTPServiceData serviceData;

    serviceData.set_appAck(svc::nack);
    serviceData.set_appAckRequired(FALSE);
    serviceData.set_messageType (svc::actionResponse);
    serviceData.set_testFlag (FALSE);

    svc::NGTPServiceData::serviceDataCoreMessage data_core;

    svc::NGTPServiceData::serviceDataCoreMessage::uplink uplink;

    assembleMsgToTSP_RISM(&uplink, svc::actionResponse, false, errorCode);

    data_core.set_uplink(uplink);

    serviceData.set_serviceDataCoreMessage(data_core);

    //encode
    svc::NGTPServiceData_PDU svcDataPDU;
    svc::tsdp_svc_Control ctl;
    PEREncodedBuffer encodedSvcData;

    ctl.setEncodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setEncodingRules(OSS_PER_UNALIGNED);
    svcDataPDU.set_data (serviceData);

    if(svcDataPDU.encode(ctl, encodedSvcData) != 0) {
        LOGE("NGTPServiceData_PDU encode fails :%s", ctl.getErrorMsg());
    } else {
        //print xml
        encodedSvcData.print_xml (ctl, 1);

        //InternalNGTPData
        InternalNGTPData internalNGTPData;

        internalNGTPData.serviceType = getNgtpServiceType(RS_RISM_MOVE);
        internalNGTPData.creationTime_seconds = getNgtpTimeSec(RS_RISM_MOVE);
        internalNGTPData.creationTime_millis = getNgtpTimeMil(RS_RISM_MOVE);

        internalNGTPData.dsptAckRequired = TRUE;
        internalNGTPData.statelessNGTPmessage = FALSE;
        internalNGTPData.messageType = dspt::upServiceData;
        internalNGTPData.tspErrorCode = 0;
        internalNGTPData.encodedSize  = encodedSvcData.get_data_size();
        internalNGTPData.transport = 0;
        internalNGTPData.timeoutCheck = 0;

        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_RETRY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = RVC_NGTP_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_ACK_DELAY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = RVC_NGTP_RETRY_DELAY;

        memcpy( internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
        LOGI("[SEND NGTP MSG] type:%d second:%d millis:%d", remoteServiceType, internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);
        if(getSLDDcmdActiveStatus()) {
            setSLDDcmdActiveStatus(false);
        } else {
            m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
        }
    }
    //RISM request CAN signal clear
//    resetSeatMoveReq();

    RvcKeepPowerRelease();
}

void RemoteServiceApp::sendRISMUpdateMsgToTSP()
{
    LOGV("sendRISMUpdateMsgToTSP()");

    svc::NGTPServiceData serviceData;

    serviceData.omit_appAck();
    serviceData.set_appAckRequired(FALSE);
    serviceData.set_messageType (svc::update);
    serviceData.set_testFlag (FALSE);

    svc::NGTPServiceData::serviceDataCoreMessage data_core;

    svc::NGTPServiceData::serviceDataCoreMessage::uplink uplink;

    assembleMsgToTSP_RISM(&uplink, svc::update, false, NULL);

    data_core.set_uplink(uplink);

    serviceData.set_serviceDataCoreMessage(data_core);

    //encode
    svc::NGTPServiceData_PDU svcDataPDU;
    svc::tsdp_svc_Control ctl;
    PEREncodedBuffer encodedSvcData;

    ctl.setEncodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setEncodingRules(OSS_PER_UNALIGNED);
    svcDataPDU.set_data (serviceData);

    if(svcDataPDU.encode(ctl, encodedSvcData) != 0) {
        LOGE("NGTPServiceData_PDU encode fails :%s", ctl.getErrorMsg());
    } else {
        //print xml
        encodedSvcData.print_xml (ctl, 1);

        //InternalNGTPData
        InternalNGTPData internalNGTPData;

        internalNGTPData.serviceType = getNgtpServiceType(RS_RISM_MOVE);
        internalNGTPData.creationTime_seconds = getRISMUpdateNgtpTimeSec();
        internalNGTPData.creationTime_millis = getRISMUpdateNgtpTimeMil();

        internalNGTPData.dsptAckRequired = TRUE;
        internalNGTPData.statelessNGTPmessage = FALSE;
        internalNGTPData.messageType = dspt::upServiceData;
        internalNGTPData.tspErrorCode = 0;
        internalNGTPData.encodedSize  = encodedSvcData.get_data_size();
        internalNGTPData.transport = 0;
        internalNGTPData.timeoutCheck = 0;

        uint8_t retryNum;
        sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_UDP_RETRY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryNum);
            internalNGTPData.retryNum = retryNum;
        }
        else
            internalNGTPData.retryNum = RVC_NGTP_UDP_RETRY_CNT;

        uint16_t retryDelay;
        buf = m_configManager->get(COMMON_CONFIG_FILE, 2, STR_NGTP_UDP_DELAY);
        if(buf != NULL && buf->errorFlag() == E_OK)
        {
            buf->data(&retryDelay);
            internalNGTPData.retryDelay = retryDelay;
        }
        else
            internalNGTPData.retryDelay = RVC_NGTP_UDP_RETRY_DELAY;

        memcpy( internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
        LOGI("[SEND_MSG_TO_TSP] RISM Update creationTime_seconds:%d creationTime_millis:%d", internalNGTPData.creationTime_seconds, internalNGTPData.creationTime_millis);

        m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
    }
}

void RemoteServiceApp::assembleMsgToTSP_RISM(svc::NGTPServiceData::serviceDataCoreMessage::uplink *uplink, svc::ServiceMessageType svcMessageType, bool appAck, uint8_t errorCode)
{
    // NGTPUplinkCore ngtpObj omit field
    uplink->omit_positionExtension();
    uplink->omit_vehicleStatus();
    uplink->omit_extendedVehicleStatus();
    uplink->omit_dashboardStatus();
    uplink->omit_electricVehicleStatus();
    uplink->omit_climateStatus();
    uplink->omit_tuStatus();
    uplink->omit_pearlChain();
    uplink->omit_lastGPSTimeStamp();
    uplink->omit_vin();
    uplink->omit_navigationInformation();
    uplink->omit_memoryStatus();
    uplink->omit_vehiclePropulsionType();
    uplink->omit_crashData();
    uplink->omit_remoteOperationResponse();
    uplink->omit_genericText();
    uplink->omit_genericParameters();
    uplink->omit_trigger();
    uplink->omit_alerts();
    uplink->omit_trackpoints();
    uplink->omit_journeys();
    uplink->omit_msdInfo();
    uplink->omit_debugInfo();
    uplink->omit_errorCodes();

    if(svcMessageType == svc::acknowledge)
    {
        //rismAcknowledge field
        svc::NGTPUplinkCore::rismAcknowledge rismAck;
        assembleMsgToTSP_rismAcknowledge(&rismAck, appAck, errorCode);
        uplink->set_rismAcknowledge(rismAck);
    }
    else if(svcMessageType == svc::actionResponse)
    {
        //rismResponse field
        svc::NGTPUplinkCore::rismResponse rismResp;
        assembleMsgToTSP_rismResponse(&rismResp);
        uplink->set_rismResponse(rismResp);
    }
    else if(svcMessageType == svc::update)
    {
        //rismUpdate field
        svc::NGTPUplinkCore::rismUpdate rismUpdate;
        assembleMsgToTSP_rismUpdate(&rismUpdate);
        uplink->set_rismUpdate(rismUpdate);
    }
    else
        LOGV("ignore serviceMessageType:%d", svcMessageType);
}

void RemoteServiceApp::assembleMsgToTSP_rismAcknowledge(svc::NGTPUplinkCore::rismAcknowledge *rismAck, bool appAck, uint8_t errorCode)
{
    if(!appAck)  //Nack
    {
        svc::NGTPUplinkCore::rismAcknowledge::errorCode errCode;
        getRISMerrorCode(&errCode, errorCode);
        //if(rismAck->errorCode_is_present())
        rismAck->set_errorCode(errCode);
    }
    else  //Ack
        rismAck->omit_errorCode();

    svc::NGTPUplinkCore::rismAcknowledge::pasSeat1stRowStatus p1stRowStatus;
    svc::NGTPUplinkCore::rismAcknowledge::drvSeat2ndRowStatus d2ndRowStatus;
    svc::NGTPUplinkCore::rismAcknowledge::pasSeat2ndRowStatus p2ndRowStatus;
    svc::NGTPUplinkCore::rismAcknowledge::drvSeat3rdRowStatus d3rdRowStatus;
    svc::NGTPUplinkCore::rismAcknowledge::pasSeat3rdRowStatus p3rdRowStatus;

    svc::NGTPUplinkCore::rismAcknowledge::leftSeat2ndRowStatus leftSeat;
    svc::NGTPUplinkCore::rismAcknowledge::armrest2ndRowStatus armrest;
    svc::NGTPUplinkCore::rismAcknowledge::rightSeat2ndRowStatus rightSeat;
    svc::NGTPUplinkCore::rismAcknowledge::skihatch2ndRowStatus skihatch;

    p1stRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_1P()));
    p1stRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_1P()));
    d2ndRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2D()));
    d2ndRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2D()));
    p2ndRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2P()));
    p2ndRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2P()));
    d3rdRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_3D()));
    d3rdRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_3D()));
    p3rdRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_3P()));
    p3rdRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_3P()));

    leftSeat.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2L()));
    leftSeat.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2L()));
    armrest.set_seatMovementState(getNgtpSeatMovementState_armRest(getSeatMovementStatus_2C()));
    armrest.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2C()));
    rightSeat.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2R()));
    rightSeat.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2R()));
    skihatch.set_seatMovementState(getNgtpSeatMovementState_skiHatch(getSeatMovementStatus_skiHatch()));
    skihatch.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2C()));
    /* harmony JLRTCU-2287 #3
    TCU should read the ''RMSeatInhib2ndRow'' CAN signal to know the inhibition status of Ski-hatch.
    Also as per ASN, the ski-hatch inhibition field on NGTP message should be filled as
    { unknown, inhibited, notInhibited -- i.e. movement allowed }
    */

    rismAck->set_pasSeat1stRowStatus(p1stRowStatus);
    rismAck->set_drvSeat2ndRowStatus(d2ndRowStatus);
    rismAck->set_pasSeat2ndRowStatus(p2ndRowStatus);
    rismAck->set_drvSeat3rdRowStatus(d3rdRowStatus);
    rismAck->set_pasSeat3rdRowStatus(p3rdRowStatus);

    rismAck->set_leftSeat2ndRowStatus(leftSeat);
    rismAck->set_armrest2ndRowStatus(armrest);
    rismAck->set_rightSeat2ndRowStatus(rightSeat);
    rismAck->set_skihatch2ndRowStatus(skihatch);
}

void RemoteServiceApp::assembleMsgToTSP_rismResponse(svc::NGTPUplinkCore::rismResponse *rismResp)
{
    svc::NGTPUplinkCore::rismResponse::pasSeat1stRowStatus p1stRowStatus;
    svc::NGTPUplinkCore::rismResponse::drvSeat2ndRowStatus d2ndRowStatus;
    svc::NGTPUplinkCore::rismResponse::pasSeat2ndRowStatus p2ndRowStatus;
    svc::NGTPUplinkCore::rismResponse::drvSeat3rdRowStatus d3rdRowStatus;
    svc::NGTPUplinkCore::rismResponse::pasSeat3rdRowStatus p3rdRowStatus;

    svc::NGTPUplinkCore::rismResponse::leftSeat2ndRowStatus leftSeat;
    svc::NGTPUplinkCore::rismResponse::armrest2ndRowStatus armrest;
    svc::NGTPUplinkCore::rismResponse::skihatch2ndRowStatus skihatch;
    svc::NGTPUplinkCore::rismResponse::rightSeat2ndRowStatus rightSeat;

    p1stRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_1P()));
    p1stRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_1P()));
    d2ndRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2D()));
    d2ndRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2D()));
    p2ndRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2P()));
    p2ndRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2P()));
    d3rdRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_3D()));
    d3rdRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_3D()));
    p3rdRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_3P()));
    p3rdRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_3P()));

    leftSeat.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2L()));
    leftSeat.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2L()));
    armrest.set_seatMovementState(getNgtpSeatMovementState_armRest(getSeatMovementStatus_2C()));
    armrest.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2C()));
    rightSeat.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2R()));
    rightSeat.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2R()));
    skihatch.set_seatMovementState(getNgtpSeatMovementState_skiHatch(getSeatMovementStatus_skiHatch()));
    skihatch.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2C()));
    /* harmony JLRTCU-2287 #3
    TCU should read the ''RMSeatInhib2ndRow'' CAN signal to know the inhibition status of Ski-hatch.
    Also as per ASN, the ski-hatch inhibition field on NGTP message should be filled as
    { unknown, inhibited, notInhibited -- i.e. movement allowed }
    */

    rismResp->set_pasSeat1stRowStatus(p1stRowStatus);
    rismResp->set_drvSeat2ndRowStatus(d2ndRowStatus);
    rismResp->set_pasSeat2ndRowStatus(p2ndRowStatus);
    rismResp->set_drvSeat3rdRowStatus(d3rdRowStatus);
    rismResp->set_pasSeat3rdRowStatus(p3rdRowStatus);

    rismResp->set_leftSeat2ndRowStatus(leftSeat);
    rismResp->set_armrest2ndRowStatus(armrest);
    rismResp->set_skihatch2ndRowStatus(skihatch);
    rismResp->set_rightSeat2ndRowStatus(rightSeat);
}

void RemoteServiceApp::assembleMsgToTSP_rismUpdate(svc::NGTPUplinkCore::rismUpdate *rismUpdate)
{
    svc::NGTPUplinkCore::rismUpdate::pasSeat1stRowStatus p1stRowStatus;
    svc::NGTPUplinkCore::rismUpdate::drvSeat2ndRowStatus d2ndRowStatus;
    svc::NGTPUplinkCore::rismUpdate::pasSeat2ndRowStatus p2ndRowStatus;
    svc::NGTPUplinkCore::rismUpdate::drvSeat3rdRowStatus d3rdRowStatus;
    svc::NGTPUplinkCore::rismUpdate::pasSeat3rdRowStatus p3rdRowStatus;

    svc::NGTPUplinkCore::rismUpdate::leftSeat2ndRowStatus leftSeat;
    svc::NGTPUplinkCore::rismUpdate::armrest2ndRowStatus armrest;
    svc::NGTPUplinkCore::rismUpdate::skihatch2ndRowStatus skihatch;
    svc::NGTPUplinkCore::rismUpdate::rightSeat2ndRowStatus rightSeat;

    p1stRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_1P()));
    p1stRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_1P()));
    d2ndRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2D()));
    d2ndRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2D()));
    p2ndRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2P()));
    p2ndRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2P()));
    d3rdRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_3D()));
    d3rdRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_3D()));
    p3rdRowStatus.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_3P()));
    p3rdRowStatus.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_3P()));

    leftSeat.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2L()));
    leftSeat.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2L()));
    armrest.set_seatMovementState(getNgtpSeatMovementState_armRest(getSeatMovementStatus_2C()));
    armrest.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2C()));
    rightSeat.set_seatMovementState(getNgtpSeatMovementState(getSeatMovementStatus_2R()));
    rightSeat.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2R()));
    skihatch.set_seatMovementState(getNgtpSeatMovementState_skiHatch(getSeatMovementStatus_skiHatch()));
    skihatch.set_seatMovementInhibition(getNgtpSeatMovementInhibition(getSeatMovementInhibition_2C()));
    /* harmony JLRTCU-2287 #3
    TCU should read the ''RMSeatInhib2ndRow'' CAN signal to know the inhibition status of Ski-hatch.
    Also as per ASN, the ski-hatch inhibition field on NGTP message should be filled as
    { unknown, inhibited, notInhibited -- i.e. movement allowed }
    */

    rismUpdate->set_pasSeat1stRowStatus(p1stRowStatus);
    rismUpdate->set_drvSeat2ndRowStatus(d2ndRowStatus);
    rismUpdate->set_pasSeat2ndRowStatus(p2ndRowStatus);
    rismUpdate->set_drvSeat3rdRowStatus(d3rdRowStatus);
    rismUpdate->set_pasSeat3rdRowStatus(p3rdRowStatus);

    rismUpdate->set_leftSeat2ndRowStatus(leftSeat);
    rismUpdate->set_armrest2ndRowStatus(armrest);
    rismUpdate->set_skihatch2ndRowStatus(skihatch);
    rismUpdate->set_rightSeat2ndRowStatus(rightSeat);
}

void RemoteServiceApp::assembleMsgToTSP(svc::NGTPServiceData::serviceDataCoreMessage::uplink *uplink)
{
    getServiceDataClimateStatus();
    getServiceDataTuStatus();

    // NGTPUplinkCore ngtpObj omit field
    uplink->omit_positionExtension();
    uplink->omit_dashboardStatus();
    uplink->omit_electricVehicleStatus();
    uplink->omit_pearlChain();
    uplink->omit_lastGPSTimeStamp();
    uplink->omit_vin();
    uplink->omit_navigationInformation();
    uplink->omit_memoryStatus();
    uplink->omit_vehiclePropulsionType();
    uplink->omit_crashData();
    uplink->omit_remoteOperationResponse();
    uplink->omit_genericText();
    uplink->omit_genericParameters();
    uplink->omit_trigger();
    uplink->omit_alerts();
    uplink->omit_trackpoints();
    uplink->omit_journeys();
    uplink->omit_msdInfo();
    uplink->omit_debugInfo();
    uplink->omit_errorCodes();

    //vehicleStatus field
    svc::NGTPUplinkCore::vehicleStatus vSts;
    assembleMsgToTSP_vehicleStatus(&vSts);
    uplink->set_vehicleStatus(vSts);

    //ExtendedVehicleStatus field
    svc::NGTPUplinkCore::extendedVehicleStatus evSts;
    assembleMsgToTSP_extendedVehicleStatus(&evSts);
    uplink->set_extendedVehicleStatus(evSts);

    //climateStatus field
    svc::NGTPUplinkCore::climateStatus cSts;
    assembleMsgToTSP_climateStatus(&cSts);
    uplink->set_climateStatus(cSts);

#if 0
    //tuStatus field
    svc::NGTPUplinkCore::tuStatus tuSts;
    assembleMsgToTSP_tuStatus(&tuSts);
    uplink->set_tuStatus(tuSts);
#else
    uplink->omit_tuStatus();
#endif
}


void RemoteServiceApp::assembleMsgToTSP_vehicleStatus(svc::NGTPUplinkCore::vehicleStatus *vSts)
{
    vSts->set_state(getNgtpVehicleState());
    vSts->set_mainBattery(svc::battery_empty);
    vSts->set_theftAlarm(getNgtpTheftAlarm());
    vSts->set_fuelLevelPerc(mFuelGaugePosition);
    vSts->set_srs(svc::srs_not_deployed);
    vSts->set_engineBlock(svc::normalUnblocked);
    vSts->set_brazilEventMode(svc::isFalse);
}

void RemoteServiceApp::assembleMsgToTSP_extendedVehicleStatus(svc::NGTPUplinkCore::extendedVehicleStatus *evSts)
{
    evSts->omit_fuelAmountDl();
    evSts->omit_vehicleDateTime();
    evSts->omit_ecoGrade();
    evSts->omit_engHrsToSrvc();
    evSts->omit_monthsToSrvc();
    evSts->omit_srvcWarnStatus();
    evSts->omit_srvcWarnTrigger();
    evSts->omit_tripMeter1();
    evSts->omit_tripMeter2();
    evSts->omit_avgFuelConsCl();
    evSts->omit_avgSpeedKmph();

    evSts->set_panicAlarmTriggered(svc::isUnknown);
    evSts->set_crashSituation(getNgtpCrashSituation());
    evSts->set_batteryVoltx10(120);
    evSts->set_cabOpenStatus(getNgtpCabOpenStatus());
    evSts->set_sunroofOpenStatus(getNgtpSunroofOpenStatus());
    evSts->set_bootOpenStatus(getNgtpBootOpenStatus());
    evSts->set_distToEmptyFuelKm(mDistanceToEmpty);
    evSts->set_kmToSrvc(0);
    evSts->set_engineCoolantTempDeg(90);
    evSts->set_headLightOn(svc::isUnknown);
    evSts->set_brakeFluidWarn(svc::LiquidLevelWarning_normal);
    evSts->set_engCoolantLevelWarn(svc::LiquidLevelWarning_normal);
    evSts->set_oilLevelWarn(svc::LiquidLevelWarning_normal);
    evSts->omit_oilPressureWarn();
    evSts->set_washerFluidWarn(svc::LiquidLevelWarning_low);

    svc::NGTPUplinkCore::extendedVehicleStatus::windowStatus wSts;
    assembleMsgToTSP_windowStatus(&wSts);
    evSts->set_windowStatus(wSts);

    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus dSts;
    assembleMsgToTSP_doorStatus(&dSts);
    evSts->set_doorStatus(dSts);

    svc::NGTPUplinkCore::extendedVehicleStatus::odometerValue oVal;
    assembleMsgToTSP_odometerValue(&oVal);
    evSts->set_odometerValue(oVal);

    svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses tyreSts;
    assembleMsgToTSP_tyreStatuses(&tyreSts);
    evSts->set_tyreStatuses(tyreSts);

    svc::NGTPUplinkCore::extendedVehicleStatus::bulbFailures bulbFail;
    assembleMsgToTSP_bulbFailures(&bulbFail);
    evSts->set_bulbFailures(bulbFail);
}

void RemoteServiceApp::assembleMsgToTSP_windowStatus(svc::NGTPUplinkCore::extendedVehicleStatus::windowStatus *wSts)
{
    wSts->set_front_left((svc::WindowStatus)mPasWindowStatus);
    wSts->set_front_right((svc::WindowStatus)mDrvWindowStatus);
    wSts->set_rear_left((svc::WindowStatus)mRPasWindowStatus);
    wSts->set_rear_right((svc::WindowStatus)mRDrvWindowStatus);
}

void RemoteServiceApp::assembleMsgToTSP_doorStatus(svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus *dSts)
{
    dSts->set_allDoorsLocked((svc::BOOLEXT) bAllDoorLockStatus);
    dSts->set_trunkLocked((svc::BOOLEXT) bTrunkLockStatus);
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus indSts;
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus::front_left dSfl;
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus::front_right dSfr;
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus::rear_left dSrl;
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus::rear_right dSrr;
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus::tailgate dStail;
    svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus::doorStatus::engineHood dSeh;
    dSfl.set_doorStatus((svc::DoorPosition) mFLdoorOpenStatus);
    dSfl.set_lockStatus((svc::DoorLockStatus) mFLdoorLockStatus);
    dSfr.set_doorStatus((svc::DoorPosition) mFRdoorOpenStatus);
    dSfr.set_lockStatus((svc::DoorLockStatus) mFRdoorLockStatus);
    dSrl.set_doorStatus((svc::DoorPosition) mRLdoorOpenStatus);
    dSrl.set_lockStatus((svc::DoorLockStatus) mRLdoorLockStatus);
    dSrr.set_doorStatus((svc::DoorPosition) mRRdoorOpenStatus);
    dSrr.set_lockStatus((svc::DoorLockStatus) mRRdoorLockStatus);
    dStail.set_doorStatus((svc::DoorPosition) mTailgateOpenStatus);
    dStail.set_lockStatus((svc::DoorLockStatus) mTailgateLockStatus);
    dSeh.set_doorStatus((svc::DoorPosition) mBonnetOpenStatus);
    dSeh.set_lockStatus((svc::DoorLockStatus) mBonnetLockStatus);
    indSts.set_front_left(dSfl);
    indSts.set_front_right(dSfr);
    indSts.set_rear_left(dSrl);
    indSts.set_rear_right(dSrr);
    indSts.set_tailgate(dStail);
    indSts.set_engineHood(dSeh);
    dSts->set_doorStatus(indSts);
}

void RemoteServiceApp::assembleMsgToTSP_odometerValue(svc::NGTPUplinkCore::extendedVehicleStatus::odometerValue *oVal)
{
    oVal->set_meterResolution(TRUE);
    oVal->set_odometerValueMtr(mOdometerMasterValue);
    oVal->set_deciMileResolution(FALSE);
    oVal->set_odometerValueDeciMiles(mODODisplayedMiles);
}

void RemoteServiceApp::assembleMsgToTSP_tyreStatuses(svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses *tyreSts)
{
    svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses::front_left tSfl;
    svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses::front_right tSfr;
    svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses::rear_left tSrl;
    svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses::rear_right tSrr;
    tSfl.set_warning(svc::no_sensors);
    tSfl.set_pressureHpa(32767);
    tSfr.set_warning(svc::no_sensors);
    tSfr.set_pressureHpa(32767);
    tSrl.set_warning(svc::no_sensors);
    tSrl.set_pressureHpa(32767);
    tSrr.set_warning(svc::no_sensors);
    tSrr.set_pressureHpa(32767);
    tyreSts->set_front_left(tSfl);
    tyreSts->set_front_right(tSfr);
    tyreSts->set_rear_left(tSrl);
    tyreSts->set_rear_right(tSrr);
}

void RemoteServiceApp::assembleMsgToTSP_bulbFailures(svc::NGTPUplinkCore::extendedVehicleStatus::bulbFailures *bulbFail)
{
    bulbFail->insert_after(OSS_NOINDEX, svc::left_turn_any);  //temporary value. example message bulbFailures size is 0.
}

void RemoteServiceApp::assembleMsgToTSP_climateStatus(svc::NGTPUplinkCore::climateStatus *cSts)
{
    cSts->omit_heatedSeatFrontLeft();
    cSts->omit_heatedSeatFrontRight();
    cSts->omit_heatedSeatRearLeft();
    cSts->omit_heatedSeatRearRight();

    cSts->set_timerActivationStatus(mClimateParameters.timerActivationStatus);
    cSts->set_climateOperatingStatus(mClimateParameters.climateOperatingStatus);
    cSts->set_remainingRunTime(mClimateParameters.remainingRunTime);
    cSts->set_ventingTime(mClimateParameters.ventingTime);
    cSts->set_heatedFrontScreenActive(mClimateParameters.heatedFrontScreenActive);
    cSts->set_heatedRearWindowActive(mClimateParameters.heatedRearWindowActive);
    cSts->set_heatedSteeringWheelActive(mClimateParameters.heatedSteeringWheelActive);

    svc::NGTPUplinkCore::climateStatus::timer1 tm1;
    tm1.set_month(mClimateParameters.timer1month);
    tm1.set_day(mClimateParameters.timer1day);
    svc::NGTPUplinkCore::climateStatus::timer1::time tmhm1;
    tmhm1.set_hour(mClimateParameters.timer1hour);
    tmhm1.set_minute(mClimateParameters.timer1minute);
    tm1.set_time(tmhm1);
    cSts->set_timer1(tm1);

    svc::NGTPUplinkCore::climateStatus::timer2 tm2;
    tm2.set_month(mClimateParameters.timer2month);
    tm2.set_day(mClimateParameters.timer2day);
    svc::NGTPUplinkCore::climateStatus::timer2::time tmhm2;
    tmhm2.set_hour(mClimateParameters.timer2hour);
    tmhm2.set_minute(mClimateParameters.timer2minute);
    tm2.set_time(tmhm2);
    cSts->set_timer2(tm2);
}
#if 0
omit_tuStatus
void RemoteServiceApp::assembleMsgToTSP_tuStatus(svc::NGTPUplinkCore::tuStatus *tuSts)
{
    tuSts->omit_hwVersion();
    tuSts->omit_swVersionMain();
    tuSts->omit_swVersionSecondary();
    tuSts->omit_swVersionConfiguration();
    tuSts->omit_serialNumber();
    tuSts->omit_imei();

    tuSts->set_power(mDiagParameters.power);
    tuSts->set_mobilePhoneConnected(svc::isUnused);
    tuSts->set_configVersion(164);
    tuSts->set_activation(svc::provisioned);
    tuSts->set_usesExternalGnss(svc::isFalse);

    svc::NGTPUplinkCore::tuStatus::diagnostics diag;
    assembleMsgToTSP_diagnostics(&diag);
    tuSts->set_diagnostics(diag);

    svc::NGTPUplinkCore::tuStatus::battery batt;
    assembleMsgToTSP_battery(&batt);
    tuSts->set_battery(batt);

    svc::NGTPUplinkCore::tuStatus::sleepCyclesStartTime sleepCycleTime;
    assembleMsgToTSP_sleepCyclesStartTime(&sleepCycleTime);
    tuSts->set_sleepCyclesStartTime(sleepCycleTime);
}

void RemoteServiceApp::assembleMsgToTSP_diagnostics(svc::NGTPUplinkCore::tuStatus::diagnostics *diag)
{
    diag->set_speaker(mDiagParameters.speaker);
    diag->set_mic(mDiagParameters.mic);
    diag->set_extHandsfree(mDiagParameters.extHandsfree);
    diag->set_handset(mDiagParameters.handset);
    diag->set_gsmModem(mDiagParameters.gsmModem);
    diag->set_gsmExtAntenna(mDiagParameters.gsmExtAntenna);
    diag->set_extPower(mDiagParameters.extPower);
    diag->set_intPower(mDiagParameters.intPower);
    diag->set_gnss(mDiagParameters.gnss);
    diag->set_gnssAntenna(mDiagParameters.gnssAntenna);
    diag->set_can(mDiagParameters.can);
    diag->set_buttons(mDiagParameters.buttons);
    diag->set_crashInput(mDiagParameters.crashInput);
    diag->set_intRtc(mDiagParameters.intRtc);
    diag->set_daysSinceGnssFix(mDiagParameters.daysSinceGnssFix);
}

void RemoteServiceApp::assembleMsgToTSP_battery(svc::NGTPUplinkCore::tuStatus::battery *batt)
{
    batt->set_primaryVoltx10(mBatteryParameters.primaryVoltx10);
    batt->set_primaryChargePerc(mBatteryParameters.primaryChargePerc);
    batt->set_secondaryVoltx10(mBatteryParameters.secondaryVoltx10);
}

void RemoteServiceApp::assembleMsgToTSP_sleepCyclesStartTime(svc::NGTPUplinkCore::tuStatus::sleepCyclesStartTime *sleepCycleTime)
{
    sleepCycleTime->set_seconds(499549692);
}
#endif

svc::ClimateOperatingStatus RemoteServiceApp::getClimateOperatingStatus()
{
    uint8_t FFHOperatingStatus = getFFHOperatingStatus();
    int8_t currParkClimateMode = getCurrParkClimateModeTCU();

    svc::ClimateOperatingStatus status = svc::ClimateOperatingStatus_off;
    // SRD_ReqRPC2134_V1
    switch(currParkClimateMode)
    {
        case CLIMATE_MODE_OFF:
            switch (FFHOperatingStatus)
            {
                case FFH_OFF:               status = svc::ClimateOperatingStatus_off;           break;
                case FFH_STARTINGUP:        status = svc::starting_up;                          break;
                case FFH_SHUTDOWN:          status = svc::shutting_down;                        break;
                case FFH_FAIL_LOW_VOLTAGE:  status = svc::failure_low_voltage;                  break;
                case FFH_FAIL_LOW_FUEL:     status = svc::failure_low_fuel;                     break;
                case FFH_FAIL_SVC_REQ:      status = svc::failure_service_required;             break;
                case FFH_FAIL_SYS_FAILURE:  status = svc::failure_system_failure;               break;
                case FFH_FAIL_MAX_TIME:     status = svc::failure_max_operational_time_reached; break;
                case FFH_STOP_BY_USER:      status = svc::stopped_by_user;                      break;
                case FFH_RUNNING:           status = svc::ClimateOperatingStatus_unknown;       break;
                case FFH_STATUS_UNKNOWN:    status = svc::ClimateOperatingStatus_unknown;       break;
                default:                    status = svc::ClimateOperatingStatus_unknown;       break;
            }
            break;
        case CLIMATE_MODE_STARTUP:
            status = svc::starting_up;
            break;
        case CLIMATE_ENGINE_HEATING:    // JLRTCU-579
            status = svc::engine_heating;
            break;
        case CLIMATE_CABIN_HEATING:
#ifdef TSDP_SVC_3_3
            status = svc::heating;
#else
            status = svc::ClimateOperatingStatus_heating;
#endif
            break;
        case CLIMATE_CABIN_VENTING:
            status = svc::venting;
            break;
        default:
            status = svc::ClimateOperatingStatus_unknown;
            break;
    }
    return status;
}

void RemoteServiceApp::getServiceDataClimateStatus()
{
    //get climateStatus
    mClimateParameters.timerActivationStatus = (svc::BOOLEXT) getRPCtimerActiveStatus();
    mClimateParameters.timer1month = getTimer1Month();
    mClimateParameters.timer1day = getTimer1Day();
    mClimateParameters.timer1hour = getTimer1Hour();
    mClimateParameters.timer1minute = getTimer1Minute();
    mClimateParameters.timer2month = getTimer2Month();
    mClimateParameters.timer2day = getTimer2Day();
    mClimateParameters.timer2hour = getTimer2Hour();
    mClimateParameters.timer2minute = getTimer2Minute();
    mClimateParameters.climateOperatingStatus = getClimateOperatingStatus();
    mClimateParameters.remainingRunTime = getRESremainingMinute();
    mClimateParameters.ventingTime = (9 + getParkHeatVentTime()); // mParkHeatVentTime에는 실제 신호 값을 저장하고, NGTP 메시지를 만들 때 offset 추가..
    mClimateParameters.heatedFrontScreenActive = (svc::BOOLEXT) mHFSCommand;
    mClimateParameters.heatedRearWindowActive = (svc::BOOLEXT) mHRWCommand;
    mClimateParameters.heatedSteeringWheelActive = svc::isFalse;
}

void RemoteServiceApp::getServiceDataTuStatus()
{
#if 1  //temporary rollback
    mDiagParameters.power = svc::mainBattery;
    mDiagParameters.speaker = svc::functioning;
    mDiagParameters.mic = svc::functioning;
    mDiagParameters.extHandsfree = svc::broken;
    mDiagParameters.handset = svc::uncertain;
    mDiagParameters.gsmModem = svc::functioning;
    mDiagParameters.gsmExtAntenna = svc::functioning;
    mDiagParameters.extPower = svc::functioning;
    mDiagParameters.intPower = svc::functioning;
    mDiagParameters.gnss = svc::functioning;
    mDiagParameters.gnssAntenna = svc::functioning;
    mDiagParameters.can = svc::functioning;
    mDiagParameters.buttons = svc::functioning;
    mDiagParameters.crashInput = svc::broken;
    mDiagParameters.intRtc = svc::functioning;
    mDiagParameters.daysSinceGnssFix = 0;

    mBatteryParameters.primaryVoltx10 = 39;
    mBatteryParameters.primaryChargePerc = 100;
    mBatteryParameters.secondaryVoltx10 = 37;
    mBatteryParameters.secondaryChargePerc = 100;
#else
    sp<TUStatusInfo> tcu_status = new TUStatusInfo();

    //get tuStatus
    m_diagManager->GetTUStatusData(tcu_status);
    mDiagParameters.power = tcu_status->TCUStatus.Power;
    mDiagParameters.speaker = tcu_status->TCUStatus.speaker;
    mDiagParameters.mic = tcu_status->TCUStatus.mic;
    mDiagParameters.extHandsfree = svc::broken;
    mDiagParameters.handset = svc::uncertain;
    mDiagParameters.gsmModem = tcu_status->TCUStatus.gsmModem;
    mDiagParameters.gsmExtAntenna = tcu_status->TCUStatus.gsmExtAntenna;
    mDiagParameters.extPower = tcu_status->TCUStatus.extPower;
    mDiagParameters.intPower = tcu_status->TCUStatus.intPower;
    mDiagParameters.gnss = svc::functioning;
    mDiagParameters.gnssAntenna = svc::functioning;
    mDiagParameters.can = tcu_status->TCUStatus.can;
    mDiagParameters.buttons = tcu_status->TCUStatus.button;
    mDiagParameters.crashInput = tcu_status->TCUStatus.crashInput;
    mDiagParameters.intRtc = tcu_status->TCUStatus.intRtc;
    mDiagParameters.daysSinceGnssFix = 0;

    mBatteryParameters.primaryVoltx10 = tcu_status->TCUStatus.Primary_Battery_Voltage;
    mBatteryParameters.primaryChargePerc = tcu_status->TCUStatus.Primary_Charge_Percentage;
    mBatteryParameters.secondaryVoltx10 = tcu_status->TCUStatus.Secondary_Battery_Voltage;
    mBatteryParameters.secondaryChargePerc = tcu_status->TCUStatus.Secondary_Charge_Percentage;
#endif
}

int32_t RemoteServiceApp::getNgtpServiceType(RS_ServiceType remoteServiceType)
{
    int32_t ngtpService = -1;  // TODO: check default value

    switch(remoteServiceType)
    {
        case RS_UNLOCK_VEHICLE:
            ngtpService = NGTP_ST_RDU;
            break;

        case RS_SECURE_VEHICLE:
            ngtpService = NGTP_ST_RDL;
            break;

        case RS_ALARM_RESET:
            ngtpService = NGTP_ST_ALOFF;
            break;

        case RS_HONK_FLASH:
            ngtpService = NGTP_ST_HBLF;
            break;

        case RS_ENGINE_START:
            ngtpService = NGTP_ST_REON;
            break;

        case RS_ENGINE_STOP:
            ngtpService = NGTP_ST_REOFF;
            break;

        case RS_PARK_CLIMATE_AUTO:
            ngtpService = NGTP_ST_RHON;
            break;

        case RS_PARK_CLIMATE_STOP:
            ngtpService = NGTP_ST_RHOFF;
            break;

        case RS_RISM_MOVE:
            ngtpService = NGTP_ST_RISM;
            break;

        case RS_SCHEDULED_WAKEUP:
            ngtpService = NGTP_ST_SCHEDULEDWAKUP;
            break;

        default:
            break;
    }
    return ngtpService;
}

uint32_t RemoteServiceApp::getNgtpTimeSec(RS_ServiceType remoteServiceType)
{
    uint32_t seconds = 0;

    switch(remoteServiceType)
    {
        case RS_UNLOCK_VEHICLE:
            seconds = getRDUngtpTimeSec();
            break;

        case RS_SECURE_VEHICLE:
            seconds = getRDLngtpTimeSec();
            break;

        case RS_ALARM_RESET:
            seconds = getALOFFngtpTimeSec();
            break;

        case RS_HONK_FLASH:
            seconds = getHBLFngtpTimeSec();
            break;

        case RS_ENGINE_START:
            seconds = getREONngtpTimeSec();
            break;

        case RS_ENGINE_STOP:
            seconds = getREOFFngtpTimeSec();
            break;

        case RS_PARK_CLIMATE_AUTO:
            seconds = getRHONngtpTimeSec();
            break;

        case RS_PARK_CLIMATE_STOP:
            seconds = getRHOFFngtpTimeSec();
            break;

        case RS_RISM_MOVE:
            seconds = getRISMMoveNgtpTimeSec();
            break;

        case RS_SCHEDULED_WAKEUP:
            seconds = getScheduledWakeupNgtpTimeSec();
            break;

        default:
            break;
    }
    return seconds;
}

uint32_t RemoteServiceApp::getNgtpTimeMil(RS_ServiceType remoteServiceType)
{
    uint32_t millis = 0;

    switch(remoteServiceType)
    {
        case RS_UNLOCK_VEHICLE:
            millis = getRDUngtpTimeMil();
            break;

        case RS_SECURE_VEHICLE:
            millis = getRDLngtpTimeMil();
            break;

        case RS_ALARM_RESET:
            millis = getALOFFngtpTimeMil();
            break;

        case RS_HONK_FLASH:
            millis = getHBLFngtpTimeMil();
            break;

        case RS_ENGINE_START:
            millis = getREONngtpTimeMil();
            break;

        case RS_ENGINE_STOP:
            millis = getREOFFngtpTimeMil();
            break;

        case RS_PARK_CLIMATE_AUTO:
            millis = getRHONngtpTimeMil();
            break;

        case RS_PARK_CLIMATE_STOP:
            millis = getRHOFFngtpTimeMil();
            break;

        case RS_RISM_MOVE:
            millis = getRISMMoveNgtpTimeMil();
            break;

        case RS_SCHEDULED_WAKEUP:
            millis = getScheduledWakeupNgtpTimeMil();
            break;

        default:
            break;
    }
    return millis;
}

svc::VehicleStateType RemoteServiceApp::getNgtpVehicleState()
{
    svc::VehicleStateType state = svc::VehicleStateType_unknown;
    RS_PowerMode mode = RemoteService_getPowerMode();
    bool resStatus = RemoteService_getRemoteStartStatus();

    switch(mode)
    {
        case PM_RUNNING_2:
            if(resStatus)
                state = svc::engine_on_remote_start;
            else
                state = svc::engine_on_moving;  //if vehicleSpeed == 0 then state = engine_on_park
            break;

        case PM_ACCESSORY_1:     //FALL-THROUGH
        case PM_POST_IGNITION_1: //FALL-THROUGH
        case PM_IGNITION_ON_2:
            state = svc::key_on_engine_off;
            break;

        case PM_POST_ACCESSORY_0:
            state = svc::delayed_accessory;
            break;

        case PM_KEY_OUT:
            state = svc::key_removed;
            break;

        default:
            LOGE("unknown PowerMode:%d", mode);
            break;
    }
    LOGV("getNgtpVehicleState() PowerMode:%d, RESstatus:%d result:%d", mode, resStatus, state);
    return state;
}

svc::TheftAlarmStatus RemoteServiceApp::getNgtpTheftAlarm()
{
    switch(getCurrAlarmStatus())
    {
        case ALARM_SET:
            return svc::alarm_armed;
        case ALARM_UNSET:
            return svc::alarm_off;
        case ALARM_ACTIVE:
            return svc::alarm_trig;
        case ALARM_IGNORE:  //FALL-THROUGH
        default:
            return svc::no_alarm_info;
    }
}

svc::BOOLEXT RemoteServiceApp::getNgtpSunroofOpenStatus()
{
    if(!isSunroofExist()) // sunroof does not exist
    {
        return svc::isUnused;
    }
    else if(getSunroofOpenStatus() == 0) // 0:FullyClosed
    {
        return svc::isFalse;
    }
    else if(getSunroofOpenStatus() == 1) // 1:NotFullyClosed
    {
        return svc::isTrue;
    }
    else
    {
        return svc::isUnknown;
    }
}

svc::BOOLEXT RemoteServiceApp::getNgtpCabOpenStatus()
{
    switch(getRoofPositionStatus())
    {
        case 0: //Closed
            return svc::isFalse;
        case 1: // Open    //FALL-THROUGH
        case 2: // Closing //FALL-THROUGH
        case 3: // Opening //FALL-THROUGH
        case 4: // Stalled
            return svc::isTrue;
        case 5: // Undefined (Unused) //FALL-THROUGH
        case 6: // Undefined (Unused) //FALL-THROUGH
        case 7: // Undefined (Unused)
            return svc::isUnused;
        default:
            return svc::isUnknown;
    }
}

svc::BOOLEXT RemoteServiceApp::getNgtpBootOpenStatus()
{
    switch(mTailgateOpenStatus)
    {
        case svc::DoorPosition_closed:
            return svc::isFalse;
        case svc::DoorPosition_open:
            return svc::isTrue;
        default:
            return svc::isUnknown;
    }
}

svc::SeatMovementInhibition RemoteServiceApp::getNgtpSeatMovementInhibition(seatMovementInhibition inhibition)
{
    switch(inhibition)
    {
        case seatMovementInhibition_available:
            return svc::notInhibited;
        case seatMovementInhibition_inhibited:
            return svc::inhibited;
        default:
            return svc::SeatMovementInhibition_unknown;
    }
}

svc::SeatMovementStatus RemoteServiceApp::getNgtpSeatMovementState(seatMovementStatus state)
{
/*    SRD_ReqRISM0434
    Note : For seat positions “5 - One Touch Reclined”, “6 - Fully Reclined” and “7 - Return to
    home”, the TCU should treat them as “2 ? Seat Fully Unfolded” seat position.
*/
    switch(state)
    {
        case seatMovementStatus_default:
            return svc::SeatMovementStatus_unknown;
        case seatMovementStatus_seatFullyFolded:
            return svc::folded;
        case seatMovementStatus_seatFullyUnfolded:
            return svc::unfolded;
        case seatMovementStatus_seatMidTravelStationary:
            return svc::midTravelStationary;
        case seatMovementStatus_seatMoving:
            return svc::moving;
        case seatMovementStatus_oneTouchReclined:   //FALL-THROUGH
        case seatMovementStatus_fullyReclined:      //FALL-THROUGH
        case seatMovementStatus_returnedToHome:
            return svc::unfolded;
        default:
            return svc::SeatMovementStatus_unknown;
    }
}

svc::SeatMovementStatus RemoteServiceApp::getNgtpSeatMovementState_armRest(seatMovementStatus state)
{
    switch(state)
    {
        case seatMovementStatus_stowedUpPosition:
            return svc::folded;
        case seatMovementStatus_deployedDownPosition:
            return svc::unfolded;
        case seatMovementStatus_default:                    //FALL-THROUGH
        case seatMovementStatus_seatFullyFolded:            //FALL-THROUGH
        case seatMovementStatus_seatFullyUnfolded:          //FALL-THROUGH
        case seatMovementStatus_seatMidTravelStationary:    //FALL-THROUGH
        case seatMovementStatus_seatMoving:
            return (svc::SeatMovementStatus)state;
        default:
            return svc::SeatMovementStatus_unknown;
    }
}

svc::SeatMovementStatus RemoteServiceApp::getNgtpSeatMovementState_skiHatch(seatMovementStatus state)
{
    switch(state)
    {
        case seatMovementStatus_closed:
            return svc::folded;
        case seatMovementStatus_open:
            return svc::unfolded;
        default:
            return svc::SeatMovementStatus_unknown;
    }
}

svc::BOOLEXT RemoteServiceApp::getNgtpCrashSituation()
{
    svc::BOOLEXT crashStatus = svc::isFalse;
    char* prop = getProperty(STR_CURR_CRASH);
    if(prop != NULL)
    {
        crashStatus = ((std::strcmp(prop, STR_FALSE)) ? svc::isTrue : svc::isFalse);
    }
    return crashStatus;
}

void RemoteServiceApp::setWindowStatus(uint16_t sigId, uint8_t sigVal)
{
    //LOGV("window status position: %d, status:%d", position, status);
    uint8_t windowStatus;

    switch(sigVal)
    {
        case 0: //FALL-THROUGH
        case 1:
            windowStatus = svc::WindowStatus_closed;
            break;
        case 2: //FALL-THROUGH
        case 3: //FALL-THROUGH
        case 4: //FALL-THROUGH
        case 5:
            windowStatus = svc::WindowStatus_open;
            break;
        default:
            windowStatus = svc::WindowStatus_unknown;
            break;
    }

    switch(sigId)
    {
        case Signal_DriverWindowPosition_RX:
            mDrvWindowStatus = windowStatus;
            break;
        case Signal_RearDriverWindowPos_RX: //FALL-THROUGH
        case Signal_DriverRearWindowPos_RX:
            mRDrvWindowStatus = windowStatus;
            break;
        case Signal_PassWindowPosition_RX:
            mPasWindowStatus = windowStatus;
            break;
        case Signal_RearPassWindowPos_RX: //FALL-THROUGH
        case Signal_PassRearWindowPos_RX:
            mRPasWindowStatus = windowStatus;
            break;
         default:
            break;
    }
/*
    if (sigId == Signal_DriverWindowPosition_RX)
        mDrvWindowStatus = ((status == 1) ? svc::WindowStatus_closed : svc::WindowStatus_open );
    else if (sigId == Signal_RearDriverWindowPos_RX || sigId == Signal_DriverRearWindowPos_RX)
        mRDrvWindowStatus = ((status == 1) ? svc::WindowStatus_closed: svc::WindowStatus_open );
    else if (sigId == Signal_PassWindowPosition_RX)
        mPasWindowStatus = ((status == 1) ? svc::WindowStatus_closed: svc::WindowStatus_open );
    else if (sigId == Signal_RearPassWindowPos_RX || sigId == Signal_PassRearWindowPos_RX)
        mRPasWindowStatus = ((status == 1) ? svc::WindowStatus_closed : svc::WindowStatus_open );
    else
        LOGE("invalid window position");
        */
}

void RemoteServiceApp::setDoorOpenStatus(uint8_t status)
{
    //LOGV("setDoorOpenStatus:%d", status);
    mDoorStatus = status;
    mFRdoorOpenStatus = (((status >> 0) & 1) ? svc::DoorPosition_closed : svc::DoorPosition_open);
    mFLdoorOpenStatus = (((status >> 1) & 1) ? svc::DoorPosition_closed : svc::DoorPosition_open);
    mRLdoorOpenStatus = (((status >> 2) & 1) ? svc::DoorPosition_closed : svc::DoorPosition_open);
    mRRdoorOpenStatus = (((status >> 3) & 1) ? svc::DoorPosition_closed : svc::DoorPosition_open);
    mTailgateOpenStatus = (((status >> 4) & 1) ? svc::DoorPosition_closed  : svc::DoorPosition_open);
    mBonnetOpenStatus = (((status >> 5) & 1) ? svc::DoorPosition_closed  : svc::DoorPosition_open);

    mBonnetLockStatus = (((status >> 5) & 1) ? svc::locked : svc::unlocked);
}

void RemoteServiceApp::setDistanceToEmpty(uint16_t value)
{
    uint8_t buffer[10];
    sprintf((char*) buffer, "%d", value);
    LOGI("setDistanceToEmpty :%d", value);

    //LOGV("DistanceToEmpty:%d", value);
    if(value < DIST_EMPTY_MAX)
    {
        mDistanceToEmpty = value;
        setProperty(PROP_DISTANCE_TO_EMPTY, (const char *) buffer);
    }
    else if( value == DIST_EMPTY_MAX)
        LOGI("invalid DistanceToEmpty value 1023");
    else
        LOGI("invalid DistanceToEmpty value value:%d", value);
}

void RemoteServiceApp::setOdometerMasterValue(uint32_t value)
{
    uint8_t buffer[10];
    sprintf((char*) buffer, "%d", value);
    LOGI("setOdometerMasterValue :%d", value);

    //LOGV("OdometerMasterValue:%d", value);
    // CAN signal max value 166777214 Km but NGTP max value 2147483647 meter
    if( value > INIT_VALUE_ZERO && value < ODOMETER_MAX)  //  need do not check zero
    {
        mOdometerMasterValue = value * 1000;  // km to meter
        setProperty(PROP_ODEMETER_MASTER_VALUE, (const char *) buffer);
    }
    else if( value >= ODOMETER_MAX)
    {
        mOdometerMasterValue = ODOMETER_MAX * 1000;  // km to meter
        setProperty(PROP_ODEMETER_MASTER_VALUE, (const char *) buffer);
    }
    else
        LOGI("invalid OdometerMasterValue:%d", value);
}

void RemoteServiceApp::setODODisplayedMiles(uint32_t value)
{
    uint8_t buffer[10];
    sprintf((char*) buffer, "%d", value);
    LOGI("setODODisplayedMiles :%d 0x%x", value, value);

    // 0xFFFFFFh = Invalid Data
    if((value > INIT_VALUE_ZERO) && (value <= ODO_DISPLAY_MILES_MAX))
    {
        mODODisplayedMiles = value;
        setProperty(PROP_ODO_DISPLAY_MILES, (const char *) buffer);
    }
}


void RemoteServiceApp::setFuelGaugePosition(uint8_t value)
{
    uint8_t buffer[10];
    sprintf((char*) buffer, "%d", value);
    LOGI("setFuelGaugePosition :%d ", value);

    //LOGV("FuelGaugePosition:%d", value);
    if(value <= FUEL_POS_MAX)
    {
        mFuelGaugePosition = value * FUEL_POS_INTERVAL;
        setProperty(PROP_FUEL_GAUGE_POSITION, (const char *) buffer);
    }
    else
        LOGI("invalid FuelGaugePosition:%d", value);
}
void RemoteServiceApp::setFuelLevelIndicated(uint16_t level)
{
    LOGI("setFuelLevelIndicated : %d -> %d", mFuelLevelIndicated, level);
    if(mFuelLevelIndicated != level)
    {
        uint8_t buffer[10];
        sprintf((char*) buffer, "%d", level);
        mFuelLevelIndicated = level;
        setProperty(PROP_FUEL_LEVEL_INDICATED_HS, (const char *) buffer);
    }
}

void RemoteServiceApp::setFuelLevelIndicatedQF(uint8_t accuracy)
{
    LOGI("setFuelLevelIndicatedQF : %d -> %d",mFuelLevelIndicatedQF, accuracy);
    if(mFuelLevelIndicatedQF != accuracy)
    {
        uint8_t buffer[10];
        sprintf((char*) buffer, "%d", accuracy);
        mFuelLevelIndicatedQF = accuracy;
        setProperty(PROP_FUEL_LEVEL_INDICATED_QFHS, (const char *) buffer);
    }
}


uint32_t RemoteServiceApp::getRESremainingMinute()
{
    uint16_t remainingTime_m = 0;
    uint16_t remainingTime_s = getUint16fromConfig(RVC_RES_RUNMAX);

    if(remainingTime_s < 60)
        remainingTime_m = 0;
    else
        remainingTime_m = remainingTime_s/60;

    LOGI("getRESremainingMinute remainingTime second:%d minute:%d", remainingTime_s, remainingTime_m);

    if(remainingTime_m > 255)  // NGTP service data range
    {
        LOGE("getRESremainingMinute. out of range remainingRuntime. set to default.");
        remainingTime_m = getUint16fromConfig(RES_ENGINE_RUN_MAX);
    }

    return remainingTime_m;
}

void RemoteServiceApp::setNGTPerrorCode(uint8_t errorCode)
{
    LOGI("setNGTPerrorCode() START mErrorIdx:%d errorCode:%d", mErrorIdx, errorCode);

    svc::NGTPUplinkCore::errorCodes::component compErrCode;

    switch(errorCode)
    {
        case notAllowed_chosen: {
            compErrCode.set_notAllowed(NULL);
        } break;

        case vehiclePowerModeNotCorrect_chosen: {
            compErrCode.set_vehiclePowerModeNotCorrect(NULL);
        } break;

        case conditionsNotCorrect_chosen: {
            compErrCode.set_conditionsNotCorrect(NULL);
        } break;

        case executionFailure_chosen: {
            compErrCode.set_executionFailure(NULL);
        } break;

        case fuelLevelTooLow_chosen: {
            compErrCode.set_fuelLevelTooLow(NULL);
        } break;

        case runTimeExpiry_chosen: {
            compErrCode.set_runTimeExpiry(NULL);
        } break;

        case theftEvent_chosen: {
            compErrCode.set_theftEvent(NULL);
        } break;

        case crashEvent_chosen: {
            compErrCode.set_crashEvent(NULL);
        } break;

        case timeout_chosen: {
            compErrCode.set_timeout(NULL);
        } break;

        case windowNotClosed_chosen: {
            compErrCode.set_windowNotClosed(NULL);
        } break;

        case hazardWarningOn_chosen: {
            compErrCode.set_hazardWarningOn(NULL);
        } break;

         case vehicleNotLocked_chosen: {
            compErrCode.set_vehicleNotLocked(NULL);
        } break;

        case alarmActive_chosen: {
            compErrCode.set_alarmActive(NULL);
        } break;

         case brakePedalPressed_chosen: {
            compErrCode.set_brakePedalPressed(NULL);
        } break;

        case vehicleNotStationary_chosen: {
            compErrCode.set_vehicleNotStationary(NULL);
        } break;

        case transmissionNotInPark_chosen: {
            compErrCode.set_transmissionNotInPark(NULL);
        } break;

        case climateModeNotCorrect_chosen: {
            compErrCode.set_climateModeNotCorrect(NULL);
        } break;

        default: {
            compErrCode.set_conditionsNotCorrect(NULL);
        } break;
    }

    mErrorIdx = mErrCode.insert_after(mErrorIdx, compErrCode);

    LOGI("setNGTPerrorCode() END mErrorIdx:%d", mErrorIdx);
}

void RemoteServiceApp::initNGTPerrorCode()
{
    while(!mErrCode.empty())
    {
       mErrCode.remove_front();
    }
    LOGI("initNGTPerrorCode() END");
}

void RemoteServiceApp::getRISMerrorCode(svc::NGTPUplinkCore::rismAcknowledge::errorCode *errCode, uint8_t errorCode)
{
    LOGV("getRISMerrorCode() errorCode:%d", errorCode);

    switch(errorCode)
    {
        case notAllowed_chosen:
            errCode->set_notAllowed(NULL);
            break;

        case vehiclePowerModeNotCorrect_chosen:
            errCode->set_vehiclePowerModeNotCorrect(NULL);
            break;

        case conditionsNotCorrect_chosen:
            errCode->set_conditionsNotCorrect(NULL);
            break;

        case executionFailure_chosen:
            errCode->set_executionFailure(NULL);
            break;

        case theftEvent_chosen:
            errCode->set_theftEvent(NULL);
            break;

        case crashEvent_chosen:
            errCode->set_crashEvent(NULL);
            break;

        case timeout_chosen:
            errCode->set_timeout(NULL);
            break;

         case vehicleNotLocked_chosen:
            errCode->set_vehicleNotLocked(NULL);
            break;

        case alarmActive_chosen:
            errCode->set_alarmActive(NULL);
            break;

        default:
            errCode->set_conditionsNotCorrect(NULL);
            break;
    }
}

uint32_t RemoteServiceApp::getREONngtpTimeSec() { return mREONngtpTimeSec; }
uint32_t RemoteServiceApp::getREONngtpTimeMil() { return mREONngtpTimeMil; }
uint32_t RemoteServiceApp::getREOFFngtpTimeSec() { return mREOFFngtpTimeSec; }
uint32_t RemoteServiceApp::getREOFFngtpTimeMil() { return mREOFFngtpTimeMil; }
uint32_t RemoteServiceApp::getRHONngtpTimeSec() { return mRHONngtpTimeSec; }
uint32_t RemoteServiceApp::getRHONngtpTimeMil() { return mRHONngtpTimeMil; }
uint32_t RemoteServiceApp::getRHOFFngtpTimeSec() { return mRHOFFngtpTimeSec; }
uint32_t RemoteServiceApp::getRHOFFngtpTimeMil() { return mRHOFFngtpTimeMil; }

uint32_t RemoteServiceApp::getRDUngtpTimeSec() { return mRDUngtpTimeSec; }
uint32_t RemoteServiceApp::getRDLngtpTimeSec() { return mRDLngtpTimeSec; }
uint32_t RemoteServiceApp::getHBLFngtpTimeSec() { return mHBLFngtpTimeSec; }
uint32_t RemoteServiceApp::getALOFFngtpTimeSec() { return mALOFFngtpTimeSec; }
uint32_t RemoteServiceApp::getRDUngtpTimeMil() { return mRDUngtpTimeMil; }
uint32_t RemoteServiceApp::getRDLngtpTimeMil() { return mRDLngtpTimeMil; }
uint32_t RemoteServiceApp::getHBLFngtpTimeMil() { return mHBLFngtpTimeMil; }
uint32_t RemoteServiceApp::getALOFFngtpTimeMil() { return mALOFFngtpTimeMil; }

uint32_t RemoteServiceApp::getRISMMoveNgtpTimeSec() { return mRISMMoveNgtpTimeSec; }
uint32_t RemoteServiceApp::getRISMMoveNgtpTimeMil() { return mRISMMoveNgtpTimeMil; }
uint32_t RemoteServiceApp::getRISMCancelNgtpTimeSec() { return mRISMCancelNgtpTimeSec; }
uint32_t RemoteServiceApp::getRISMCancelNgtpTimeMil() { return mRISMCancelNgtpTimeMil; }

uint32_t RemoteServiceApp::getRISMUpdateNgtpTimeSec() { return mRISMUpdateNgtpTimeSec; }
uint32_t RemoteServiceApp::getRISMUpdateNgtpTimeMil() { return mRISMUpdateNgtpTimeMil; }

uint32_t RemoteServiceApp::getScheduledWakeupNgtpTimeSec() { return mScheduledWakeupNgtpTimeSec; }
uint32_t RemoteServiceApp::getScheduledWakeupNgtpTimeMil() { return mScheduledWakeupNgtpTimeMil; }

void RemoteServiceApp::setRISMMoveNgtpTimeSec(uint32_t sec) { mRISMMoveNgtpTimeSec = sec; }
void RemoteServiceApp::setRISMMoveNgtpTimeMil(uint32_t mil) { mRISMMoveNgtpTimeMil = mil; }
void RemoteServiceApp::setRISMCancelNgtpTimeSec(uint32_t sec) { mRISMCancelNgtpTimeSec = sec; }
void RemoteServiceApp::setRISMCancelNgtpTimeMil(uint32_t mil) { mRISMCancelNgtpTimeMil = mil; }

void RemoteServiceApp::setRISMUpdateNgtpTimeSec(uint32_t sec) { mRISMUpdateNgtpTimeSec = sec; }
void RemoteServiceApp::setRISMUpdateNgtpTimeMil(uint32_t mil) { mRISMUpdateNgtpTimeMil = mil; }

void RemoteServiceApp::setREONngtpTimeSec(uint32_t sec) { mREONngtpTimeSec = sec; }
void RemoteServiceApp::setREONngtpTimeMil(uint32_t mil) { mREONngtpTimeMil = mil; }
void RemoteServiceApp::setREOFFngtpTimeSec(uint32_t sec) { mREOFFngtpTimeSec = sec; }
void RemoteServiceApp::setREOFFngtpTimeMil(uint32_t mil) { mREOFFngtpTimeMil = mil; }
void RemoteServiceApp::setRHONngtpTimeSec(uint32_t sec) { mRHONngtpTimeSec = sec; }
void RemoteServiceApp::setRHONngtpTimeMil(uint32_t mil) { mRHONngtpTimeMil = mil; }
void RemoteServiceApp::setRHOFFngtpTimeSec(uint32_t sec) { mRHOFFngtpTimeSec = sec; }
void RemoteServiceApp::setRHOFFngtpTimeMil(uint32_t mil) { mRHOFFngtpTimeMil = mil; }

void RemoteServiceApp::setRDUngtpTimeSec(uint32_t sec) { mRDUngtpTimeSec = sec; }
void RemoteServiceApp::setRDLngtpTimeSec(uint32_t sec) { mRDLngtpTimeSec = sec; }
void RemoteServiceApp::setHBLFngtpTimeSec(uint32_t sec) { mHBLFngtpTimeSec = sec; }
void RemoteServiceApp::setALOFFngtpTimeSec(uint32_t sec) { mALOFFngtpTimeSec = sec; }
void RemoteServiceApp::setRDUngtpTimeMil(uint32_t mil) { mRDUngtpTimeMil = mil; }
void RemoteServiceApp::setRDLngtpTimeMil(uint32_t mil) { mRDLngtpTimeMil = mil; }
void RemoteServiceApp::setHBLFngtpTimeMil(uint32_t mil) { mHBLFngtpTimeMil = mil; }
void RemoteServiceApp::setALOFFngtpTimeMil(uint32_t mil) { mALOFFngtpTimeMil = mil; }

void RemoteServiceApp::setScheduledWakeupNgtpTimeSec(uint32_t sec) { mScheduledWakeupNgtpTimeSec = sec; }
void RemoteServiceApp::setScheduledWakeupNgtpTimeMil(uint32_t mil) { mScheduledWakeupNgtpTimeMil = mil; }

