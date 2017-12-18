/**
* \file    RemoteServiceApp_Utils.cpp
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


void RemoteServiceApp::reqDefaultParkClimate()
{
    LOGV("reqDefaultParkClimate()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();

    sp<vifCANContainer> buf = new vifCANContainer();

    values[0] = RPC_REQ_PARK_DEFAULT;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    setCurrParkClimateReqTCU(RPC_REQ_PARK_DEFAULT);

    delete[] values;
}

bool RemoteServiceApp::getParkClimateSystemPresent()
{
    uint8_t parkClimateSystemPresent;
    sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, CCF_FFH_PRESENT);
    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&parkClimateSystemPresent);
        if(parkClimateSystemPresent == FFH_SYS_PRESENT)
            return true;
        else
            return false;
    }
    else
    {
        LOGE("getParkClimateSystemPresent() Can't read config parameter additionalHeater.");
        return false;
    }
}

bool RemoteServiceApp::getParkClimateRemoteEnabled()
{
    uint8_t parkClimateEnabled;
    sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, APP_MODE_RVC);
    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&parkClimateEnabled);
        if(parkClimateEnabled == STATUS_ENABLE)
            return true;
        else
            return false;
    }
    else
    {
        LOGE("getParkClimateRemoteEnabled() Can't read config parameter AppModeRVC.");
        return false;
    }
}

bool RemoteServiceApp::IsVehicleWithFFH()
{
    if((getParkClimateSystemPresent() == true) && (getParkClimateRemoteEnabled() == true) )
    {
        return true;
    }
    return false;
}

void RemoteServiceApp::startRequestService(int32_t ngtpServiceType, uint32_t seconds, uint32_t millis)
{
    if(checkDuplicateReqFromTSP(ngtpServiceType, seconds, millis))
    {
        LOGI("startRequestService() ngtpServiceType:%d duplicate request message", ngtpServiceType);
        // if need: send DSPTack
        return;
    }

    switch(ngtpServiceType)
    {
        case NGTP_ST_RDU: {
            setRDUngtpTimeSec(seconds);
            setRDUngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_UNLOCK_VEHICLE);
        } break;

        case NGTP_ST_RDL: {
            setRDLngtpTimeSec(seconds);
            setRDLngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_SECURE_VEHICLE);
        } break;

        case NGTP_ST_HBLF: {
            setHBLFngtpTimeSec(seconds);
            setHBLFngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_HONK_FLASH);
        } break;

        case NGTP_ST_ALOFF: {
            setALOFFngtpTimeSec(seconds);
            setALOFFngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_ALARM_RESET);
        } break;

        case NGTP_ST_REON: {
            setREONngtpTimeSec(seconds);
            setREONngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_ENGINE_START);
        } break;

        case NGTP_ST_REOFF: {
            setRESstopReason(RES_STOP_TSP);
            setREOFFngtpTimeSec(seconds);
            setREOFFngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_ENGINE_STOP);
        } break;

        case NGTP_ST_RHON: {
            setRHONngtpTimeSec(seconds);
            setRHONngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_PARK_CLIMATE_AUTO);
        } break;

        case NGTP_ST_RHOFF: {
            setRHOFFngtpTimeSec(seconds);
            setRHOFFngtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP(RS_PARK_CLIMATE_STOP);
        } break;

        case NGTP_ST_RISM: {
            if(requestRISM.requestedType == svc::terminateReq) {
                if((getRISMMoveNgtpTimeSec() == seconds) && (getRISMMoveNgtpTimeMil() == millis))   //cancel reuquest timestamp check
                {
                    setRISMCancelNgtpTimeSec(seconds);
                    setRISMCancelNgtpTimeMil(millis);
                    RemoteService_receiveMessageFromTSP(RS_RISM_CANCEL);
                }
                else
                    LOGE("RISM need cancel request timestamp check. seconds:%d millis:%d", seconds, millis);
            } else if(requestRISM.requestedType == svc::startRequest) {
                setRISMMoveNgtpTimeSec(seconds);
                setRISMMoveNgtpTimeMil(millis);
                RemoteService_receiveMessageFromTSP(RS_RISM_MOVE);  //FOLD or UNFOLD
            } else {
                LOGE("RISM request type error:%d", requestRISM.requestedType);
            }
        } break;

        case NGTP_ST_SCHEDULEDWAKUP: {
            setScheduledWakeupNgtpTimeSec(seconds);
            setScheduledWakeupNgtpTimeMil(millis);
            RemoteService_receiveMessageFromTSP_ScheduledWakeup(RS_SCHEDULED_WAKEUP);
        } break;

        default:
            break;
    }
}

bool RemoteServiceApp::checkDuplicateReqFromTSP(int32_t ngtpServiceType, uint32_t seconds, uint32_t millis)
{
    bool bDuplicateMsg = false;
    //bool bAlreadyActive = checkServiceState(ngtpServiceType);

    switch(ngtpServiceType)
    {
        case NGTP_ST_RDU: {
            if(/*(bAlreadyActive) &&*/ (getRDUngtpTimeSec() == seconds) && (getRDUngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_UNLOCK_VEHICLE);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_RDL: {
            if(/*(bAlreadyActive) &&*/ (getRDLngtpTimeSec() == seconds) && (getRDLngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_SECURE_VEHICLE);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_HBLF: {
            if(/*(bAlreadyActive) &&*/ (getHBLFngtpTimeSec() == seconds) && (getHBLFngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_HONK_FLASH);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_ALOFF: {
            if(/*(bAlreadyActive) &&*/ (getALOFFngtpTimeSec() == seconds) && (getALOFFngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_ALARM_RESET);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_REON: {
            if(/*(bAlreadyActive) &&*/ (getREONngtpTimeSec() == seconds) && (getREONngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_ENGINE_START);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_REOFF: {
            if(/*(bAlreadyActive) &&*/ (getREOFFngtpTimeSec() == seconds) && (getREOFFngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_ENGINE_STOP);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_RHON: {
            if(/*(bAlreadyActive) &&*/ (getRHONngtpTimeSec() == seconds) && (getRHONngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_PARK_CLIMATE_AUTO);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_RHOFF: {
            if(/*(bAlreadyActive) &&*/ (getRHOFFngtpTimeSec() == seconds) && (getRHOFFngtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_PARK_CLIMATE_STOP);
                bDuplicateMsg = true;
            }
        } break;

        case NGTP_ST_RISM: {
            if((requestRISM.requestedType == svc::terminateReq) &&
                ((getRISMCancelNgtpTimeSec() == seconds) && (getRISMCancelNgtpTimeMil() == millis)))
                    bDuplicateMsg = true;
            else if((requestRISM.requestedType == svc::startRequest) &&
                ((getRISMMoveNgtpTimeSec() == seconds) && (getRISMMoveNgtpTimeMil() == millis)))
                    bDuplicateMsg = true;
            else
                LOGV("RISM reqType:%d seconds:%d millis:%d", requestRISM.requestedType, seconds, millis);
        } break;

        case NGTP_ST_SCHEDULEDWAKUP:{
            if((getScheduledWakeupNgtpTimeSec() == seconds) && (getScheduledWakeupNgtpTimeMil() == millis))
            {
                //sendDSPTackMsgToTSP(RS_SCHEDULED_WAKEUP);
                bDuplicateMsg = true;
            }
        } break;

        default:
            break;
    }
    LOGV("checkDuplicateReqFromTSP() ngtpServiceType:%d bDuplicateMsg:%d", ngtpServiceType, bDuplicateMsg);
    return  bDuplicateMsg;
}

bool RemoteServiceApp::checkServiceState(int32_t ngtpServiceType)
{
    RS_ServiceState serviceState;
    bool bAlreadyActive = false;

    switch(ngtpServiceType)
    {
        case NGTP_ST_RDU: {
            serviceState = getCurrState(RS_UNLOCK_VEHICLE);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        case NGTP_ST_RDL: {
            serviceState = getCurrState(RS_SECURE_VEHICLE);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        case NGTP_ST_HBLF: {
            serviceState = getCurrState(RS_HONK_FLASH);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        case NGTP_ST_ALOFF: {
            serviceState = getCurrState(RS_ALARM_RESET);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        // TODO: check RES/RPC state.
        case NGTP_ST_REON: {
            serviceState = getCurrState(RS_ENGINE_START);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;

            serviceState = getCurrState(RS_FFH_PREREQ);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;

            serviceState = getCurrState(RS_FFH_START);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        case NGTP_ST_REOFF: {
            serviceState = getCurrState(RS_ENGINE_STOP);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;

            serviceState = getCurrState(RS_FFH_STOP);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        case NGTP_ST_RHON: {
            serviceState = getCurrState(RS_PARK_CLIMATE_AUTO);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        case NGTP_ST_RHOFF: {
            serviceState = getCurrState(RS_PARK_CLIMATE_STOP);
            if(serviceState == RS_ACTIVE || serviceState == RS_WAIT_RESPONSE)
                bAlreadyActive = true;
        } break;

        default:
            break;
    }
    return bAlreadyActive;
}

bool RemoteServiceApp::getCrashStatus(RS_ServiceType serviceType)
{
    bool crashStatus = false;
    char* prop = getProperty(STR_CURR_CRASH);
    if(prop != NULL)
    {
        crashStatus = (std::strcmp(prop, STR_FALSE) ? TRUE : FALSE);
    }

    setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_CRASH, (crashStatus ? STR_FAIL : STR_OK));

    if(crashStatus)
    {
        LOGI("[getCrashStatus Crash status ON");
        //svc::crashEvent_chosen => 16
        return false;
    }
    else
    {
        LOGI("getCrashStatus Crash status OFF");
        return true;
    }
}

void RemoteServiceApp::setDoorLatchStatus(uint8_t status)
{
    LOGE("setDoorLatchStatus:0x%x", status);

    bAllDoorLockStatus = ((status == RVI_DOORLATCH_ALLLOCKED) ? true : false);
    bTrunkLockStatus = (((status >> 4) & 1) ? false: true);

    mFRdoorLockStatus = (((status >> 0) & 1) ? RVI_DOOR_UNLOCKED : RVI_DOOR_LOCKED);   //status: 0:locked 1:unlocked
    mFLdoorLockStatus = (((status >> 1) & 1) ? RVI_DOOR_UNLOCKED : RVI_DOOR_LOCKED);   //status: 0:locked 1:unlocked
    mRLdoorLockStatus = (((status >> 2) & 1) ? RVI_DOOR_UNLOCKED : RVI_DOOR_LOCKED);   //status: 0:locked 1:unlocked
    mRRdoorLockStatus = (((status >> 3) & 1) ? RVI_DOOR_UNLOCKED : RVI_DOOR_LOCKED);   //status: 0:locked 1:unlocked
    mTailgateLockStatus = (((status >> 4) & 1) ? RVI_DOOR_UNLOCKED : RVI_DOOR_LOCKED); //status: 0:locked 1:unlocked
}

bool RemoteServiceApp::getSVTstatus(RS_ServiceType serviceType)
{
    bool svtStatus = getSVTactive();

    setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_SVT, (svtStatus ? STR_FAIL : STR_OK));

    if(svtStatus)
    {
        LOGI("getSVTstatus SVT active status");
        //svc::theftEvent_chosen => 15
        return false;
    }
    else
    {
        LOGI("getSVTstatus SVT not active status");
        return true;
    }
}

void RemoteServiceApp::setCurrState(RS_ServiceType service, RS_ServiceState state)
{
    LOGI("setCurrState service:%d state:%d ", service, state);
    mCurrentState[service] = state;
}

void RemoteServiceApp::updateFromConfig_sunroofExist()
{
    uint8_t bodyControl = getUint8fromConfig(CONFIG_BODY_CONTROLLER);
    LOGI("updateFromConfig_sunroofExist bodyControl:0x%x, mask 00000100 -> sunroof", bodyControl);
    bSunroofExist = (bool)(bodyControl & 0x4);
}

void RemoteServiceApp::updateFromConfig_movingWindows() // == Door Zone Module (DZM)
{ // refer RVM SRD
    /*
        Not supported                00
        No Moving Windows        01
        Two Moving Windows      02
        Three Moving Windows    03
        Four Moving Windows      04
        Five Moving Windows       05
    */
#if 0  // TODO: temporary inactive. movingWindows variable is coverity type "unused value", Please check this routine.
    uint8_t movingWindows = getUint8fromConfig(CONFIG_MOVING_WINDOWS);
    LOGI("updateFromConfig_movingWindows:0x%x ", movingWindows);
    mMovingWindows = movingWindows;
#endif

    mMovingWindows = 4; // temp
}

uint8_t RemoteServiceApp::getUint8fromConfig(const char* name)
{
    uint8_t result = 0;
    sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, name);

    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&result);
        LOGV("getUint8fromConfig() param name:%s value:%d", name, result);
    }
    else
    {
        LOGE("getUint8fromConfig() Can't read config parameter: %s", name);
    }
    return result;
}

uint16_t RemoteServiceApp::getUint16fromConfig(const char* name)
{
    uint16_t result = 0;
    sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, name);

    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&result);
        LOGV("getUint16fromConfig() param name:%s value:%d", name, result);
    }
    else
    {
        LOGE("getUint16fromConfig() Can't read config parameter: %s", name);
    }
    return result;
}

uint32_t RemoteServiceApp::getUint32fromConfig(const char* name)
{
    uint32_t result = 0;
    sp<ConfigurationBuffer> buf = m_configManager->get(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, name);

    if(buf != NULL && buf->errorFlag() == E_OK)
    {
        buf->data(&result);
        LOGV("getUint32fromConfig() param name:%s value:%d", name, result);
    }
    else
    {
        LOGE("getUint32fromConfig() Can't read config parameter: %s", name);
    }
    return result;
}

void RemoteServiceApp::setUint16toConfig(const char* name, uint16_t value)
{
    sp<ConfigurationBuffer> pBuffer= new ConfigurationBuffer;
    error_t error_flag;

    pBuffer->setTo(value);
    //pBuffer->setType(4);  //uint16

    error_flag = m_configManager->change_set(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, name, pBuffer);

    if(error_flag == E_OK)
        LOGV("setUint16toConfig saved name:%s value:%d", name, value);
    else
        LOGE("setUint16toConfig can't save name:%s value:%d", name, value);
}

void RemoteServiceApp::initRESremainingTime()
{
    uint16_t remainingInitTime_s = getUint16fromConfig(RES_ENGINE_RUN_MAX);

//    LOGV("initRESremainingTime:%d", remainingInitTime_s);
    setUint16toConfig(RVC_RES_RUNMAX, remainingInitTime_s);
}


void RemoteServiceApp::setVehicleStatusInit()
{
    uint32_t prop_DistanceToEmpty = getPropertyInt(PROP_DISTANCE_TO_EMPTY);
//    LOGV("setVehicleStatusInit prop_DistanceToEmpty:%d", prop_DistanceToEmpty);
    setDistanceToEmpty((uint16_t)prop_DistanceToEmpty);

    uint32_t prop_OdometerMasterValue = getPropertyInt(PROP_ODEMETER_MASTER_VALUE);
//    LOGV("setVehicleStatusInit prop_OdometerMasterValue:%d", prop_OdometerMasterValue);
    setOdometerMasterValue((uint32_t)prop_OdometerMasterValue);

    uint32_t prop_setODODisplayedMiles = getPropertyInt(PROP_ODO_DISPLAY_MILES);
//    LOGV("setVehicleStatusInit prop_setODODisplayedMiles:%d", prop_setODODisplayedMiles);
    setODODisplayedMiles((uint32_t)prop_setODODisplayedMiles);

    uint32_t prop_FuelLevelIndicatedQF = getPropertyInt(PROP_FUEL_LEVEL_INDICATED_QFHS);
//    LOGV("setVehicleStatusInit prop_FuelLevelIndicatedQF:%d", prop_FuelLevelIndicatedQF);
    setFuelLevelIndicatedQF((uint8_t)prop_FuelLevelIndicatedQF);

    uint32_t prop_FuelLevelIndicated = getPropertyInt(PROP_FUEL_LEVEL_INDICATED_HS);
//    LOGV("setVehicleStatusInit prop_FuelLevelIndicated:%d", prop_FuelLevelIndicated);
    setFuelLevelIndicated((uint16_t)prop_FuelLevelIndicated);

    uint32_t prop_FuelGaugePosition = getPropertyInt(PROP_FUEL_GAUGE_POSITION);
//    LOGV("setVehicleStatusInit prop_FuelGaugePosition:%d", prop_FuelGaugePosition);
    setFuelGaugePosition((uint8_t)prop_FuelGaugePosition);

    setDoorLatchStatus(0);
    setSunroofOpenStatus(0);
    setRoofPositionStatus(0);
    updateSecureState();

    initRISM();
}

#if 0
uint8_t RemoteServiceApp::getFuelLevelIndicatedQF()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint8_t result = 0;

    if(getCurrCANData(Signal_FuelLevelIndicatedQFHS_RX, sigdatabuf) == true)
    {
        uint8_t *fuelLevelQF = sigdatabuf->data();

        if(fuelLevelQF[0] == RES_FUEL_QF)
            result = fuelLevelQF[0];
        LOGV("fuelLevelQF:%d", fuelLevelQF[0]);
    }

    return result;
}
#endif
bool RemoteServiceApp::getCurrCANData(uint16_t sigid, sp<Buffer>& sigdatabuf, bool recevedAfterCanWakeup)
{
    bool ret = false;
    uint32_t cansigreqnum = 1;
    int32_t errorType = -1;

    sp<vifCANSigRequest> cansigreq = new vifCANSigRequest(sigid);

    if(recevedAfterCanWakeup == true)
    {
        errorType = m_vifManager->getCANDataFromVifmgr(cansigreqnum, cansigreq, 0x00, vifmgrGetKindOfCANData_e::CurData_InCurPowerCycle);
    }
    else
    {
        errorType = m_vifManager->getCANDataFromVifmgr(cansigreqnum, cansigreq, 0x00);
    }

    if(errorType == E_OK)
    {
        if (cansigreq->getCANSigDataSize() > 0 && cansigreq->getCANSigDataPtr() != NULL)
        {
            sigdatabuf->setSize(cansigreq->getCANSigDataSize());
            sigdatabuf->setTo(cansigreq->getCANSigDataPtr(),  cansigreq->getCANSigDataSize());
            ret = true;
        }
        else
        {
            LOGE("getCurrCANData sigid:%d sigdata size(%d) is 0 or sigdata ptr is NULL", sigid, cansigreq->getCANSigDataSize());
            ret = false;
        }
    }
    else
    {
        LOGE("getCurrCANData sigid:%d\t m_vifManager->getCANDataFromVifmgr errorType: 0x%x ", sigid, errorType);
        ret = false;
    }

    return ret;
}

void RemoteServiceApp::updateWarmupVariable()
{
    LOGI("updateWarmupVariable");

    if(updateFromCan_FuelLevelIndicatedQFHS() == RES_FUEL_QF)
    { // TCU shall ignore the FuelLevelIndicated if the FuelLevelIndicatedQF is not equal to 3.
        updateFromCan_FuelLevelIndicatedHS();
    }
    updateFromCan_FuelGaugePosition();
    updateFromCan_OdometerMasterValue();
    updateFromCan_ODODisplayedMiles();
    updateFromCan_DistanceToEmpty();

    stopRESuponLowFuel();
}

void RemoteServiceApp::updateSecureState()
{
    updateFromCan_doorLatch();
    updateFromCan_WindowPosition();
    if(isSunroofExist())
    {
        updateFromCan_sunroofOpen();
    }
    updateFromCan_roofPositionStatus();
}


/*
void RemoteServiceApp::updateFFHPrereq()
{
    setRESWindowStatus(0);
    setRESVehicleLockStatus(0);
    setRESAlarmStatus(0);
    setRESHazardSwitchStatus(0);
    setRESBrakePedalStatus(0);

    updateFromCan_RESWindowStatus();
    updateFromCan_RESVehicleLockStatus();
    updateFromCan_RESAlarmStatus();
    updateFromCan_RESHazardSwitchStatus();
    updateFromCan_RESBrakePedalStatus();

    LOGI("updateFFHPrereq %d, %d, %d, %d, %d ",
        getRESWindowStatus(),
        getRESVehicleLockStatus(),
        getRESAlarmStatus(),
        getRESHazardSwitchStatus(),
        getRESBrakePedalStatus());
}
*/
void RemoteServiceApp::updateResRelevantValiables()
{
    bool RESAlarmStatus_previous = getRESAlarmStatus();
    bool RESBrakePedalStatus_previous = getRESBrakePedalStatus();
    bool RESEngineRunningStatus_previous = getRESEngineRunningStatus();
    bool RESHazardSwitchStatus_previous = getRESHazardSwitchStatus();
    uint8_t RESSequenceStatus_previous = getRESSequenceStatus();
    bool RESTransmissionStatus_previous = getRESTransmissionStatus();
    bool RESVehicleLockStatus_previous = getRESVehicleLockStatus();
    bool RESVehStationaryStatus_previous = getRESVehStationaryStatus();
    bool RESWindowStatus_previous = getRESWindowStatus();

    updateFromCan_RESAlarmStatus();
    updateFromCan_RESBrakePedalStatus();
    updateFromCan_RESEngineRunningStatus();
    updateFromCan_RESHazardSwitchStatus();
    updateFromCan_RESSequenceStatus();
    updateFromCan_RESTransmissionStatus();
    updateFromCan_RESVehicleLockStatus();
    updateFromCan_RESVehStationaryStatus();
    updateFromCan_RESWindowStatus();

    LOGI("updateResRelevantValiables  %d->%d %d->%d %d->%d %d->%d 0x%x->0x%x %d->%d %d->%d %d->%d %d->%d",
        RESAlarmStatus_previous, getRESAlarmStatus(),
        RESBrakePedalStatus_previous, getRESBrakePedalStatus(),
        RESEngineRunningStatus_previous, getRESEngineRunningStatus(),
        RESHazardSwitchStatus_previous, getRESHazardSwitchStatus(),
        RESSequenceStatus_previous, getRESSequenceStatus(),
        RESTransmissionStatus_previous, getRESTransmissionStatus(),
        RESVehicleLockStatus_previous, getRESVehicleLockStatus(),
        RESVehStationaryStatus_previous, getRESVehStationaryStatus(),
        RESWindowStatus_previous, getRESWindowStatus());
}

bool RemoteServiceApp::updateParkClimateStatusSampling()
{
    int8_t fFHOperatingStatus = updateFromCan_FFHOperatingStatus();
    int8_t parkClimateMode = updateFromCan_parkClimateMode();
    updateFromCan_ParkHeatVentTime();
    updateFromCan_TimerActivationStatus();
    updateFromCan_Timer1Time();
    updateFromCan_Timer2Time();
    LOGI("updateParkClimateStatusSampling %d, %d(%d), %d, %d",
        fFHOperatingStatus, getCurrParkClimateModeTCU(), parkClimateMode,
        getParkHeatVentTime(), getRPCtimerActiveStatus());
    if( (fFHOperatingStatus == -1) || (parkClimateMode == -1) )
    {
        return false;
    }
    return true;
}

bool RemoteServiceApp::updateSeatStatus_L462()
{
    bool statusChanged = false;

    seatMovementStatus     prevStatus_1P = getSeatMovementStatus_1P();
    seatMovementStatus     prevStatus_2D = getSeatMovementStatus_2D();
    seatMovementStatus     prevStatus_2P = getSeatMovementStatus_2P();
    seatMovementStatus     prevStatus_3D = getSeatMovementStatus_3D();
    seatMovementStatus     prevStatus_3P = getSeatMovementStatus_3P();
    seatMovementInhibition prevInhibition_1P = getSeatMovementInhibition_1P();
    seatMovementInhibition prevInhibition_2D = getSeatMovementInhibition_2D();
    seatMovementInhibition prevInhibition_2P = getSeatMovementInhibition_2P();
    seatMovementInhibition prevInhibition_3D = getSeatMovementInhibition_3D();
    seatMovementInhibition prevInhibition_3P = getSeatMovementInhibition_3P();

    updateFromCan_PasSeatPosition1stRow();
    updateFromCan_DrvSeatPosition2ndRow();
    updateFromCan_PasSeatPosition2ndRow();
    updateFromCan_DrvSeatPosition3rdRow();
    updateFromCan_PasSeatPosition3rdRow();
    updateFromCan_PasSeatInhib1stRow();
    updateFromCan_DrvSeatInhib2ndRow();
    updateFromCan_PasSeatInhib2ndRow();
    updateFromCan_DrvSeatInhib3rdRow();
    updateFromCan_PasSeatInhib3rdRow();

    seatMovementStatus     currStatus_1P = getSeatMovementStatus_1P();
    seatMovementStatus     currStatus_2D = getSeatMovementStatus_2D();
    seatMovementStatus     currStatus_2P = getSeatMovementStatus_2P();
    seatMovementStatus     currStatus_3D = getSeatMovementStatus_3D();
    seatMovementStatus     currStatus_3P = getSeatMovementStatus_3P();
    seatMovementInhibition currInhibition_1P = getSeatMovementInhibition_1P();
    seatMovementInhibition currInhibition_2D = getSeatMovementInhibition_2D();
    seatMovementInhibition currInhibition_2P = getSeatMovementInhibition_2P();
    seatMovementInhibition currInhibition_3D = getSeatMovementInhibition_3D();
    seatMovementInhibition currInhibition_3P = getSeatMovementInhibition_3P();

    LOGD("updateSeatStatus status : 1P:%d->%d  2D:%d->%d  2P:%d->%d  3D:%d->%d  3P:%d->%d ",
        prevStatus_1P, currStatus_1P,
        prevStatus_2D, currStatus_2D,
        prevStatus_2P, currStatus_2P,
        prevStatus_3D, currStatus_3D,
        prevStatus_3P, currStatus_3P);
    LOGD("updateSeatStatus inhibit : 1P:%d->%d  2D:%d->%d  2P:%d->%d  3D:%d->%d  3P:%d->%d",
        prevInhibition_1P, currInhibition_1P,
        prevInhibition_2D, currInhibition_2D,
        prevInhibition_2P, currInhibition_2P,
        prevInhibition_3D, currInhibition_3D,
        prevInhibition_3P, currInhibition_3P);

    if(   (prevStatus_2D != currStatus_2D)
        || (prevStatus_2P != currStatus_2P)
        || (prevStatus_3D != currStatus_3D)
        || (prevStatus_3P != currStatus_3P)
        || (prevInhibition_2D != currInhibition_2D)
        || (prevInhibition_2P != currInhibition_2P)
        || (prevInhibition_3D != currInhibition_3D)
        || (prevInhibition_3P != currInhibition_3P) )
    {
        statusChanged = true;
    }
    return statusChanged;
}

bool RemoteServiceApp::updateSeatStatus_L405()
{
    bool statusChanged = false;

    seatMovementStatus     prevStatus_2L        = getSeatMovementStatus_2L();
    seatMovementStatus     prevStatus_2C        = getSeatMovementStatus_2C();
    seatMovementStatus     prevStatus_2R        = getSeatMovementStatus_2R();
    seatMovementStatus     prevStatus_skiHatch  = getSeatMovementStatus_skiHatch();
    seatMovementInhibition prevInhibition_2L    = getSeatMovementInhibition_2L();
    seatMovementInhibition prevInhibition_2C    = getSeatMovementInhibition_2C();
    seatMovementInhibition prevInhibition_2R    = getSeatMovementInhibition_2R();

    updateFromCan_RLSeatMovementStat();
    updateFromCan_RRSeatMovementStat();
    updateFromCan_RCntrSeatMovmentStat();
    updateFromCan_SkiHatchStat();
    updateFromCan_RLSeatInhib2ndRow();
    updateFromCan_RMSeatInhib2ndRow();
    updateFromCan_RRSeatInhib2ndRow();

    seatMovementStatus     currStatus_2L         = getSeatMovementStatus_2L();
    seatMovementStatus     currStatus_2C         = getSeatMovementStatus_2C();
    seatMovementStatus     currStatus_2R         = getSeatMovementStatus_2R();
    seatMovementStatus     currStatus_skiHatch   = getSeatMovementStatus_skiHatch();
    seatMovementInhibition currInhibition_2L     = getSeatMovementInhibition_2L();
    seatMovementInhibition currInhibition_2C     = getSeatMovementInhibition_2C();
    seatMovementInhibition currInhibition_2R     = getSeatMovementInhibition_2R();

    LOGD("updateSeatStatus status : 2L:%d->%d  2C:%d->%d  2R:%d->%d  skiHatch:%d->%d ",
        prevStatus_2L       , currStatus_2L         ,
        prevStatus_2C       , currStatus_2C         ,
        prevStatus_2R       , currStatus_2R         ,
        prevStatus_skiHatch , currStatus_skiHatch   );
    LOGD("updateSeatStatus inhibit : 2L:%d->%d  2C:%d->%d  2R:%d->%d ",
        prevInhibition_2L   , currInhibition_2L     ,
        prevInhibition_2C   , currInhibition_2C     ,
        prevInhibition_2R   , currInhibition_2R     );

    #if 0
    //requirement SRD_TCU_S06_v1_10_011_RVC.pdf for PCR019
    switch(getSeatType())
    {
        case SEATTYPE_6040: //Seat type 1 2nd Row 60-40
            if( (prevStatus_2L    != currStatus_2L)
                ||(prevStatus_2R   != currStatus_2R)
                ||(prevInhibition_2L    != currInhibition_2L )
                ||(prevInhibition_2R    != currInhibition_2R ) )
            {
                statusChanged = true;
            }
            break;
        case SEATTYPE_4040: //Seat type 2 2nd Row 40-40
            if( (prevStatus_2R      != currStatus_2R)
                ||(prevInhibition_2R    != currInhibition_2R ) )
            {
                statusChanged = true;
            }
            break;
        case SEATTYPE_402040: //Seat type 2 2nd Row 40-20-40
            if( (prevStatus_2L         != currStatus_2L        )
                ||(prevStatus_2C        != currStatus_2C        )
                ||(prevStatus_2R        != currStatus_2R        )
                ||(prevStatus_skiHatch  != currStatus_skiHatch  )
                ||(prevInhibition_2L    != currInhibition_2L    )
                ||(prevInhibition_2C    != currInhibition_2C    )
                ||(prevInhibition_2R    != currInhibition_2R    ) )
            {
                statusChanged = true;
            }
            break;
        default:
            break;
    }
    #else
    //requirement SRD_TCU_S06_v1_10_012_RVC.pdf for PCR019b
    //if(isSeatType3() == true)
    {
        if( (prevStatus_2L         != currStatus_2L        )
            ||(prevStatus_2C        != currStatus_2C        )
            ||(prevStatus_2R        != currStatus_2R        )
            ||(prevStatus_skiHatch  != currStatus_skiHatch  )
            ||(prevInhibition_2L    != currInhibition_2L    )
            ||(prevInhibition_2C    != currInhibition_2C    )
            ||(prevInhibition_2R    != currInhibition_2R    ) )
        {
            statusChanged = true;
        }
    }
    #endif
    return statusChanged;
}

#if 0
bool RemoteServiceApp::updateFromCanFrame_resRelevantValiables()
{
    uint8_t canVariant = getCanVariant();
    if( (canVariant != CAN_PMZ) && (canVariant != CAN_MULTICAN) )
    {
        LOGE("CanVariant ERROR !!!!! getCanVariant():%d ", canVariant);
        return false;
    }

    int8_t RESAlarmStatus           = 0xFF;
    int8_t RESBrakePedalStatus      = 0xFF;
    int8_t RESEngineRunningStatus   = 0xFF;
    int8_t RESHazardSwitchStatus    = 0xFF;
    int8_t RESSequenceStatus        = 0xFF;
    int8_t RESTransmissionStatus    = 0xFF;
    int8_t RESVehicleLockStatus     = 0xFF;
    int8_t RESVehStationaryStatus   = 0xFF;
    int8_t RESWindowStatus          = 0xFF;

    sp<vifCANFrameSigData> curframesigdata;

    switch(canVariant)
    {
        case CAN_PMZ:
            curframesigdata = new vifCANFrameSigData(SignalFrame_PMZ_e::SignalFrame_BCM_PMZ_F_RX);
            m_vifManager->getFrameCANDataFromVifmgr(curframesigdata, 0x9999);
            //LOGI("curframesigdata->mFrameId:%d curframesigdata->mNumOfSigInFrame:%d ", curframesigdata->mFrameId, curframesigdata->mNumOfSigInFrame);

            if(curframesigdata->mFrameId != SignalFrame_PMZ_e::SignalFrame_BCM_PMZ_F_RX)
            {
                LOGE("ERROR!!! curframesigdata->mFrameId:%d ", curframesigdata->mFrameId);
                return false;
            }
            break;
        case CAN_MULTICAN:
            curframesigdata = new vifCANFrameSigData(SignalFrame_MULTI_e::SignalFrame_BCM_PT_G_RX);
            m_vifManager->getFrameCANDataFromVifmgr(curframesigdata, 0x9999);
            //LOGI("curframesigdata->mFrameId:%d curframesigdata->mNumOfSigInFrame:%d", curframesigdata->mFrameId, curframesigdata->mNumOfSigInFrame);

            if(curframesigdata->mFrameId != SignalFrame_MULTI_e::SignalFrame_BCM_PT_G_RX)
            {
                LOGE("ERROR!!! curframesigdata->mFrameId:%d ", curframesigdata->mFrameId);
                return false;
            }
            break;
        default:
            LOGE("CanVariant ERROR !!!!! : %d ", canVariant);
            return false;
    }

    if (curframesigdata->mFrameSigData != NULL)
    {
        //char buff[200];
        for (uint16_t i = 0 ; i < curframesigdata->mNumOfSigInFrame ; i++)
        {
            /*
            memset(buff, 0x00, 200);
            sprintf(buff, "vifCANFrameSigData MULTI sigid:[%02d] validvaluesize(%d):",
                curframesigdata->mFrameSigData[i].sigid,
                curframesigdata->mFrameSigData[i].validvaluesize);
                                LOGD("sigid:%d validvaluesize:%d", curframesigdata->mFrameSigData[i].sigid, curframesigdata->mFrameSigData[i].validvaluesize);

            for (uint8_t j = 0; j < curframesigdata->mFrameSigData[i].validvaluesize; j++)
            {
                sprintf(buff, "%s %02X ", buff, curframesigdata->mFrameSigData[i].values[j]);
            }*/
            switch(curframesigdata->mFrameSigData[i].sigid)
            {
                case Signal_RESAlarmStatus_RX:
                    RESAlarmStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESBrakePedalStatus_RX:
                    RESBrakePedalStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESEngineRunningStatus_RX:
                    RESEngineRunningStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESHazardSwitchStatus_RX:
                    RESHazardSwitchStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESTransmissionStatus_RX:
                    RESTransmissionStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESVehicleLockStatus_RX:
                    RESVehicleLockStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESVehStationaryStatus_RX:
                    RESVehStationaryStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESWindowStatus_RX:
                    RESWindowStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                case Signal_RESSequenceStatus_RX:
                    RESSequenceStatus = curframesigdata->mFrameSigData[i].values[0];
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        LOGE("ERROR!!! curframesigdata->mFrameSigData == NULL ");
        return false;
    }

    LOGI("updateFromCanFrame_resRelevantValiables at %d : %d %d %d %d %d %d %d %d %d", RESSequenceStatus,
        RESAlarmStatus,
        RESBrakePedalStatus,
        RESEngineRunningStatus,
        RESHazardSwitchStatus,
        RESSequenceStatus,
        RESTransmissionStatus,
        RESVehicleLockStatus,
        RESVehStationaryStatus,
        RESWindowStatus);

    if(RESWindowStatus != 0xFF)
    {
        setRESWindowStatus(RESWindowStatus);
    }
    if(RESVehicleLockStatus != 0xFF)
    {
        setRESVehicleLockStatus(RESVehicleLockStatus);
    }
    if(RESAlarmStatus != 0xFF)
    {
        setRESAlarmStatus(RESAlarmStatus);
    }
    if(RESHazardSwitchStatus != 0xFF)
    {
        setRESHazardSwitchStatus(RESHazardSwitchStatus);
    }
    if(RESBrakePedalStatus != 0xFF)
    {
        setRESBrakePedalStatus(RESBrakePedalStatus);
    }
    if(RESVehStationaryStatus != 0xFF)
    {
        setRESVehStationaryStatus(RESVehStationaryStatus);
    }
    if(RESTransmissionStatus != 0xFF)
    {
        setRESTransmissionStatus(RESTransmissionStatus);
    }

    return true;
}
#endif
#if 0
bool RemoteServiceApp::updateFromCanFrame_fuelRelevantValiables()
{
    uint8_t canVariant = getCanVariant();
    if( (canVariant != CAN_PMZ) && (canVariant != CAN_MULTICAN) )
    {
        LOGE("CanVariant ERROR !!!!! getCanVariant():%d ", canVariant);
        return false;
    }

    uint8_t fuelLevelIndicatedQF = 0xFF;
    uint16_t fuelLevelIndicated = 0xFF;

    sp<vifCANFrameSigData> curframesigdata;

    switch(canVariant)
    {
        case CAN_PMZ:
            curframesigdata = new vifCANFrameSigData(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_M_RX);
            m_vifManager->getFrameCANDataFromVifmgr(curframesigdata, 0x9999);
            //LOGI("curframesigdata->mFrameId:%d curframesigdata->mNumOfSigInFrame:%d ", curframesigdata->mFrameId, curframesigdata->mNumOfSigInFrame);

            if(curframesigdata->mFrameId != SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_M_RX)
            {
                LOGE("ERROR!!! curframesigdata->mFrameId:%d ", curframesigdata->mFrameId);
                return false;
            }
            break;
        case CAN_MULTICAN:
            curframesigdata = new vifCANFrameSigData(SignalFrame_MULTI_e::SignalFrame_IPC_PT_E_RX);
            m_vifManager->getFrameCANDataFromVifmgr(curframesigdata, 0x9999);
            //LOGI("curframesigdata->mFrameId:%d curframesigdata->mNumOfSigInFrame:%d", curframesigdata->mFrameId, curframesigdata->mNumOfSigInFrame);

            if(curframesigdata->mFrameId != SignalFrame_MULTI_e::SignalFrame_IPC_PT_E_RX)
            {
                LOGE("ERROR!!! curframesigdata->mFrameId:%d ", curframesigdata->mFrameId);
                return false;
            }
            break;
        default:
            LOGE("CanVariant ERROR !!!!! : %d ", canVariant);
            return false;
    }

    if (curframesigdata->mFrameSigData != NULL)
    {
        //char buff[200];
        for (uint16_t i = 0 ; i < curframesigdata->mNumOfSigInFrame ; i++)
        {
            //LOGD("vifCANFrameSigData sigid:%d validvaluesize:%d", curframesigdata->mFrameSigData[i].sigid, curframesigdata->mFrameSigData[i].validvaluesize);
            //memset(buff, 0x00, 200);
            //sprintf(buff, "vifCANFrameSigData MULTI sigid:[%02d] validvaluesize(%d):",
            //    curframesigdata->mFrameSigData[i].sigid,
            //    curframesigdata->mFrameSigData[i].validvaluesize);

            //for (uint8_t j = 0; j < curframesigdata->mFrameSigData[i].validvaluesize; j++)
            //{
                //sprintf(buff, "%s [%d] ", buff, curframesigdata->mFrameSigData[i].values[j]);
                //LOGD("[%d]", curframesigdata->mFrameSigData[i].values[j]);
            //}
            //LOGD("%s", buff);

            switch(curframesigdata->mFrameSigData[i].sigid)
            {
                case Signal_FuelLevelIndicatedHS_RX:
                    fuelLevelIndicated = 0;
                    fuelLevelIndicated |= ((uint16_t)curframesigdata->mFrameSigData[i].values[0] << 8);
                    fuelLevelIndicated |= curframesigdata->mFrameSigData[i].values[1];
                    LOGI("updateFromCanFrame_fuelRelevantValiables fuelLevelIndicated %d", fuelLevelIndicated);
                    break;
                case Signal_FuelLevelIndicatedQFHS_RX:
                    fuelLevelIndicatedQF = curframesigdata->mFrameSigData[i].values[0];
                    LOGI("updateFromCanFrame_fuelRelevantValiables fuelLevelIndicatedQF %d", fuelLevelIndicatedQF);
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        LOGE("ERROR!!! curframesigdata->mFrameSigData == NULL ");
        return false;
    }

    if(fuelLevelIndicatedQF != 0xFF)
    {
        setFuelLevelIndicatedQF(fuelLevelIndicatedQF);
    }
    if(fuelLevelIndicated != 0xFF)
    {
        if(fuelLevelIndicatedQF == RES_FUEL_QF)
        {
            //fuel level: 0-1023 * 0.2(scale)  litres
            uint16_t fuelDeciLevel = (fuelLevelIndicated & RES_FUEL_MASK) * 2; //save deci litres;
            setFuelLevelIndicated(fuelDeciLevel);
        }
    }
    return true;
}
#endif
void RemoteServiceApp::RemoteService_recalcurateResRunMax()
{
    uint16_t remainingTime_s = 0;
    uint32_t remainingTime_ms = 0;

    remainingTime_ms = m_RESmaxRunTimer->getRemainingTimeMs();

    if(remainingTime_ms < 1000)
        remainingTime_s = 0;
    else
        remainingTime_s = (uint16_t)(remainingTime_ms/1000);

    LOGV("RemoteService_recalcurateResRunMax remainingTime_s:%d remainingTime_ms:%d", remainingTime_s, remainingTime_ms);

    setUint16toConfig(RVC_RES_RUNMAX, remainingTime_s);
}

void RemoteServiceApp::RemoteService_askUpdateMessageToRVM(request_to_rvm_type msgType, int8_t param1)
{
    LOGI("askUpdateMessageToRVM msgType:%d param1:%d ", msgType, param1);

    sp<Post> post = Post::obtain(msgType, param1);
    m_appManager->sendPost((appid_t)APP_ID_RVM, post);
}


void RemoteServiceApp::requestSimLock(RS_ServiceType remoteServiceType)
{
    LOGV("requestSimLock data connection state:%d", m_dataService->getDataState());
    switch(remoteServiceType)
    {
        case RS_UNLOCK_VEHICLE:
            if(TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_TSIM)
            {
                if(RvcTsimLockAcquire())
                    setRDUTsimLockState(true);
                else
                    LOGE("Remote Door Unlock: TSIM Lock request fail !");
            }
            break;

        case RS_SECURE_VEHICLE:
            if(!isTimerOn(MyTimer::TIMER_PSIM_LOCK) &&
                (TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM) &&
                (m_dataService->getDataState() == 0))
            {
                if(RvcPsimLockAcquire())
                    startTimerId(MyTimer::TIMER_PSIM_LOCK, false);
                else
                    LOGE("Remote Door Lock: PSIM Lock request fail !");
            }
            break;

        case RS_ENGINE_START:
            if(TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_TSIM)
            {
                if(RvcTsimLockAcquire())
                    setRESTsimLockState(true);
                else
                    LOGE("Remote Engine Start: TSIM Lock request fail !");
            }
            break;

        default:
            break;
    }
}

bool RemoteServiceApp::RvcTsimLockAcquire()
{
    bool result = m_TsimLock->acquire();

    if(result)
        LOGV("RVC TSIM lock acquired");
    else
        LOGV("RVC TSIM lock acquire FAIL");

    return result;
}

bool RemoteServiceApp::RvcTsimLockRelease()
{
    bool result = m_TsimLock->release();

    if(result)
        LOGV("RVC TSIM lock released");
    else
        LOGV("RVC TSIM lock release FAIL");

    return result;
}

bool RemoteServiceApp::RvcPsimLockAcquire()
{
    bool result = m_PsimLock->acquire();

    if(result)
        LOGV("RVC PSIM lock acquired");
    else
        LOGV("RVC PSIM lock acquire FAIL");

    return result;
}

bool RemoteServiceApp::RvcPsimLockRelease()
{
    bool result = m_PsimLock->release();

    if(result)
        LOGV("RVC PSIM lock released");
    else
        LOGV("RVC PSIM lock release FAIL");

    return result;
}

void RemoteServiceApp::RvcKeepPowerAcquire()
{
#ifdef KEEP_POWER
    mKeepPower.acquire();
    ++mKeepPowerCnt;
    LOGV("KeepPower acquire count:%d", mKeepPowerCnt);
    startTimerId(MyTimer::TIMER_KEEPPOWER_RELEASE, true);
#endif
}
void RemoteServiceApp::RvcKeepPowerRelease()
{
#ifdef KEEP_POWER
    if(mKeepPowerCnt > 0)
    {
        mKeepPower.release();
        --mKeepPowerCnt;
        LOGV("KeepPower release count:%d", mKeepPowerCnt);
        if(mKeepPowerCnt == 0)
            stopTimerId(MyTimer::TIMER_KEEPPOWER_RELEASE);
    }
#endif
}

void RemoteServiceApp::RvcAllKeepPowerRelease()
{
#ifdef KEEP_POWER
    while(mKeepPowerCnt > 0)
    {
        mKeepPower.release();
        --mKeepPowerCnt;
        LOGV("KeepPower release count:%d", mKeepPowerCnt);
    }
#endif
}

bool RemoteServiceApp::decodeNGTPSvcData(uint8_t *buffer, uint32_t size)
{
    LOGV("decodeNGTPSvcData start size:%d", size);

    svc::tsdp_svc_Control ctl;
    ctl.setDecodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);

    if(size > 0)
    {
        //decode for validation
        PEREncodedBuffer encodedSvcData;
        encodedSvcData.set_buffer(size, (char*)buffer);
        svc::NGTPServiceData_PDU  outPDU;
        outPDU.decode (ctl, encodedSvcData);

        svc::NGTPServiceData *pServiceData;
        svc::NGTPDownlinkCore *pDownlink;
        pServiceData = outPDU.get_data();

        if(pServiceData == NULL)
        {
            LOGE("ServiceData is NULL, Decoding FAILED");
            return false;
        }

        svc::ServiceMessageType svcMessageType = pServiceData->get_messageType();
        LOGV("RISM decode serviceMessageType:%d", svcMessageType);

        if(svcMessageType == svc::terminateReq)
        {
            requestRISM.requestedType = svc::terminateReq;
            // TODO: RISM.cpp when receive terminateReq, if requestRISM.seat is fold or unfold then send seat move cancel request.
            //if(getReqPasSeat1stRowCmd() == Fold or Unfold) sendCANData(cancelRequest1stRow);
        }
        else if(svcMessageType == svc::startRequest)
        {
            pDownlink = pServiceData->get_serviceDataCoreMessage()->get_downlink();
            if(pDownlink->rismRequest_is_present() == 0)
            {
                LOGE("rismRequest_is_present == 0");
                return false;
            }

            requestRISM.requestedType = svc::startRequest;

            requestRISM.pasSeat1stRowCmd = (seatMovementCommand)pDownlink->get_rismRequest()->get_pasSeat1stRowCmd();
            requestRISM.drvSeat2ndRowCmd = (seatMovementCommand)pDownlink->get_rismRequest()->get_drvSeat2ndRowCmd();
            requestRISM.pasSeat2ndRowCmd = (seatMovementCommand)pDownlink->get_rismRequest()->get_pasSeat2ndRowCmd();
            requestRISM.drvSeat3rdRowCmd = (seatMovementCommand)pDownlink->get_rismRequest()->get_drvSeat3rdRowCmd();
            requestRISM.pasSeat3rdRowCmd = (seatMovementCommand)pDownlink->get_rismRequest()->get_pasSeat3rdRowCmd();

            requestRISM.REM2ndRowLeftMoveReq    = (seatMovementCommand)pDownlink->get_rismRequest()->get_leftSeat2ndRowCmd();
            requestRISM.REM2ndRowCntrMoveReq    = (seatMovementCommand)pDownlink->get_rismRequest()->get_armrest2ndRowCmd();
            requestRISM.REM2ndRowRightMoveReq   = (seatMovementCommand)pDownlink->get_rismRequest()->get_rightSeat2ndRowCmd();
            requestRISM.REMSkiHatchReq          = (seatMovementCommand)pDownlink->get_rismRequest()->get_skihatch2ndRowCmd();

            LOGV("[Decode] RISM request. reqType:%d \
                pas1st:%d drv2nd:%d pas2nd:%d drv3rd:%d pas3rd:%d \
                L:%d M:%d R:%d ski-hatch:%d ",
                requestRISM.requestedType,
                requestRISM.pasSeat1stRowCmd, requestRISM.drvSeat2ndRowCmd, requestRISM.pasSeat2ndRowCmd, requestRISM.drvSeat3rdRowCmd, requestRISM.pasSeat3rdRowCmd,
                requestRISM.REM2ndRowLeftMoveReq, requestRISM.REM2ndRowCntrMoveReq, requestRISM.REM2ndRowRightMoveReq, requestRISM.REMSkiHatchReq );
/*
            if( (requestRISM.pasSeat1stRowCmd == seatMovementCommand_none)
                && (requestRISM.drvSeat2ndRowCmd == seatMovementCommand_none)
                && (requestRISM.pasSeat2ndRowCmd == seatMovementCommand_none)
                && (requestRISM.drvSeat3rdRowCmd == seatMovementCommand_none)
                && (requestRISM.pasSeat3rdRowCmd == seatMovementCommand_none)
                && (requestRISM.REM2ndRowLeftMoveReq == seatMovementCommand_none)
                && (requestRISM.REM2ndRowCntrMoveReq == seatMovementCommand_none)
                && (requestRISM.REMSkiHatchReq == seatMovementCommand_none)
                && (requestRISM.REM2ndRowRightMoveReq == seatMovementCommand_none)
              )
            {
                LOGE("Move request cmd all none");
                return false;
            }
            */
        }
        else
        {
            LOGE("decodeNGTPSvcData invalid svcMessageType:%d", svcMessageType);
            return false;
        }
        return true;
    }
    return false;
}

bool RemoteServiceApp::decodeNGTPSvcData_scheduledWakeup(uint8_t *buffer, uint32_t size)
{
    LOGV("decodeNGTPSvcData_scheduledWakeUp start size:%d", size);

    svc::tsdp_svc_Control ctl;
    ctl.setDecodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);

    if(size > 0)
    {
        //decode for validation
        PEREncodedBuffer encodedSvcData;
        encodedSvcData.set_buffer(size, (char*)buffer);
        svc::NGTPServiceData_PDU  outPDU;
        outPDU.decode (ctl, encodedSvcData);

        svc::NGTPServiceData *pServiceData;
        svc::NGTPDownlinkCore *pDownlink;
        pServiceData = outPDU.get_data();

        if(pServiceData == NULL)
        {
            LOGE("decodeNGTPSvcData_scheduledWakeUp ServiceData is NULL, Decoding FAILED");
            return false;
        }
        else
        {
            svc::ServiceMessageType svcMessageType = pServiceData->get_messageType();
            LOGV("decodeNGTPSvcData_scheduledWakeUp svcMessageType = %d ", svcMessageType);
            if(svcMessageType == svc::startRequest)
            {
                pDownlink = pServiceData->get_serviceDataCoreMessage()->get_downlink();
                if(pDownlink == NULL)
                {
                    LOGV("pDownlink is null");
                    return false;
                }

                if(pDownlink->remoteOperations_is_present())
                {
                    LOGV("decodeNGTPSvcData_scheduledWakeUp remoteOperations_is_present");

                    svc::RemoteOperationRequestList *pRemoteOperations;
                    pRemoteOperations = pDownlink->get_remoteOperations();
                    if(pRemoteOperations == NULL)
                    {
                        LOGV("pRemoteOperations is null");
                        return false;
                    }

                    svc::RemoteOperationTrigger executionTrigger;
                    executionTrigger = pRemoteOperations->get_executionTrigger();

                    svc::RemoteOperationName *m_pName;    // Name
                    m_pName = pRemoteOperations->get_name();
                    LOGV("      Name = %d", *m_pName);

                    if(*m_pName == svc::rotNameEnd)
                    {
                        mScheduledWakeUpSecond = 0;
                        LOGV(" !!! STOP ");
                        return true;
                    }
                    else if (*m_pName == svc::rotNameStart)
                    {
                        svc::__seq2 *pPeriodic = executionTrigger.get_periodic();
                        if(pPeriodic == NULL)
                        {
                            LOGV("pPeriodic is null");
                            return false;
                        }
                        svc::TimeStamp startAt = pPeriodic->get_startAt();
                        if(startAt == NULL)
                        {
                            LOGV("startAt is null");
                            return false;
                        }
                        mScheduledWakeUpSecond = startAt.get_seconds();
                        LOGV(" !!! mScheduledWakeUpSecond:%ld startAt.get_seconds():%ld", mScheduledWakeUpSecond, startAt.get_seconds());
                        return true;
                    }
                    else
                    {
                        LOGE("m_pName :%d FALSE", m_pName);
                        return false;
                    }
                }
                else
                {
                    LOGE("pDownlink->remoteOperations_is_present() FALSE");
                    return false;
                }
            }
            else
            {
                LOGE("decodeNGTPSvcData_scheduledWakeUp invalid svcMessageType:%d", svcMessageType);
                return false;
            }
         }
    }
    return false;
}


error_t RemoteServiceApp::setScheduledWakeUpSecond(uint32_t second)
{
    uint32_t scheduledTime;
    error_t error;

    scheduledTime = second + 946684800 ; // gap for 30 years

    sp<IAlarmManagerService> alarmManager = interface_cast<IAlarmManagerService>
                (defaultServiceManager()->getService(String16(AlarmManagerService::getServiceName())));

    if( second == 0 )
    {
        error = alarmManager->cancelScheduledWakeup(0, 0);
        LOGE("cancelScheduledWakeup  error:0x%x ", error);
    }
    else
    {
        error = alarmManager->setScheduledWakeup((int64_t)scheduledTime, 0);
        LOGE("setScheduledWakeUpSecond  second:%ld scheduledTime:%ld error:0x%x ", second, scheduledTime, error);
    }

    return error;
}

void RemoteServiceApp::initRISM()
{
    sp<Buffer> sigdata = new Buffer();

    requestRISM.requestedType = -1;

    //Seat move command
    requestRISM.pasSeat1stRowCmd = seatMovementCommand_none;
    requestRISM.drvSeat2ndRowCmd = seatMovementCommand_none;
    requestRISM.pasSeat2ndRowCmd = seatMovementCommand_none;
    requestRISM.drvSeat3rdRowCmd = seatMovementCommand_none;
    requestRISM.pasSeat3rdRowCmd = seatMovementCommand_none;
    statusRISM.pasSeat1stRowInhib = seatMovementInhibition_available;
    statusRISM.drvSeat2ndRowInhib = seatMovementInhibition_available;
    statusRISM.pasSeat2ndRowInhib = seatMovementInhibition_available;
    statusRISM.drvSeat3rdRowInhib = seatMovementInhibition_available;
    statusRISM.pasSeat3rdRowInhib = seatMovementInhibition_available;
    statusRISM.pasSeat1stRowState = seatMovementStatus_default;
    statusRISM.drvSeat2ndRowState = seatMovementStatus_default;
    statusRISM.pasSeat2ndRowState = seatMovementStatus_default;
    statusRISM.drvSeat3rdRowState = seatMovementStatus_default;
    statusRISM.pasSeat3rdRowState = seatMovementStatus_default;

    requestRISM.REM2ndRowLeftMoveReq    = seatMovementCommand_none;
    requestRISM.REM2ndRowCntrMoveReq    = seatMovementCommand_none;
    requestRISM.REM2ndRowRightMoveReq   = seatMovementCommand_none;
    requestRISM.REMSkiHatchReq          = seatMovementCommand_none;
    statusRISM.RLSeatMovementStat       = seatMovementStatus_default;
    statusRISM.RCntrSeatMovementStat    = seatMovementStatus_default;
    statusRISM.RRSeatMovementStat       = seatMovementStatus_default;
    statusRISM.REMSkiHatchStat          = seatMovementStatus_default;
    statusRISM.RLSeatInhib2ndRow        = seatMovementInhibition_available;
    statusRISM.RMSeatInhib2ndRow        = seatMovementInhibition_available;
    statusRISM.RRSeatInhib2ndRow        = seatMovementInhibition_available;

    setAppModeRISM(getUint8fromConfig(APP_MODE_RISM));
    setConvenienceFold(getUint8fromConfig(CONFIG_CONVENIENCE_FOLD));
    setVehicleType(getUint8fromConfig(CONFIG_VEHICLE_TYPE));
    setIntelligentSeatFold(getUint8fromConfig(CONFIG_INTELLIGENT_SEAT_FOLD));

    uint8_t vehicleType = getVehicleType();
    switch(vehicleType)
    {
        case VEHICLETYPE_L462:
            updateFromCan_PasSeatPosition1stRow();
            if(updateFromCan_DrvSeatPosition2ndRow() == -1)
            {
                statusRISM.drvSeat2ndRowState = (seatMovementStatus)getProperty_seats(Signal_DrvSeatPosition2ndRow_RX);
            }
            if(updateFromCan_PasSeatPosition2ndRow() == -1)
            {
                statusRISM.pasSeat2ndRowState = (seatMovementStatus)getProperty_seats(Signal_PasSeatPosition2ndRow_RX);
            }
            if(updateFromCan_DrvSeatPosition3rdRow() == -1)
            {
                statusRISM.drvSeat3rdRowState = (seatMovementStatus)getProperty_seats(Signal_DrvSeatPosition3rdRow_RX);
            }
            if(updateFromCan_PasSeatPosition3rdRow() == -1)
            {
                statusRISM.pasSeat3rdRowState = (seatMovementStatus)getProperty_seats(Signal_PasSeatPosition3rdRow_RX);
            }
            updateFromCan_PasSeatInhib1stRow();
            if(updateFromCan_DrvSeatInhib2ndRow() == -1)
            {
                statusRISM.drvSeat2ndRowInhib = (seatMovementInhibition)getProperty_seats(Signal_DrvSeatInhib2ndRow_RX);
            }
            if(updateFromCan_PasSeatInhib2ndRow() == -1)
            {
                statusRISM.pasSeat2ndRowInhib = (seatMovementInhibition)getProperty_seats(Signal_PasSeatInhib2ndRow_RX);
            }
            if(updateFromCan_DrvSeatInhib3rdRow() == -1)
            {
                statusRISM.drvSeat3rdRowInhib = (seatMovementInhibition)getProperty_seats(Signal_DrvSeatInhib3rdRow_RX);
            }
            if(updateFromCan_PasSeatInhib3rdRow() == -1)
            {
                statusRISM.pasSeat3rdRowInhib = (seatMovementInhibition)getProperty_seats(Signal_PasSeatInhib3rdRow_RX);
            }
            LOGI("seat inhibit init. Drv2nd:%d Pas2nd:%d Drv3rd:%d Pas3rd:%d", statusRISM.drvSeat2ndRowInhib, statusRISM.pasSeat2ndRowInhib, statusRISM.drvSeat3rdRowInhib, statusRISM.pasSeat3rdRowInhib);
            LOGI("seat position init. Drv2nd:%d Pas2nd:%d Drv3rd:%d Pas3rd:%d", statusRISM.drvSeat2ndRowState, statusRISM.pasSeat2ndRowState, statusRISM.drvSeat3rdRowState, statusRISM.pasSeat3rdRowState);
            break;
        case VEHICLETYPE_L405:
            if(updateFromCan_RLSeatMovementStat() == -1)
            {
                statusRISM.RLSeatMovementStat = (seatMovementStatus)getProperty_seats(Signal_RLSeatMovementStat_RX);
            }
            if(updateFromCan_RCntrSeatMovmentStat() == -1)
            {
                statusRISM.RCntrSeatMovementStat = (seatMovementStatus)getProperty_seats(Signal_RCntrSeatMovmentStat_RX);
            }
            if(updateFromCan_RRSeatMovementStat() == -1)
            {
                statusRISM.RRSeatMovementStat = (seatMovementStatus)getProperty_seats(Signal_RRSeatMovementStat_RX);
            }
            if(updateFromCan_SkiHatchStat() == -1)
            {
                statusRISM.REMSkiHatchStat = (seatMovementStatus)getProperty_seats(Signal_SkiHatchStat_RX);
            }
            if(updateFromCan_RLSeatInhib2ndRow() == -1)
            {
                statusRISM.RLSeatInhib2ndRow = (seatMovementInhibition)getProperty_seats(Signal_RLSeatInhib2ndRow_RX);
            }
            if(updateFromCan_RMSeatInhib2ndRow() == -1)
            {
                statusRISM.RMSeatInhib2ndRow = (seatMovementInhibition)getProperty_seats(Signal_RMSeatInhib2ndRow_RX);
            }
            if(updateFromCan_RRSeatInhib2ndRow() == -1)
            {
                statusRISM.RRSeatInhib2ndRow = (seatMovementInhibition)getProperty_seats(Signal_RRSeatInhib2ndRow_RX);
            }
            LOGI("seat inhibit init. L:%d M:%d R:%d ", statusRISM.RLSeatInhib2ndRow, statusRISM.RMSeatInhib2ndRow, statusRISM.RRSeatInhib2ndRow);
            LOGI("seat position init. L:%d C:%d R:%d skiHatch:%d", statusRISM.RLSeatMovementStat, statusRISM.RCntrSeatMovementStat, statusRISM.RRSeatMovementStat, statusRISM.REMSkiHatchStat);
            break;
        default:
            break;
    }
}

void RemoteServiceApp::resetSeatMoveReq()
{
    /*
    SRD_ReqRISM0450_V1: Once the response for fold/unfold seat request or stow/deploy armrest
    request is sent by TCU to TSP in RISM_RESPONSE NGTP message, the corresponding CAN
    signals used for indicating the request should be reset to 0.
    */
    uint8_t vehicleType = getVehicleType();
    switch(vehicleType)
    {
        case VEHICLETYPE_L462:
            resetSeatMoveReq_L462();
            return; //break;;
        case VEHICLETYPE_L405:
            resetSeatMoveReq_L405();
            return; //break;
        default:
            break;
    }
}
void RemoteServiceApp::resetSeatMoveReq_L462()
{
    LOGV("resetSeatMoveReq reqType:%d cmd - 2D:%d 2P:%d 3D:%d 3P:%d req - 2D:%d 2P:%d 3D:%d 3P:%d",
        requestRISM.requestedType,
        requestRISM.drvSeat2ndRowCmd, requestRISM.pasSeat2ndRowCmd, requestRISM.drvSeat3rdRowCmd, requestRISM.pasSeat3rdRowCmd,
        getSeatMovementRequest_2D(), getSeatMovementRequest_2P(), getSeatMovementRequest_3D(), getSeatMovementRequest_3P());

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(CAN_VALUES_MIN_BUF_SIZE)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    seatMovementCommand move = seatMovementCommand_none;

    values[0] = 0;

    move = getSeatMovementRequest_2D();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_RemRDrvFoldReq2ndRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRDrvFoldReq2ndRow_TX), values);
        setSeatMovementRequest_2D(seatMovementCommand_none);
    }

    move = getSeatMovementRequest_2P();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_RemRPasFoldReq2ndRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRPasFoldReq2ndRow_TX), values);
        setSeatMovementRequest_2P(seatMovementCommand_none);
    }

    move = getSeatMovementRequest_3D();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_RemRDrvFoldReq3rdRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRDrvFoldReq3rdRow_TX), values);
        setSeatMovementRequest_3D(seatMovementCommand_none);
    }

    move = getSeatMovementRequest_3P();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_RemRPasFoldReq3rdRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRPasFoldReq3rdRow_TX), values);
        setSeatMovementRequest_3P(seatMovementCommand_none);
    }

    LOGV("buf->NumofSignals:%d", buf->NumofSignals);
    if(buf->NumofSignals > 0)
        RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    else
        LOGE("No request available seats!");

    delete[] values;
}

