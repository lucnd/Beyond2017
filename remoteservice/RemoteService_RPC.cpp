/**
* \file    RemoteService_RPC.cpp
* \brief     Implementation of RemoteService_RPC Class.
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

#define LOG_TAG "RVC_RPC"
#include "Log.h"
#include "RemoteService_RPC.h"




RemoteService_RPC::RemoteService_RPC(RemoteServiceApp& app)
    :m_RemoteServiceApp(app)
{

}


RemoteService_RPC::~RemoteService_RPC(){

}
#if 0
void RemoteService_RPC::requestHandler(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;
    bool prerequisite = false;

    LOGE("requestHandler() serviceType: %d", serviceType);

    switch(serviceType)
    {
        case RS_PARK_CLIMATE_VENT:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "vent");
            break;
        case RS_PARK_CLIMATE_HEAT:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "heat");
            break;
        case RS_PARK_CLIMATE_AUTO:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "auto");
            break;
        case RS_PARK_CLIMATE_STOP:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "stop");
            break;
        default:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "invalid");
            break;
    }

    if( (m_RemoteServiceApp.getParkClimateSystemPresent() == true)
        && (m_RemoteServiceApp.getParkClimateRemoteEnabled() == true)
        && ( m_RemoteServiceApp.RemoteService_getPowerMode() == PM_KEY_OUT) )
    { // 5.9.3 Park Climate Status Check SRD_ReqRPC2118_V2
        m_RemoteServiceApp.updateFromCan_FFHOperatingStatus();
        m_RemoteServiceApp.updateFromCan_parkClimateMode();
    }

    prerequisite = RemoteService_PrereqCheck(serviceType);

    if(prerequisite)
    {
        switch(serviceType)
        {
            case RS_PARK_CLIMATE_VENT:
            {
                reqParkVenting();
            }
                break;
            case RS_PARK_CLIMATE_HEAT:
            {
                reqParkHeating();
            }
                break;
            case RS_PARK_CLIMATE_AUTO:
            {
                reqParkAuto();
            }
                break;
            case RS_PARK_CLIMATE_STOP:
            {
                reqParkStop();
            }
                break;
            default:
                break;
        }

        if(serviceType >= RS_PARK_CLIMATE_AUTO && serviceType <= RS_PARK_CLIMATE_HEAT)
        {
            m_RemoteServiceApp.setCurrState(serviceType, RS_WAIT_RESPONSE);
            m_RemoteServiceApp.startTimer(serviceType);
        }
    }
}
#else
void RemoteService_RPC::requestHandler(RS_ServiceType remoteServiceType)
{
    RS_ServiceState serviceState = m_RemoteServiceApp.getCurrState(remoteServiceType);
    LOGE("requestHandler serviceType: %d serviceState:%d ", remoteServiceType, serviceState);

    if(serviceState == RS_ACTIVE)
    {
        requestHandler_HVAC(remoteServiceType);
        return;
    }

    if(serviceState == RS_READY)
    {
        if(requestHandler_TCU(remoteServiceType) == false)
        {
            return;
        }
        m_RemoteServiceApp.setCurrState(remoteServiceType, RS_ACTIVE);
    }

    if(m_RemoteServiceApp.RemoteService_IsCanBusWaking() == false) // can sleep
    {
        m_RemoteServiceApp.reqDefaultParkClimate(); // to wake up can bus
    }
    else // can already waken up
    {
        if(m_RemoteServiceApp.isTimerOn(RemoteServiceApp::MyTimer::TIMER_PARK_CLIMATE_STATUS_TIMEOUT))
        {
            LOGE("?? TIMER_PARK_CLIMATE_STATUS_TIMEOUT ON", remoteServiceType);
            // do noting here. action will be taken upon signal receved or timer expired
        }
        else if(m_RemoteServiceApp.isTimerOn(RemoteServiceApp::MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT))
        {
            LOGE("?? TIMER_PARK_CLIMATE_MIN_STATUS_WAIT ON", remoteServiceType);

            // 5.9.4 Park Climate Status Timeout
            m_RemoteServiceApp.startTimerId(RemoteServiceApp::MyTimer::TIMER_PARK_CLIMATE_STATUS_TIMEOUT);
        }
        else // can already waken up and passed more than 6 secs
        {
            requestHandler_HVAC(remoteServiceType);
        }
    }
}
#endif

bool RemoteService_RPC::requestHandler_TCU(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;

    LOGE("requestHandler_TCU() serviceType: %d", serviceType);

    switch(serviceType)
    {
        case RS_PARK_CLIMATE_VENT:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "vent");
            break;
        case RS_PARK_CLIMATE_HEAT:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "heat");
            break;
        case RS_PARK_CLIMATE_AUTO:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "auto");
            break;
        case RS_PARK_CLIMATE_STOP:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "stop");
            break;
        default:
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, "invalid");
            break;
    }

    if(RemoteService_PrereqCheck_TCU(serviceType) == false)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void RemoteService_RPC::requestHandler_HVAC(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;
    bool prerequisite = false;

    LOGE("requestHandler_HVAC() serviceType: %d", serviceType);

    prerequisite = RemoteService_PrereqCheck_HVAC(serviceType);

    if(prerequisite)
    {
        switch(serviceType)
        {
            case RS_PARK_CLIMATE_VENT:
                reqParkVenting();
                break;
            case RS_PARK_CLIMATE_HEAT:
                reqParkHeating();
                break;
            case RS_PARK_CLIMATE_AUTO:
                reqParkAuto();
                break;
            case RS_PARK_CLIMATE_STOP:
                reqParkStop();
                break;
            default:
                return;
        }

        m_RemoteServiceApp.setCurrState(serviceType, RS_WAIT_RESPONSE);
        m_RemoteServiceApp.startTimer(serviceType);
    }
}

void RemoteService_RPC::responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size)
{

}

bool RemoteService_RPC::responseStatus(RS_ServiceType remoteServiceType)
{
    return false;
}

void RemoteService_RPC::reqParkVenting()
{
    LOGE("reqParkVenting() \n");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();

    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = RPC_REQ_PARK_VENT;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    m_RemoteServiceApp.setCurrParkClimateReqTCU(RPC_REQ_PARK_VENT);

    delete[] values;
}

void RemoteService_RPC::reqParkHeating()
{
    LOGE("reqParkHeating() \n");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();

    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = RPC_REQ_PARK_HEAT;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    m_RemoteServiceApp.setCurrParkClimateReqTCU(RPC_REQ_PARK_HEAT);

    delete[] values;
}

void RemoteService_RPC::reqParkAuto()
{
    LOGE("reqParkAuto() \n");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();

    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = RPC_REQ_PARK_AUTO;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    m_RemoteServiceApp.setCurrParkClimateReqTCU(RPC_REQ_PARK_AUTO);

    delete[] values;
}

void RemoteService_RPC::reqParkStop()
{
    LOGE("reqParkStop() \n");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();

    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = RPC_REQ_PARK_STOP;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    m_RemoteServiceApp.setCurrParkClimateReqTCU(RPC_REQ_PARK_STOP);

    delete[] values;
}


bool RemoteService_RPC::RPC_getParkClimateSystemPresent(RS_ServiceType serviceType)
{
    bool parkClimateSystemPresent = m_RemoteServiceApp.getParkClimateSystemPresent();

    //m_RemoteServiceApp.setRVCproperty(serviceType, NULL, SLDD_PROP_RPCSYS, ((parkClimateSystemPresent) ? STR_TRUE : STR_FALSE));
    if(parkClimateSystemPresent)
    {
        LOGI("[RPC Prerequisite Success] PARK_CLIMATE_SYSTEM_PRESENT is TRUE");
        return true;
    }
    else
    {
        LOGI("[RPC Prerequisite Fail] PARK_CLIMATE_SYSTEM_PRESENT is FALSE");
        return false;
    }

}

bool RemoteService_RPC::RPC_getParkClimateRemoteEnabled(RS_ServiceType serviceType)
{
    bool parkClimateRemoteEnabled = m_RemoteServiceApp.getParkClimateRemoteEnabled();

    //m_RemoteServiceApp.setRVCproperty(serviceType, NULL, SLDD_PROP_RPCEN, ((parkClimateRemoteEnabled) ? STR_TRUE : STR_FALSE));
    if(parkClimateRemoteEnabled)
    {
        LOGI("[RPC Prerequisite Success] PARK_CLIMATE_REMOTE_ENABLED is TRUE");
        return true;
    }
    else
    {
        LOGI("[RPC Prerequisite Fail] PARK_CLIMATE_REMOTE_ENABLED is FALSE");
        return false;
    }
}

bool RemoteService_RPC::RPC_getPowerMode(RS_ServiceType serviceType)
{
    RS_PowerMode powerMode = m_RemoteServiceApp.RemoteService_getPowerMode();

    //m_RemoteServiceApp.setRVCproperty(serviceType, NULL, SLDD_PROP_PWR, ((powerMode == PM_KEY_OUT) ? STR_OK : STR_FAIL));
    if(powerMode == PM_KEY_OUT)
    {
        LOGI("[RPC Prerequisite Success] PowerMode == 0");
        return true;
    }
    else
    {
        LOGI("[RPC Prerequisite Fail] PowerMode(%d) != 0", powerMode);
        return false;
    }
}

bool RemoteService_RPC::RPC_getFFHOperatingStatus(RS_ServiceType serviceType)
{
    uint8_t ffhOperatingStatus = m_RemoteServiceApp.getFFHOperatingStatus();

    //m_RemoteServiceApp.setRVCproperty(serviceType, NULL, SLDD_PROP_FFHOP, ((ffhOperatingStatus == FFH_OFF) ? STR_OK : STR_FAIL));
    if(ffhOperatingStatus == FFH_OFF)
    {
        LOGI("[RPC Prerequisite Success] FFHOperatingStatus == 0");
        return true;
    }
    else
    {
        LOGI("[RPC Prerequisite Fail] FFHOperatingStatus %d != FFH_OFF", ffhOperatingStatus);
        return false;
    }
}

bool RemoteService_RPC::RPC_getparkClimateModeTCU(RS_ServiceType serviceType)
{
    int8_t parkClimateMode = m_RemoteServiceApp.getCurrParkClimateModeTCU();

    if(serviceType == RS_PARK_CLIMATE_STOP)
    {
        //m_RemoteServiceApp.setRVCproperty(serviceType, NULL, SLDD_PROP_, ((parkClimateMode != FFH_OFF) ? STR_OK : STR_FAIL));
        if(parkClimateMode != FFH_OFF)
        {
            LOGI("[RPC Prerequisite Success] parkClimateModeTCU : %d ", parkClimateMode);
            return true;

        }
        else
        {
            LOGI("[RPC Prerequisite Fail] parkClimateModeTCU == 0");
            return false;
        }
    }
    else
    {
        //m_RemoteServiceApp.setRVCproperty(serviceType, NULL, SLDD_PROP_FFHOP, ((ffhOperatingStatus == FFH_OFF) ? STR_OK : STR_FAIL));
        if(parkClimateMode == FFH_OFF)
        {
            LOGI("[RPC Prerequisite Success] parkClimateModeTCU == 0");
            return true;

        }
        else
        {
            LOGI("[RPC Prerequisite Fail] parkClimateModeTCU : %d ", parkClimateMode);
            return false;
        }
    }
}


bool RemoteService_RPC::RemoteService_PrereqCheck(RS_ServiceType remoteServiceType)
{
    LOGV("RemoteService_PrereqCheck() serviceType:%d", remoteServiceType);

    bool result = true;

    if( remoteServiceType == RS_PARK_CLIMATE_AUTO || remoteServiceType == RS_PARK_CLIMATE_VENT || remoteServiceType == RS_PARK_CLIMATE_HEAT )
    {
        if(!RPC_getParkClimateSystemPresent(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(notAllowed_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "ParkClimateSystemPresent fail");
        }
        if(!RPC_getParkClimateRemoteEnabled(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(notAllowed_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "ParkClimateRemoteEnabled fail");
        }
        if(!RPC_getPowerMode(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "PowerMode fail");
        }
        if(!RPC_getparkClimateModeTCU(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "parkClimateMode fail");
        }
        if(!RPC_getFFHOperatingStatus(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "FFHOperatingStatus fail");
        }
    }

    if( remoteServiceType == RS_PARK_CLIMATE_STOP )
    {
        if(!RPC_getParkClimateSystemPresent(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(notAllowed_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "ParkClimateSystemPresent fail");
        }
        if(!RPC_getParkClimateRemoteEnabled(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(notAllowed_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "ParkClimateRemoteEnabled fail");
        }
        if(!RPC_getPowerMode(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "PowerMode fail");
        }
        if(!RPC_getparkClimateModeTCU(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
            m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "parkClimateMode fail");
        }
    }
    // climate start fail reason. errorCode
    // 4 /*notAllowed_chosen*/ : notAllowed FFH not fitted  <-- RPC_getParkClimateSystemPresent
    // 9 /*vehiclePowerModeNotCorrect_chosen*/: vehiclePowerModeNotCorrecct   <-- RPC_getPowerMode
    // 25 /*climateModeNotCorrect_chosen*/: climateModeNotCorrect <-- RPC_getparkClimateModeTCU
    // 11 The climate system could not start. The specific reason can be determined from the climateOperatingStatus field./*executionFailure_chosen*/: executionFailure <- RPC_getFFHOperatingStatus
    // 17 /*timeout_chosen*/:timeOut   <--  climate system did not respond tothe stop request.  PARK_CLIMATE_SHUTDOWN_TIME

    // climate stop fail reason. errorCode
    // 4 /*notAllowed_chosen*/ : notAllowed FFH not fitted  <-- RPC_getParkClimateSystemPresent
    // 9 /*vehiclePowerModeNotCorrect_chosen*/: vehiclePowerModeNotCorrecct   <-- RPC_getPowerMode
    // 25 /*climateModeNotCorrect_chosen*/: climateModeNotCorrect <-- RPC_getparkClimateModeTCU
    // 17 /*timeout_chosen*/:timeOut   <--  climate system did not respond tothe stop request.  PARK_CLIMATE_SHUTDOWN_TIME

    if(!result)
    {
        //m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, SLDD_PROP_SUCCESS, STR_FAIL);
        //m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, SLDD_PROP_CAUSE, STR_PRE_FAIL);

        m_RemoteServiceApp.stopTimer(remoteServiceType);
        m_RemoteServiceApp.sendNackMsgToTSP(remoteServiceType);
    }
    else
    {
        m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "success");
    }
    return result;
}

