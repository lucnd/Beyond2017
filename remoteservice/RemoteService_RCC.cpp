/**
* \file    RemoteService_RCC.cpp
* \brief     Implementation of RemoteService_RCC Class.
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       dongjin1.seo
* \date    2015.06.25
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/
#define LOG_TAG "RVC_RCC"
#include "Log.h"

#include "RemoteService_RCC.h"




RemoteService_RCC::RemoteService_RCC(RemoteServiceApp& app)
    :m_RemoteServiceApp(app)
{

}


RemoteService_RCC::~RemoteService_RCC(){

}

void RemoteService_RCC::requestHandler(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;
    bool prerequisite = false;

    LOGE("requestHandler() serviceType: %d", serviceType);

    prerequisite = RemoteService_PrereqCheck(serviceType);

    if(prerequisite)
    {
        switch(serviceType)
        {
            case RS_TARGET_TEMP:
            {
                RCC_reqTempTargetTCU();
                //need check periodic generation
                RCC_reqClimateDisable();
                RCC_reqHeatedSeatFLMode();
                RCC_reqHeatedSeatFRMode();
                RCC_reqHeatedSeatRLMode();
                RCC_reqHeatedSeatRRMode();
                RCC_reqHeatedSeatFL();
                RCC_reqHeatedSeatFR();
                RCC_reqHeatedSeatRL();
                RCC_reqHeatedSeatRR();
                RCC_reqMaxDefrost();
            }
                break;

            default:
                break;
        }
    }
}

void RemoteService_RCC::responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size)
{
    uint8_t sigVal;

    sigVal = buf[0];
    LOGE("responseHandler sigId:%d sigVal:%d", sigId, sigVal);

    switch(sigId)
    {
        case Signal_HFSCommand_RX:
            m_RemoteServiceApp.setHFSCommand((bool)sigVal);
            break;
        case Signal_HRWCommand_RX:
            m_RemoteServiceApp.setHRWCommand((bool)sigVal);
            break;
        case Signal_HeatedSeatFLModeRequest_RX:
            m_RemoteServiceApp.setHeatedSeatFLModeReq(sigVal);
            break;
        case Signal_HeatedSeatFLRequest_RX:
            m_RemoteServiceApp.setHeatedSeatFLRequest(sigVal);
            break;
        case Signal_HeatedSeatFRModeRequest_RX:
            m_RemoteServiceApp.setHeatedSeatFRModeReq(sigVal);
            break;
        case Signal_HeatedSeatFRRequest_RX:
            m_RemoteServiceApp.setHeatedSeatFRRequest(sigVal);
            break;
        case Signal_HeatedSeatRLModeReq_RX:
            m_RemoteServiceApp.setHeatedSeatRLModeReq(sigVal);
            break;
        case Signal_HeatedSeatRLReq_RX:
            m_RemoteServiceApp.setHeatedSeatRLRequest(sigVal);
            break;
        case Signal_HeatedSeatRRModeReq_RX:
            m_RemoteServiceApp.setHeatedSeatRRModeReq(sigVal);
            break;
        case Signal_HeatedSeatRRReq_RX:
            m_RemoteServiceApp.setHeatedSeatRRRequest(sigVal);
            break;
        default:
            break;
    }
}

bool RemoteService_RCC::responseStatus(RS_ServiceType remoteServiceType)
{
    return false;
}

bool RemoteService_RCC::RCC_getFrontSystemOnCmd(){

    return false;
}


uint8_t RemoteService_RCC::RCC_getTempTargetTCU(){

    return  NULL;
}


void RemoteService_RCC::RCC_reqTempTargetTCU()
{
    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TempTargetTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = m_RemoteServiceApp.getUint8fromConfig(RCC_TARGET_TEMP);
    //LOGI("RCC_reqTempTargetTCU() TempTargetTCU: %d ---->>>>  ", values[0]);

    if ((values[0] != RCC_USER_TEMP) && (values[0] < RCC_MIN_TEMP || values[0] > RCC_MAX_TEMP))
    {
        values[0] = RCC_DEFAULT_TEMP;
    }

    LOGI("RCC_reqTempTargetTCU() TempTargetTCU: %d ", values[0]);

    buf->setData(Signal_TempTargetTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TempTargetTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqClimateDisable()
{
    LOGE("RCC_reqClimateDisable()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ClimateDisableReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 0;   //SRD_ReqRVC1205_V1: provisioned for future features

    buf->setData(Signal_ClimateDisableReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ClimateDisableReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatFLMode()
{
    LOGE("RCC_reqHeatedSeatFLMode()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFLModeReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 3;   //SRD_ReqRVC1206_V1: always be set to the default of 3

    buf->setData(Signal_HeatedSeatFLModeReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFLModeReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatFRMode()
{
    LOGE("RCC_reqHeatedSeatFRMode()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFRModeReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 3;   //SRD_ReqRVC1206_V1: always be set to the default of 3

    buf->setData(Signal_HeatedSeatFRModeReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFRModeReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatRLMode()
{
    LOGE("RCC_reqHeatedSeatRLMode()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRLModeReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 3;   //SRD_ReqRVC1206_V1: always be set to the default of 3

    buf->setData(Signal_HeatedSeatRLModeReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRLModeReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatRRMode()
{
    LOGE("RCC_reqHeatedSeatRRMode()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRRModeReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 3;   //SRD_ReqRVC1206_V1: always be set to the default of 3

    buf->setData(Signal_HeatedSeatRRModeReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRRModeReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatFL()
{
    LOGE("RCC_reqHeatedSeatFL()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFLRequestTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 0;   //SRD_ReqRVC1206_V1: always be set to the default of 0

    buf->setData(Signal_HeatedSeatFLRequestTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFLRequestTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatFR()
{
    LOGE("RCC_reqHeatedSeatFR()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFRRequestTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 0;   //SRD_ReqRVC1206_V1: always be set to the default of 0

    buf->setData(Signal_HeatedSeatFRRequestTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatFRRequestTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatRL()
{
    LOGE("RCC_reqHeatedSeatRL()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRLReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 0;   //SRD_ReqRVC1206_V1: always be set to the default of 0

    buf->setData(Signal_HeatedSeatRLReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRLReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqHeatedSeatRR()
{
    LOGE("RCC_reqHeatedSeatRR()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRRReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 0;   //SRD_ReqRVC1206_V1: always be set to the default of 0

    buf->setData(Signal_HeatedSeatRRReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_HeatedSeatRRReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

void RemoteService_RCC::RCC_reqMaxDefrost()
{
    LOGE("RCC_reqMaxDefrost()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_MaxDefrostTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = 0;   //SRD_ReqRVC1207_V1: always publish the MaxDefrostTCU signal as 0

    buf->setData(Signal_MaxDefrostTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_MaxDefrostTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
}

bool RemoteService_RCC::RemoteService_PrereqCheck(RS_ServiceType remoteServiceType)
{
    RS_PowerMode powerMode = m_RemoteServiceApp.RemoteService_getPowerMode();
    bool remoteStartStatus = m_RemoteServiceApp.getCurrRemoteStartStatus();

    if((powerMode == PM_RUNNING_2) && (remoteStartStatus == true))
    {
        LOGV("[RCC Prerequisite Success] PowerMode, RemoteStartStatus All OK powerMod:%d, remoteStartStatus:%d ",powerMode,remoteStartStatus);
        return true;
    }
    else
    {
        LOGV("[RCC Prerequisite Fail] PowerMode:%d RemoteStartStatus:%d", powerMode, remoteStartStatus);
        return false;
    }
}