void RemoteServiceApp::resetSeatMoveReq_L405()
{
    LOGV("resetSeatMoveReq reqType:%d cmd - 2L:%d C:%d 2R:%d skiHatch:%d req - 2L:%d C:%d 2R:%d skiHatch:%d",
        requestRISM.requestedType,
        requestRISM.REM2ndRowLeftMoveReq, requestRISM.REM2ndRowCntrMoveReq, requestRISM.REM2ndRowRightMoveReq, requestRISM.REMSkiHatchReq,
        getSeatMovementRequest_2L(), getSeatMovementRequest_2C(), getSeatMovementRequest_2R(), getSeatMovementRequest_skiHatch());

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(CAN_VALUES_MIN_BUF_SIZE)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    seatMovementCommand move = seatMovementCommand_none;

    values[0] = 0;

    move = getSeatMovementRequest_2L();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
        setSeatMovementRequest_2L(seatMovementCommand_none);
    }

    move = getSeatMovementRequest_2C();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_REM2ndRowCntrMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowCntrMoveReq_TX), values);
        setSeatMovementRequest_2C(seatMovementCommand_none);
    }

    move = getSeatMovementRequest_2R();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
        setSeatMovementRequest_2R(seatMovementCommand_none);
    }

    move = getSeatMovementRequest_skiHatch();
    if(move != seatMovementCommand_none)
    {
        buf->setData(Signal_REMSkiHatchReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REMSkiHatchReq_TX), values);
        setSeatMovementRequest_skiHatch(seatMovementCommand_none);
    }

    LOGV("buf->NumofSignals:%d", buf->NumofSignals);
    if(buf->NumofSignals > 0)
    {
        RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    }

    delete[] values;
}

