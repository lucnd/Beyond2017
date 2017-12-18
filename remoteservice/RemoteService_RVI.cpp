/**
* \file    RemoteService_RVI.cpp
* \brief     Implementation of RemoteService_RVI Class.
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

#define LOG_TAG "RVC_RVI"
#include "Log.h"

#include "RemoteService_RVI.h"


RemoteService_RVI::RemoteService_RVI(RemoteServiceApp& app)
    :m_RemoteServiceApp(app)
{

}


RemoteService_RVI::~RemoteService_RVI(){

}

void RemoteService_RVI::requestHandler(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;
    error_t result = E_OK;

    LOGV("requestHandler() serviceType: %d", serviceType);

    if(RemoteService_PrereqCheck(serviceType))
    {
        if(RVI_delayForReqBCMauth(serviceType) == E_ERROR)
        {
            LOGE("requestHandler() serviceType:%d request fail", serviceType);
            return;
        }

        switch(serviceType)
        {
            case RS_UNLOCK_VEHICLE:
                result = RVI_reqVehicleUnlock();
                break;

            case RS_SECURE_VEHICLE:
                result = RVI_reqVehicleSecure();
                break;

            case RS_ALARM_RESET:
                result = RVI_reqAlarmReset();
                break;

            case RS_HONK_FLASH:
                result = RVI_reqLocationCheck();
                break;

            default:
               break;
        }

        if(result == E_OK)
        {
            m_RemoteServiceApp.reqBCMauthentication(serviceType);
        }
        else
        {
            m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
            m_RemoteServiceApp.sendNackMsgToTSP(serviceType);
            m_RemoteServiceApp.setCurrState(serviceType, RS_READY);
        }
    }
    else
    {
        m_RemoteServiceApp.setCurrState(serviceType, RS_READY);
        if(m_RemoteServiceApp.getDBGcmdActiveStatus())
        {
            //send to dbgMgr payload with 0x03
            sp<Buffer> resp = new Buffer();
            uint8_t *dbgResult = new uint8_t;

            dbgResult[0] = DBG_PRECOND_FAIL;
            resp->setTo(dbgResult, 0x01);

            switch(serviceType)
            {
                case RS_UNLOCK_VEHICLE:
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_UNLOCK, DBG_CMD_TYPE_REQUEST, resp);
                    break;

                case RS_SECURE_VEHICLE:
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_SECURE, DBG_CMD_TYPE_REQUEST, resp);
                    break;

                case RS_ALARM_RESET:
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_ALARM_RESET, DBG_CMD_TYPE_REQUEST, resp);
                    break;

                case RS_HONK_FLASH:
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_LOCATION, DBG_CMD_TYPE_REQUEST, resp);
                    break;

                default:
                   break;
            }
            delete dbgResult;
            m_RemoteServiceApp.setDBGcmdActiveStatus(false);
        }
        else
        {
            //already setted NGTP error code in prerequisite check routine.
            m_RemoteServiceApp.sendNackMsgToTSP(serviceType);
        }
    }
    LOGV("RemoteService_RVI::requestHandler() END");
}

void RemoteService_RVI::responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size)
{
    RS_ServiceType serviceType = remoteServiceType;
    uint8_t sigVal;
    uint8_t* dbgResult = new uint8_t;
    sp<Buffer> resp = new Buffer();
    bool result = false;
    bool dbgStatus = m_RemoteServiceApp.getDBGcmdActiveStatus();

    sigVal = buf[0];
    LOGV("responseHandler() serviceType: %d sigVal:0x%02X size:%d ", serviceType, sigVal, size);

    switch(serviceType)
    {
        case RS_UNLOCK_VEHICLE: {
            // read DoorLatchStatus
            if(sigVal & RVI_DOORLATCH_DRIVERUNLOCKED)
            {
                dbgResult[0] = DBG_RESP_SUCCESS;
                result = true;

                if(dbgStatus) {
                    resp->setTo(dbgResult, 0x01);
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_UNLOCK, DBG_CMD_TYPE_REQUEST, resp);
                }
            }
        } break;

        case RS_SECURE_VEHICLE: {
            if(sigVal == RVI_DOORLATCH_ALLLOCKED)
            {
                dbgResult[0] = DBG_RESP_SUCCESS;
                result = true;

                if(dbgStatus) {
                    resp->setTo(dbgResult, 0x01);
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_SECURE, DBG_CMD_TYPE_REQUEST, resp);
                }
            }
        } break;

        case RS_ALARM_RESET: {
            if((sigVal != RVI_ALARM_ACTIVATED) && (m_RemoteServiceApp.getCurrState(RS_ALARM_RESET) == RS_WAIT_RESPONSE))  {
                dbgResult[0] = DBG_RESP_SUCCESS;
                result = true;

                if(dbgStatus) {
                    resp->setTo(dbgResult, 0x01);
                    m_RemoteServiceApp.RemoteService_sendDBGcmdResponse(DBG_CMD_ALARM_RESET, DBG_CMD_TYPE_REQUEST, resp);
                }
            }
        } break;

        default:
            break;
    }

    if(result) {
        m_RemoteServiceApp.stopTimer(serviceType);
        if(!dbgStatus) {
            m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_SUCCESS, STR_SUCCESS);
            m_RemoteServiceApp.sendAckMsgToTSP(serviceType, svc::actionResponse);  //sendEndRequestMessage
        } else {
            m_RemoteServiceApp.setDBGcmdActiveStatus(false);
        }
    }

    delete dbgResult;
}

bool RemoteService_RVI::responseStatus(RS_ServiceType remoteServiceType)
{
    return false;
}

error_t RemoteService_RVI::RVI_reqAlarmReset()
{
    LOGV("RVI_reqAlarmReset()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_ALARM_RESET;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}


error_t RemoteService_RVI::RVI_reqLocationCheck()
{
    LOGV("RVI_reqLocationCheck()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_HONK_FLASH;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RVI::RVI_reqVehicleSecure()
{
    LOGV("RVI_reqVehicleSecure()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_SECURE_VEHICLE;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}


error_t RemoteService_RVI::RVI_reqVehicleUnlock()
{
    LOGV("RVI_reqVehicleUnlock()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_UNLOCK_VEHICLE;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RVI::RVI_delayForReqBCMauth(RS_ServiceType serviceType)
{
    error_t result = E_ERROR;
    int32_t count = BCM_RESP_WAIT_MAX_TIME;

    while(count > 0)
    {
        if(m_RemoteServiceApp.getCurrBCMauthService() == RS_SERVICE_TYPE_MAX)
        {
            LOGV("RVI (serviceType:%d) TCU-BCM authentication request available.", serviceType);
            result = E_OK;
            break;
        }
        else
        {
            LOGV("wait 1 second for RVI (serviceType:%d) TCU-BCM authentication request.", serviceType);
            usleep(BCM_RESP_WAIT_TIME);
            count -= BCM_RESP_WAIT_TIME;
        }
    }

    LOGV("RVI_delayForReqBCMauth() serviceType:%d result:0x%x delayedCount:%d(microSeconds)", serviceType, result, (BCM_RESP_WAIT_MAX_TIME - count));

    if(result == E_ERROR)
    {
        m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
        m_RemoteServiceApp.sendNackMsgToTSP(serviceType);
        m_RemoteServiceApp.setCurrState(serviceType, RS_READY);
    }

    return result;
}

bool RemoteService_RVI::RemoteService_PrereqCheck(RS_ServiceType remoteServiceType)
{
    RS_PowerMode powerMode = m_RemoteServiceApp.RemoteService_getPowerMode();
    bool remoteStartStatus = m_RemoteServiceApp.RemoteService_getRemoteStartStatus();

    bool result = true;

    LOGV("RemoteService_PrereqCheck() serviceType: %d PowerMode: %d RESstatus:%d\n", remoteServiceType, powerMode, remoteStartStatus);

    if(remoteServiceType == RS_ALARM_RESET)
    {
        RS_AlarmMode alarmMode = m_RemoteServiceApp.getCurrAlarmStatus();

        if(alarmMode != ALARM_ACTIVE)
        {
            LOGV("Alarm not activated.\n");
            m_RemoteServiceApp.setNGTPerrorCode(incorrectState_chosen);
            m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_SUCCESS, STR_FAIL);
            m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_CAUSE, STR_ALOFF_NA);
            return false;
        }
    }

    m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_PWR, (powerMode <= PM_ACCESSORY_1) ? STR_OK : STR_FAIL);
    m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_RES, (remoteStartStatus ? STR_OK : STR_FAIL));

    if(powerMode <= PM_ACCESSORY_1 || (powerMode > PM_ACCESSORY_1 && remoteStartStatus == true))
    {
        //result = true; // JLRTCU-2461 change to check door open status when RDL executed
    }
    else
    {
        m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
        result = false;
    }

    if(remoteServiceType == RS_SECURE_VEHICLE) // JLRTCU-2461
    {
        uint8_t doorStatus = m_RemoteServiceApp.getDoorStatus();
        if(doorStatus != ALL_DOOR_CLOSED)
        {
            LOGV("door not closed doorStatus:0x%x\n", doorStatus);
            m_RemoteServiceApp.setNGTPerrorCode(vehicleNotLocked_chosen);
            result = false;
        }
    }

    if (false == result)
    {
        m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_SUCCESS, STR_FAIL);
        m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_CAUSE, STR_PRE_FAIL);
    }
    return result;
}