bool RemoteService_RPC::RemoteService_PrereqCheck_TCU(RS_ServiceType remoteServiceType)
{
    LOGV("RemoteService_PrereqCheck_TCU() serviceType:%d", remoteServiceType);

    bool result = true;

    if(!RPC_getParkClimateSystemPresent(remoteServiceType))
    {
        result = false;
        m_RemoteServiceApp.setNGTPerrorCode(notAllowed_chosen);
        m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "ParkClimateSystemPresent fail");
    }
    if(!RPC_getParkClimateRemoteEnabled(remoteServiceType))
    {
        result = false;
        m_RemoteServiceApp.setNGTPerrorCode(notAllowed_chosen);
        m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "ParkClimateRemoteEnabled fail");
    }
    if(!RPC_getPowerMode(remoteServiceType))
    {
        result = false;
        m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
        m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "PowerMode fail");
    }

    if(!result)
    {
        //m_RemoteServiceApp.stopTimer(remoteServiceType);
        m_RemoteServiceApp.sendNackMsgToTSP(remoteServiceType);
        m_RemoteServiceApp.setCurrState(remoteServiceType, RS_READY);
    }
    return result;
}

bool RemoteService_RPC::RemoteService_PrereqCheck_HVAC(RS_ServiceType remoteServiceType)
{
    LOGV("RemoteService_PrereqCheck_HVAC() serviceType:%d", remoteServiceType);

    bool result = true;

    switch(remoteServiceType)
    {
        case RS_PARK_CLIMATE_AUTO: //FALL-THROUGH
        case RS_PARK_CLIMATE_VENT: //FALL-THROUGH
        case RS_PARK_CLIMATE_HEAT:
            if(!RPC_getparkClimateModeTCU(remoteServiceType))
            {
                result = false;
                m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
                m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "parkClimateMode fail");
            }
            if(!RPC_getFFHOperatingStatus(remoteServiceType))
            {
                result = false;
                m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
                m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "FFHOperatingStatus fail");
            }
            break;
        case RS_PARK_CLIMATE_STOP:
            if(!RPC_getparkClimateModeTCU(remoteServiceType))
            {
                result = false;
                m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
                m_RemoteServiceApp.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "parkClimateMode fail");
            }
            break;
        default:
            result = false;
            break;
    }

    if(!result)
    {
        //m_RemoteServiceApp.stopTimer(remoteServiceType);
        m_RemoteServiceApp.sendNackMsgToTSP(remoteServiceType);
        m_RemoteServiceApp.setCurrState(remoteServiceType, RS_READY);
    }
    return result;
}