bool RemoteServiceApp::checkSeatMovedAsRequested_L462()
{
    seatMovementCommand moveReq;
    seatMovementStatus seatPosition;

    moveReq = getSeatMovementCommand_2D();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_2D() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_2D();
        if((moveReq == seatMovementCommand_fold) && (seatPosition != seatMovementStatus_seatFullyFolded))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_unfold) && (seatPosition != seatMovementStatus_seatFullyUnfolded))
        {
            return false;
        }
    }
    moveReq = getSeatMovementCommand_2P();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_2P() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_2P();
        if((moveReq == seatMovementCommand_fold) && (seatPosition != seatMovementStatus_seatFullyFolded))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_unfold) && (seatPosition != seatMovementStatus_seatFullyUnfolded))
        {
            return false;
        }
    }
    moveReq = getSeatMovementCommand_3D();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_3D() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_3D();
        if((moveReq == seatMovementCommand_fold) && (seatPosition != seatMovementStatus_seatFullyFolded))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_unfold) && (seatPosition != seatMovementStatus_seatFullyUnfolded))
        {
            return false;
        }
    }
    moveReq = getSeatMovementCommand_3P();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_3P() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_3P();
        if((moveReq == seatMovementCommand_fold) && (seatPosition != seatMovementStatus_seatFullyFolded))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_unfold) && (seatPosition != seatMovementStatus_seatFullyUnfolded))
        {
            return false;
        }
    }
    return true;
}

bool RemoteServiceApp::checkSeatMovedAsRequested_L405()
{
    seatMovementCommand moveReq;
    seatMovementStatus seatPosition;

    moveReq = getSeatMovementCommand_2L();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_2L() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_2L();
        if((moveReq == seatMovementCommand_fold) && (seatPosition != seatMovementStatus_seatFullyFolded))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_unfold) && (seatPosition != seatMovementStatus_seatFullyUnfolded))
        {
            return false;
        }
    }
    moveReq = getSeatMovementCommand_2R();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_2R() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_2R();
        if((moveReq == seatMovementCommand_fold) && (seatPosition != seatMovementStatus_seatFullyFolded))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_unfold) && (seatPosition != seatMovementStatus_seatFullyUnfolded))
        {
            return false;
        }
    }
    moveReq = getSeatMovementCommand_2C();
    if(moveReq != seatMovementCommand_none)
    {
        if(getSeatMovementInhibition_2C() == seatMovementInhibition_inhibited)
        {
            return false;
        }
        seatPosition = getSeatMovementStatus_2C();
        if((moveReq == seatMovementCommand_deploy) && (seatPosition != seatMovementStatus_deployedDownPosition))
        {
            return false;
        }
        if((moveReq == seatMovementCommand_stow) && (seatPosition != seatMovementStatus_stowedUpPosition))
        {
            return false;
        }
    }
    moveReq = getSeatMovementCommand_skiHatch();
    if(moveReq != seatMovementCommand_none)
    {
        seatPosition = getSeatMovementStatus_skiHatch();
        if((moveReq == seatMovementCommand_drop) && (seatPosition != seatMovementStatus_open))
        {
            return false;
        }
    }
    return true;
}

void RemoteServiceApp::foldRequestBySldd(int32_t seatPosition)
{
    LOGV("foldRequestBySldd requested seatPosition:%d", seatPosition);

    if(seatPosition <= 0 || seatPosition > RISM_REQ_ALL_SEAT)
    {
        LOGE("Seat fold sldd command fail. Not available seatPosition [%d]", seatPosition);
        return;
    }

    requestRISM.requestedType = svc::startRequest;
    uint8_t vehicleType = getVehicleType();
    switch(vehicleType)
    {
        case VEHICLETYPE_L462:
            LOGV("foldRequestBySldd L462");

            requestRISM.drvSeat2ndRowCmd = seatMovementCommand_none;
            requestRISM.pasSeat2ndRowCmd = seatMovementCommand_none;
            requestRISM.drvSeat3rdRowCmd = seatMovementCommand_none;
            requestRISM.pasSeat3rdRowCmd = seatMovementCommand_none;

            if(seatPosition & 0x01)
                requestRISM.drvSeat2ndRowCmd = seatMovementCommand_fold;

            if(seatPosition & (0x01 << 1))
                requestRISM.pasSeat2ndRowCmd = seatMovementCommand_fold;

            if(seatPosition & (0x01 << 2))
                requestRISM.drvSeat3rdRowCmd = seatMovementCommand_fold;

            if(seatPosition & (0x01 << 3))
                requestRISM.pasSeat3rdRowCmd = seatMovementCommand_fold;

            break;
        case VEHICLETYPE_L405:
            LOGV("foldRequestBySldd L405, IntelligentSeatFold:%d ", getIntelligentSeatFold());

            requestRISM.REM2ndRowLeftMoveReq = seatMovementCommand_none;
            requestRISM.REM2ndRowCntrMoveReq = seatMovementCommand_none;
            requestRISM.REM2ndRowRightMoveReq = seatMovementCommand_none;
            requestRISM.REMSkiHatchReq = seatMovementCommand_none;

            if(seatPosition & 0x01)
                requestRISM.REM2ndRowLeftMoveReq = seatMovementCommand_fold;

            if(seatPosition & (0x01 << 1))
                requestRISM.REM2ndRowCntrMoveReq = seatMovementCommand_fold;

            if(seatPosition & (0x01 << 2))
                requestRISM.REM2ndRowRightMoveReq = seatMovementCommand_fold;

            if(seatPosition & (0x01 << 3))
                requestRISM.REMSkiHatchReq = seatMovementCommand_fold;
            break;
        default:
            LOGE("Seat fold sldd command fail. Not available vehicleType [%d]", vehicleType);
            return;
    }

    setSLDDcmdActiveStatus(true);

    setRISMMoveNgtpTimeSec(0);
    setRISMMoveNgtpTimeMil(0);
    RemoteService_receiveMessageFromTSP(RS_RISM_MOVE);
}

void RemoteServiceApp::unfoldRequestBySldd(int32_t seatPosition)
{
    LOGV("unfoldRequestBySldd requested seatPosition:%d", seatPosition);

    if(seatPosition <= 0 || seatPosition > RISM_REQ_ALL_SEAT)
    {
        LOGE("Seat unfold sldd command fail. Not available seatPosition [%d]", seatPosition);
        return;
    }

    requestRISM.requestedType = svc::startRequest;
    uint8_t vehicleType = getVehicleType();
    switch(vehicleType)
    {
        case VEHICLETYPE_L462:
            LOGV("foldRequestBySldd L462");

            requestRISM.drvSeat2ndRowCmd = seatMovementCommand_none;
            requestRISM.pasSeat2ndRowCmd = seatMovementCommand_none;
            requestRISM.drvSeat3rdRowCmd = seatMovementCommand_none;
            requestRISM.pasSeat3rdRowCmd = seatMovementCommand_none;

            if(seatPosition & 0x01)
                requestRISM.drvSeat2ndRowCmd = seatMovementCommand_unfold;

            if(seatPosition & (0x01 << 1))
                requestRISM.pasSeat2ndRowCmd = seatMovementCommand_unfold;

            if(seatPosition & (0x01 << 2))
                requestRISM.drvSeat3rdRowCmd = seatMovementCommand_unfold;

            if(seatPosition & (0x01 << 3))
                requestRISM.pasSeat3rdRowCmd = seatMovementCommand_unfold;

            break;
        case VEHICLETYPE_L405:
            LOGV("foldRequestBySldd L405, IntelligentSeatFold:%d ", getIntelligentSeatFold());

            requestRISM.REM2ndRowLeftMoveReq = seatMovementCommand_none;
            requestRISM.REM2ndRowCntrMoveReq = seatMovementCommand_none;
            requestRISM.REM2ndRowRightMoveReq = seatMovementCommand_none;
            requestRISM.REMSkiHatchReq = seatMovementCommand_none;

            if(seatPosition & 0x01)
                requestRISM.REM2ndRowLeftMoveReq = seatMovementCommand_unfold;

            if(seatPosition & (0x01 << 1))
                requestRISM.REM2ndRowCntrMoveReq = seatMovementCommand_unfold;

            if(seatPosition & (0x01 << 2))
                requestRISM.REM2ndRowRightMoveReq = seatMovementCommand_unfold;

            if(seatPosition & (0x01 << 3))
                requestRISM.REMSkiHatchReq = seatMovementCommand_unfold;
            break;
        default:
            LOGE("Seat fold sldd command fail. Not available vehicleType [%d]", vehicleType);
            return;
    }

    setSLDDcmdActiveStatus(true);

    setRISMMoveNgtpTimeSec(0);
    setRISMMoveNgtpTimeMil(0);
    RemoteService_receiveMessageFromTSP(RS_RISM_MOVE);
}

void RemoteServiceApp::makeLog()
{
    LOGI("makeLog start");
    uint32_t ngtpTime = get_NGTP_time();

    uint8_t logIndex = m_remoteServiceLog->increaseLogIndex();

    m_remoteServiceLog->initLogData(logIndex);
/*
    if(updateFromCanFrame_resRelevantValiables() == false)
    {
        updateFromCan_RESWindowStatus();
        updateFromCan_RESVehicleLockStatus();
        updateFromCan_RESAlarmStatus();
        updateFromCan_RESHazardSwitchStatus();
        updateFromCan_RESBrakePedalStatus();
        updateFromCan_RESVehStationaryStatus();
        updateFromCan_RESTransmissionStatus();
        LOGI("update RES signals %d, %d, %d, %d, %d, %d, %d ",
            getRESWindowStatus(),
            getRESVehicleLockStatus(),
            getRESAlarmStatus(),
            getRESHazardSwitchStatus(),
            getRESBrakePedalStatus(),
            getRESVehStationaryStatus(),
            getRESTransmissionStatus());
    }
    */
    assembleLog(ngtpTime, logIndex);

//    LOGI("makeLog end logIndex:%d ngtpTime:%ld %d, %d", logIndex, ngtpTime,getLogResRunTimer(),getLogFfhRunTimer());
    m_remoteServiceLog->writeLogToFile();
}
void RemoteServiceApp::assembleLog(uint32_t ngtpTime,uint8_t logIndex)
{
    m_remoteServiceLog->set_routineResult(0, logIndex);
    m_remoteServiceLog->set_eventCreateTime(ngtpTime, logIndex);
    LOGI("%d, %d ", logIndex, ngtpTime);

    m_remoteServiceLog->set_serviceType(getLogserviceType(), logIndex); //Engine Start
    m_remoteServiceLog->set_successStatus(getLogSuccess(), logIndex);
    m_remoteServiceLog->set_simpleStatus(LOG_NOT_AVAILABLE, logIndex);
    m_remoteServiceLog->set_heaterStatus(getFFHOperatingStatus(), logIndex);
    m_remoteServiceLog->set_windowStatus((uint8_t)getRESWindowStatus(), logIndex);
    m_remoteServiceLog->set_lockStatus((uint8_t)getRESVehicleLockStatus(), logIndex);
    m_remoteServiceLog->set_alarmStatus((uint8_t)getRESAlarmStatus(), logIndex);
    m_remoteServiceLog->set_hazardStatus((uint8_t)getRESHazardSwitchStatus(), logIndex);
    LOGI("%d, %d, %d, %d, %d, %d, %d, %d   ",
        getLogserviceType(),
        getLogSuccess(),
        LOG_NOT_AVAILABLE,
        getFFHOperatingStatus(),
        (uint8_t)getRESWindowStatus(),
        (uint8_t)getRESVehicleLockStatus(),
        (uint8_t)getRESAlarmStatus(),
        (uint8_t)getRESHazardSwitchStatus()
        );

    m_remoteServiceLog->set_brakePedalStatus((uint8_t)getRESBrakePedalStatus(), logIndex);
    m_remoteServiceLog->set_stationaryStatus((uint8_t)getRESVehStationaryStatus(), logIndex);
    m_remoteServiceLog->set_transmissionStatus((uint8_t)getRESTransmissionStatus(), logIndex);
    m_remoteServiceLog->set_currCrashStatus(getLogCrashStatusCurrentIgnitionCycle(), logIndex);
    m_remoteServiceLog->set_prevCrashStatus(getLogCrashStatusPreviousIgnitionCycle(), logIndex);
    m_remoteServiceLog->set_fuelLevel(getLogfuelLevel(), logIndex);
    m_remoteServiceLog->set_maxRunTime(getLogMaxRunTimeReached(), logIndex);
    m_remoteServiceLog->set_svtStatus((uint8_t)getSVTactive(), logIndex);
    LOGI("%d, %d, %d, %d, %d, %d, %d, %d   ",
        (uint8_t)getRESBrakePedalStatus(),
        (uint8_t)getRESVehStationaryStatus(),
        (uint8_t)getRESTransmissionStatus(),
        getLogCrashStatusCurrentIgnitionCycle(),
        getLogCrashStatusPreviousIgnitionCycle(),
        getLogfuelLevel(),
        getLogMaxRunTimeReached(),
        (uint8_t)getSVTactive()
        );

    m_remoteServiceLog->set_transitionNormal(getLogTransistionFromREStoNormal(), logIndex);
    m_remoteServiceLog->set_prerequisiteStatus(getLogPrerequisiteCheckRequestStatus(), logIndex);
    m_remoteServiceLog->set_powerMode(getLogPowerModeWhenRequestReceived(), logIndex);
    m_remoteServiceLog->set_currResSequence(getRESSequenceStatus(), logIndex);
    m_remoteServiceLog->set_prevResSequence(getLogRESSequenceStatusPrevious(), logIndex);
    m_remoteServiceLog->set_resRunTimer(getLogResRunTimer(), logIndex);
    m_remoteServiceLog->set_ffhRunTimer(getLogFfhRunTimer(), logIndex);
    LOGI("%d, %d, %d, %d, %d, %d, %d",
        getLogTransistionFromREStoNormal(),
        getLogPrerequisiteCheckRequestStatus(),
        getCurrRemoteStartStatus(),
        getLogPrerequisiteCheckRequestStatus(),
        getLogPowerModeWhenRequestReceived(),
        getLogResRunTimer(),
        getLogFfhRunTimer()
        );

    m_remoteServiceLog->set_ffhStatusWhenFFHStopped(getLogffhStatusWhenFFHStopped(), logIndex);
    LOGI("%d ", getLogffhStatusWhenFFHStopped());
}

uint8_t RemoteServiceApp::getLogSuccess()
{
    switch(getRESSequenceStatus())
    {
        case 0x07:  //Engine Running
            return 0x01; //True
        case 0x09:  //any of the BCM prerequisite is not satisified //FALL-THROUGH
        case 0x0A:  //RES exited before engine start                //FALL-THROUGH
        case 0x0B:  //RES exited after engine start                 //FALL-THROUGH
        case 0x0C:  //PCM Timer Expired                             //FALL-THROUGH
        case 0x0D:  //User Pressed Stop Button                      //FALL-THROUGH
        case 0x0E:  //Key Not Found
            return 0x00; //False
        default:
            return LOG_NOT_AVAILABLE;
    }
}

void RemoteServiceApp::setLogffhStatusWhenFFHStopped(uint8_t sigVal)
{
    if(sigVal == FFH_OFF)
    {
        uint8_t FFHOperatingStatus = getFFHOperatingStatus();
        switch(FFHOperatingStatus)
        {
            case FFH_OFF:               //FALL-THROUGH
            case FFH_STARTINGUP:        //FALL-THROUGH
            case FFH_RUNNING:           //FALL-THROUGH
            case FFH_SHUTDOWN:          //FALL-THROUGH
            case FFH_FAIL_LOW_VOLTAGE:  //FALL-THROUGH
            case FFH_FAIL_LOW_FUEL:     //FALL-THROUGH
            case FFH_FAIL_SVC_REQ:      //FALL-THROUGH
            case FFH_FAIL_SYS_FAILURE:  //FALL-THROUGH
            case FFH_FAIL_MAX_TIME:     //FALL-THROUGH
            case FFH_STOP_BY_USER:
                mLogffhStatusWhenFFHStopped = FFHOperatingStatus;
                break;
            case FFH_RESERVED:          //FALL-THROUGH
            case FFH_STATUS_UNKNOWN:    //FALL-THROUGH
            default:
                mLogffhStatusWhenFFHStopped = LOG_NOT_AVAILABLE;
                break;
        }
    }
}

void RemoteServiceApp::setLogserviceType(uint8_t status)
{
    switch(status)
    {
        case RS_UNLOCK_VEHICLE:     mLogserviceType = 0x01; break;
        case RS_SECURE_VEHICLE:     mLogserviceType = 0x02; break;
        case RS_ALARM_RESET:        mLogserviceType = 0x04; break;
        case RS_HONK_FLASH:         mLogserviceType = 0x05; break;
        case RS_ENGINE_START:       mLogserviceType = 0x06; break;
        case RS_ENGINE_STOP:        mLogserviceType = 0x07; break;
        case RS_PARK_CLIMATE_AUTO:  mLogserviceType = 0x08; break;
        case RS_PARK_CLIMATE_STOP:  mLogserviceType = 0x09; break;
        default :                   mLogserviceType = LOG_NOT_AVAILABLE; break;
    }
}

uint8_t RemoteServiceApp::getLogCrashStatusCurrentIgnitionCycle()
{
    char* prop = getProperty(STR_CURR_CRASH);
    if( (prop != NULL) && (std::strcmp(prop, STR_TRUE) == 0) )
    {
        return 0x01;
    }
    return 0x00;
}

uint8_t RemoteServiceApp::getLogCrashStatusPreviousIgnitionCycle()
{
    char* prop = getProperty(STR_PREV_CRASH);
    if( (prop != NULL) && (std::strcmp(prop, STR_TRUE) == 0) )
    {
        return 0x01;
    }
    return 0x00;
}


uint16_t RemoteServiceApp::getLogMaxRunTimeReached()
{
    if(isTimerOn(MyTimer::TIMER_RES_MAX_RUN))
    {
        return 0x00;
    }
    if(getUint16fromConfig(RVC_RES_RUNMAX) != 0)
    {
        return 0x00;
    }
    return 0x01;
}

void RemoteServiceApp::setLogTransistionFromREStoNormal(uint8_t status)
{
    LOGI("setLogTransistionFromREStoNormal %d ", status);
    mLogTransistionFromREStoNormal = status;
}

uint16_t RemoteServiceApp::getLogResRunTimer()
{
    if(isTimerOn(MyTimer::TIMER_RES_MAX_RUN))
    {
        uint32_t remainingTimeMs = m_RESmaxRunTimer->getRemainingTimeMs();
        return (uint16_t)remainingTimeMs/100 ;
    }
    else
    {
        return getUint16fromConfig(RVC_RES_RUNMAX);
    }
}

void RemoteServiceApp::setLogFfhRunTimer(bool bStart)
{
    if(bStart == false)
    {
        uint32_t remainingTimeMs = m_FFHmaxRunTimer->getRemainingTimeMs();
        mLogFfhRunTimer = remainingTimeMs/100 ;
    }
    else
    {
        mLogFfhRunTimer = getUint16fromConfig(CONFIG_RES_FFH_RUN_TIME_MAX);
    }
}
uint16_t RemoteServiceApp::getLogFfhRunTimer()
{
    if(isTimerOn(MyTimer::TIMER_FFH_MAX_RUN))
    {
        uint32_t remainingTimeMs = m_FFHmaxRunTimer->getRemainingTimeMs();
        mLogFfhRunTimer = remainingTimeMs/100 ;
    }
    return mLogFfhRunTimer;
}

void RemoteServiceApp::setCanVariant(uint8_t val)
{
    if( (val != CAN_PMZ) && (val != CAN_MULTICAN) )
    {
        LOGE("setCanVariant ERROR !!!!! val:%d ", val);
        return;
    }
    if( val == mCanVariant )
    {
        return;
    }

    mCanVariant = val;

    uint8_t buffer[10];
    sprintf((char*) buffer, "%d", val);
    setProperty(PROP_CAN_VARIANT, (const char *) buffer);
    LOGI("setCanVariant !!!!! val:%d ", val);
}
uint8_t RemoteServiceApp::getCanVariant()
{
    if( (mCanVariant == CAN_PMZ) || (mCanVariant == CAN_MULTICAN) )
    {
        return mCanVariant;
    }

    uint8_t prop_canVariant = getPropertyInt(PROP_CAN_VARIANT);
    if( (prop_canVariant == CAN_PMZ) || (prop_canVariant == CAN_MULTICAN) )
    {
        mCanVariant = prop_canVariant;
        return mCanVariant;
    }

    int8_t fromCan_canVariant = updateFromCan_Internal_Serv_CAN_VARIANT();
    if( (fromCan_canVariant == CAN_PMZ) || (fromCan_canVariant == CAN_MULTICAN) )
    {
        mCanVariant = fromCan_canVariant;
        return mCanVariant;
    }

    LOGV("getCanVariant ERROR!! mCanVariant:%d prop_canVariant:%d fromCan_canVariant:%d ", mCanVariant, prop_canVariant, fromCan_canVariant);
    return CAN_DEFAULT;
}


void RemoteServiceApp::updateFromProperty_RPCvariables()
{
    uint32_t prop_RPCvariables = getPropertyInt(PROP_RPC_VARIABLES);
    uint32_t prop_timer1time = getPropertyInt(PROP_TIMER_1_TIME);
    uint32_t prop_timer2time = getPropertyInt(PROP_TIMER_2_TIME);
    LOGI("updateFromProperty_RPCvariables prop_RPCvariables:0x%x, prop_timer1time:%d, prop_timer2time:%d",
        prop_RPCvariables, prop_timer1time, prop_timer2time);

    setFFHOperatingStatus((prop_RPCvariables >> 24) & 0xFF);
    mCurrParkClimateModeTCU = ((prop_RPCvariables >> 16) & 0xFF);
    setParkHeatVentTime((prop_RPCvariables >> 8) & 0xFF);
    setRPCtimerActiveStatus(prop_RPCvariables & 0xFF);

    setTimer1Time(prop_timer1time);
    setTimer2Time(prop_timer2time);
}

void RemoteServiceApp::saveProperty_RPCvariables(uint16_t sigid, uint8_t sigVal)
{
    uint32_t buffer[10];

    uint32_t prop_val = getPropertyInt(PROP_RPC_VARIABLES);
    LOGI("saveProperty_RPCvariables prop_val:0x%x, sigid:%d sigVal:%d", prop_val, sigid, sigVal);

    switch(sigid)
    {
        case Signal_FFHOperatingStatus_RX:
            prop_val = prop_val & 0x00FFFFFF;
            prop_val = prop_val ^ (sigVal << 24);
            break;
        case Signal_ParkClimateMode_RX:
            prop_val = prop_val & 0xFF00FFFF;
            prop_val = prop_val ^ (sigVal << 16);
            break;
        case Signal_ParkHeatVentTime_RX:
            prop_val = prop_val & 0xFFFF00FF;
            prop_val = prop_val ^ (sigVal << 8);
            break;
        case Signal_TimerActivationStatus_RX:
            prop_val = prop_val & 0xFFFFFF00;
            prop_val = prop_val ^ sigVal;
            break;
    }
    sprintf((char*) buffer, "%d", prop_val);
    setProperty(PROP_RPC_VARIABLES, (const char *) buffer, true);
}


void RemoteServiceApp::setProperty_seats(uint16_t sigid, uint8_t sigVal)
{
    uint8_t buffer[10];
    sprintf((char*) buffer, "%d", sigVal);

    switch(sigid)
    {
        case Signal_DrvSeatInhib2ndRow_RX:  //FALL-THROUGH
        case Signal_RLSeatInhib2ndRow_RX:
            setProperty(RISM_PROP_DRV2ND_IHB, (const char *) buffer, true);
            break;
        case Signal_PasSeatInhib2ndRow_RX:  //FALL-THROUGH
        case Signal_RMSeatInhib2ndRow_RX:
            setProperty(RISM_PROP_PAS2ND_IHB, (const char *) buffer, true);
            break;
        case Signal_DrvSeatInhib3rdRow_RX:  //FALL-THROUGH
        case Signal_RRSeatInhib2ndRow_RX:
            setProperty(RISM_PROP_DRV3RD_IHB, (const char *) buffer, true);
            break;
        case Signal_PasSeatInhib3rdRow_RX:
            setProperty(RISM_PROP_PAS3RD_IHB, (const char *) buffer, true);
            break;

        case Signal_DrvSeatPosition2ndRow_RX:   //FALL-THROUGH
        case Signal_RLSeatMovementStat_RX:
            setProperty(RISM_PROP_DRV2ND, (const char *) buffer, true);
            break;
        case Signal_PasSeatPosition2ndRow_RX:   //FALL-THROUGH
        case Signal_RCntrSeatMovmentStat_RX:
            setProperty(RISM_PROP_PAS2ND, (const char *) buffer, true);
            break;
        case Signal_DrvSeatPosition3rdRow_RX:   //FALL-THROUGH
        case Signal_RRSeatMovementStat_RX:
            setProperty(RISM_PROP_DRV3RD, (const char *) buffer, true);
            break;
        case Signal_PasSeatPosition3rdRow_RX:   //FALL-THROUGH
        case Signal_SkiHatchStat_RX:
            setProperty(RISM_PROP_PAS3RD, (const char *) buffer, true);
            break;
        default:
            LOGI("setProperty_seats sigid %d is not seats signal", sigid);
            break;
    }
}

uint8_t RemoteServiceApp::getProperty_seats(uint16_t sigid)
{
    switch(sigid)
    {
        case Signal_DrvSeatInhib2ndRow_RX:  //FALL-THROUGH
        case Signal_RLSeatInhib2ndRow_RX:
            return getPropertyInt(RISM_PROP_DRV2ND_IHB);
        case Signal_PasSeatInhib2ndRow_RX:  //FALL-THROUGH
        case Signal_RMSeatInhib2ndRow_RX:
            return getPropertyInt(RISM_PROP_PAS2ND_IHB);
        case Signal_DrvSeatInhib3rdRow_RX:  //FALL-THROUGH
        case Signal_RRSeatInhib2ndRow_RX:
            return getPropertyInt(RISM_PROP_DRV3RD_IHB);
        case Signal_PasSeatInhib3rdRow_RX:
            return getPropertyInt(RISM_PROP_PAS3RD_IHB);

        case Signal_DrvSeatPosition2ndRow_RX:   //FALL-THROUGH
        case Signal_RLSeatMovementStat_RX:
            return getPropertyInt(RISM_PROP_DRV2ND);
        case Signal_PasSeatPosition2ndRow_RX:   //FALL-THROUGH
        case Signal_RCntrSeatMovmentStat_RX:
            return getPropertyInt(RISM_PROP_PAS2ND);
        case Signal_DrvSeatPosition3rdRow_RX:   //FALL-THROUGH
        case Signal_RRSeatMovementStat_RX:
            return getPropertyInt(RISM_PROP_DRV3RD);
        case Signal_PasSeatPosition3rdRow_RX:   //FALL-THROUGH
        case Signal_SkiHatchStat_RX:
            return getPropertyInt(RISM_PROP_PAS3RD);
        default:
            LOGI("getProperty_seats sigid %d is not seats signal", sigid);
            return 0;
    }
}


uint32_t RemoteServiceApp::getPropertyInt(const char* name)
{
    char* prop = getProperty(name);
    LOGI("getPropertyInt name : %s", name);

    if( prop != NULL )
    {
        LOGI("getPropertyInt prop:%s %d", prop, strtoul(prop, NULL, 10));
        return strtoul(prop, NULL, 10);
    }
    else
    {
        LOGE("getPropertyInt %s NULL !!!", name);
        return 0;
    }
}

bool RemoteServiceApp::isRISMFitted()
{
    if(RISM_FITTED == getConvenienceFold())
    {
        return true;
    }
    return false;
}

bool RemoteServiceApp::isRISMAppEnabled()
{
    if(getAppModeRISM() == STATUS_DISABLE)
    {
        return false;
    }
    return true;
}

bool RemoteServiceApp::isSeatType3()
{
    /*
    8.11 Remote Seat Fold for L405
    8.11.1 Seat type 3 2nd Row 40-20-40 (IntelligentSeatFold=0x04 OR
    IntelligentSeatFold=0x07)

    8.12 Remote Seat Unfold for L405
    8.12.1 Seat type 3 2nd Row 40-20-40 (IntelligentSeatFold=0x04 OR
    IntelligentSeatFold=0x07)
    */
    switch(getIntelligentSeatFold())
    {
        case 0x04:
        case 0x07:
            return true;
        default:
            return false;
    }
}
