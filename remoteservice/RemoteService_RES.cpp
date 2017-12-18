/**
* \file    RemoteService_RES.cpp
* \brief     Implementation of RemoteService_RES Class.
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
#define LOG_TAG "RVC_RES"
#include "Log.h"

#include "RemoteService_RES.h"


RemoteService_RES::RemoteService_RES(RemoteServiceApp & app)
    :m_RemoteServiceApp(app)
{

}


RemoteService_RES::~RemoteService_RES(){

}

void RemoteService_RES::requestHandler(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;
    error_t result = E_OK;
    uint8_t currentParkClimate = 0;

    LOGV("requestHandler() serviceType: %d", serviceType);

    if(RemoteService_PrereqCheck(serviceType) == false)
    {
        LOGE("requestHandler() serviceType: %d Prerequisite fail", serviceType);
        return;
    }

    switch(serviceType)
    {
        case RS_ENGINE_START:
        {
            m_RemoteServiceApp.setLogPrerequisiteCheckRequestStatus(0x01);
            m_RemoteServiceApp.setLogTransistionFromREStoNormal(0x00);

            if(RES_delayForReqBCMauth(RS_ENGINE_START) == E_ERROR)
            {
                //RS_ENGINE_START sequence END
                LOGE("requestHandler() serviceType:%d request fail", serviceType);
                return;
            }

            result = RES_reqEngineStart();
            if(result == E_OK)
            {
                m_RemoteServiceApp.reqBCMauthentication(RS_ENGINE_START);
            }
            else
            {
                LOGE("[RS_ENGINE_START] CAN signal Tx error. cause:%d", result);
                //RS_ENGINE_START sequence END
                if(m_RemoteServiceApp.IsVehicleWithFFH()) // RES after FFH
                {
                    m_RemoteServiceApp.RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_FAILURE);
                }
                else // RES WITHOUT FFH
                {
                    m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
                    m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_START);
                }
                m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_READY);
            }
        } break;

        case RS_ENGINE_STOP:
        {
            if(RES_delayForReqBCMauth(RS_ENGINE_STOP) == E_ERROR)
            {
                //RS_ENGINE_STOP sequence END
                return;
            }

            result = RES_reqEngineStop();
            if(result == E_OK)
            {
                m_RemoteServiceApp.sendAckMsgToTSP(RS_ENGINE_STOP, svc::actionResponse);
                m_RemoteServiceApp.reqBCMauthentication(RS_ENGINE_STOP);
            }
            else
            {
                LOGE("[RS_ENGINE_STOP] CAN signal Tx error. cause:%d", result);
                //RS_ENGINE_STOP sequence END
                m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
                m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_STOP);
                m_RemoteServiceApp.setCurrState(RS_ENGINE_STOP, RS_READY);
            }
        } break;

        case RS_FFH_PREREQ:
        {
            if(RES_delayForReqBCMauth(RS_FFH_PREREQ) == E_ERROR)
            {
                //RES(RS_FFH_PREREQ) sequence END
                return;
            }

            result = RES_reqFFHactivation();
            if(result == E_OK)
            {
                m_RemoteServiceApp.reqBCMauthentication(RS_FFH_PREREQ);
            }
            else
            {
                LOGE("[RS_FFH_PREREQ] CAN signal Tx error. cause:%d", result);
                //RES(RS_FFH_PREREQ) sequence END
                m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
                m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_START);
                m_RemoteServiceApp.setCurrState(RS_FFH_PREREQ, RS_READY);
            }
        } break;

        case RS_FFH_START:
        {
            currentParkClimate = m_RemoteServiceApp.getCurrParkClimateReqTCU();
            result = RES_reqEngineHeating();
            if(result != E_OK)
            {
                LOGE("[RS_FFH_START] CAN signal Tx error. cause:%d", result);
                //RS_FFH_START sequence END
                //EngineHeating signal send fail. thus do not need default parkClimate request.
                m_RemoteServiceApp.setCurrParkClimateReqTCU(currentParkClimate);
                m_RemoteServiceApp.setCurrState(RS_FFH_START, RS_READY);
                m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_ACTIVE);
                m_RemoteServiceApp.RemoteService_requestHandler(RS_RES, RS_ENGINE_START);
            }
        } break;

        case RS_FFH_STOP:
        {
            currentParkClimate = m_RemoteServiceApp.getCurrParkClimateReqTCU();
            result = RES_reqFFHstop();
            if(result == E_OK)
            {
                m_RemoteServiceApp.sendAckMsgToTSP(RS_ENGINE_STOP, svc::actionResponse);
                m_RemoteServiceApp.startTimerId(RemoteServiceApp::MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP);
            }
            else
            {
                LOGE("[RS_FFH_STOP] CAN signal Tx error. cause:%d", result);
                //RS_FFH_STOP sequence END
                m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
                m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_STOP);
                m_RemoteServiceApp.setCurrParkClimateReqTCU(currentParkClimate);
                m_RemoteServiceApp.setCurrState(RS_FFH_STOP, RS_READY);
            }
        } break;

        default:
            break;
    }
}

void RemoteService_RES::responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size)
{
    RS_ServiceType serviceType = remoteServiceType;
    uint8_t sigVal;

    sigVal = buf[0];  //temporary get value
    LOGE("responseHandler() serviceType:%d sigId:%d sigVal:0x%02X size:%d ", serviceType, sigId, sigVal, size);

    switch(serviceType)
    {
        case RS_ENGINE_START:
        {
            if(sigId == Signal_RESSequenceStatus_RX)
            {
                switch(sigVal)
                {
                    case 0x0A:  //RES exited before engine start    //FALL-THROUGH
                    case 0x0B:  //RES exited after engine start     //FALL-THROUGH
                    case 0x0C:  //PCM Timer Expired                 //FALL-THROUGH
                    case 0x0D:  //User Pressed Stop Button          //FALL-THROUGH
                        m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_READY);//m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        m_RemoteServiceApp.RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_FAILURE);
                        m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        break;

                    case 0x0E:  //Key Not Found
                        m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_READY);//m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        m_RemoteServiceApp.RemoteService_askUpdateMessageToRVM(VEHICLE_COMMUNICATION_FAILED);
                        m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        break;

                    case 0x07:  //Engine Running
                        m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_READY);//m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        break;

                    //// temp. to update InControlApp -->>> RESSequenceStatus will be sent as debugInfo
                    case 0x09:  //any of the BCM prerequisite is not satisified
                        m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_READY);//m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        m_RemoteServiceApp.RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_FAILURE);
                        m_RemoteServiceApp.stopTimer(RS_ENGINE_START);
                        break;
                    //// temp. to update InControlApp

                    case 0x01:  //all the BCM prerequisites are satisfied                   //FALL-THROUGH
                    case 0x02:  //Successful completion of ignition enable protocol         //FALL-THROUGH
                    case 0x03:  //Successful completion of BCM Prerequisite check at PM6    //FALL-THROUGH
                    case 0x04:  //Successful completion of steering column unlock           //FALL-THROUGH
                    case 0x06:  //CM received StartModeAck = 5 Remote
                        // RESSequenceStatus will be sent as debugInfo
                        break;

                    default:
                        break;
                }
            }
        }
            break;
/*
        case RS_ENGINE_STOP:
        {
            LOGI("RS_ENGINE_STOP  powermode:%d ", m_RemoteServiceApp.RemoteService_getPowerMode());
            if(sigId == Signal_RemoteStartStatusMS_RX)
            {
                if ((sigVal == 0x00) && (m_RemoteServiceApp.RemoteService_getPowerMode() < PM_IGNITION_ON_2))
                {
                    //success Engine stop
                    m_RemoteServiceApp.stopTimer(RS_ENGINE_STOP);
                    //m_RemoteServiceApp.sendAckMsgToTSP(serviceType);
                }
            }
        }
            break;
            */
        default:
            break;
    }
}

bool RemoteService_RES::responseStatus(RS_ServiceType remoteServiceType)
{
    bool result = false;

    switch(remoteServiceType)
    {
        case RS_FFH_PREREQ:
            result = (m_RemoteServiceApp.getRESWindowStatus()
                    && m_RemoteServiceApp.getRESVehicleLockStatus()
                    && m_RemoteServiceApp.getRESAlarmStatus()
                    && m_RemoteServiceApp.getRESHazardSwitchStatus()
                    && m_RemoteServiceApp.getRESBrakePedalStatus()
                    );
            break;
        default:
            break;
    }

    if(!result)
    {
        if(!m_RemoteServiceApp.getRESWindowStatus())
            m_RemoteServiceApp.setNGTPerrorCode(windowNotClosed_chosen);

        if(!m_RemoteServiceApp.getRESVehicleLockStatus())
            m_RemoteServiceApp.setNGTPerrorCode(vehicleNotLocked_chosen);

        if(!m_RemoteServiceApp.getRESAlarmStatus())
            m_RemoteServiceApp.setNGTPerrorCode(alarmActive_chosen);

        if(!m_RemoteServiceApp.getRESHazardSwitchStatus())
            m_RemoteServiceApp.setNGTPerrorCode(hazardWarningOn_chosen);

        if(!m_RemoteServiceApp.getRESBrakePedalStatus())
            m_RemoteServiceApp.setNGTPerrorCode(brakePedalPressed_chosen);

        //need SRD update
        //if(!mRESVehStationaryStatus)
            //m_RemoteServiceApp.setNGTPerrorCode(vehicleNotStationary_chosen);

        //if(!mRESTransmissionStatus)
            //m_RemoteServiceApp.setNGTPerrorCode(transmissionNotInPark_chosen);
    }

    return result;
}

error_t RemoteService_RES::RES_reqEngineStart()
{
    LOGV("RES_reqEngineStart()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_ENGINE_START;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RES::RES_reqEngineStop()
{
    LOGV("RES_reqEngineStop()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_ENGINE_STOP;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RES::RES_reqFFHactivation()
{
    LOGV("RES_reqFFHactivation()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_BCM_PREREQ; //EWuP(EarlyWakeup bit)

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RES::RES_reqEngineHeating()
{
    LOGV("RES_reqEngineHeating()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = RES_ENGINE_HEAT_REQUEST;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    m_RemoteServiceApp.setCurrParkClimateReqTCU(RES_ENGINE_HEAT_REQUEST);

    delete[] values;
    return result;
}

error_t RemoteService_RES::RES_reqFFHstop()
{
    LOGV("RES_reqFFHstop()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = RES_FFH_STOP_REQUEST;

    buf->setData(Signal_ParkClimateReqTCU_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_ParkClimateReqTCU_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
    m_RemoteServiceApp.setCurrParkClimateReqTCU(RES_FFH_STOP_REQUEST);

    delete[] values;
    return result;
}

error_t RemoteService_RES::RES_delayForReqBCMauth(RS_ServiceType serviceType)
{
    error_t result = E_ERROR;
    int32_t count = BCM_RESP_WAIT_MAX_TIME;

    while(count > 0)
    {
        if(m_RemoteServiceApp.getCurrBCMauthService() == RS_SERVICE_TYPE_MAX)
        {
            LOGV("RES (serviceType:%d) TCU-BCM authentication request available.", serviceType);
            result = E_OK;
            break;
        }
        else
        {
            LOGV("wait 1 second for RES (serviceType:%d) TCU-BCM authentication request.", serviceType);
            usleep(BCM_RESP_WAIT_TIME);
            count -= BCM_RESP_WAIT_TIME;
        }
    }

    LOGV("RES_delayForReqBCMauth() serviceType:%d result:0x%x delayedCount:%d(microSeconds)", serviceType, result, (BCM_RESP_WAIT_MAX_TIME - count));

    if((result == E_ERROR) && (serviceType == RS_ENGINE_START))
    {
        if(m_RemoteServiceApp.IsVehicleWithFFH()) // RES after FFH
        {
            m_RemoteServiceApp.RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_FAILURE);
        }
        else // RES WITHOUT FFH
        {
            m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
            m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_START);
        }
        m_RemoteServiceApp.setCurrState(RS_ENGINE_START, RS_READY);
    }
    else if((result == E_ERROR) && (serviceType == RS_ENGINE_STOP))
    {
        m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
        m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_STOP);
        m_RemoteServiceApp.setCurrState(RS_ENGINE_STOP, RS_READY);
    }
    else if((result == E_ERROR) && (serviceType == RS_FFH_PREREQ))
    {
        m_RemoteServiceApp.setNGTPerrorCode(executionFailure_chosen);
        m_RemoteServiceApp.sendNackMsgToTSP(RS_ENGINE_START);
        m_RemoteServiceApp.setCurrState(RS_FFH_PREREQ, RS_READY);
    }
    else
    {

    }

    return result;
}

bool RemoteService_RES::RES_getPowerMode(RS_ServiceType serviceType)
{
    RS_PowerMode powerMode = m_RemoteServiceApp.RemoteService_getPowerMode();

    if(serviceType == RS_ENGINE_STOP)
    {
        m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_PWR, ((powerMode == PM_RUNNING_2) ? STR_OK : STR_FAIL));
        if(powerMode != PM_RUNNING_2)
        {
            LOGI("[RES Prerequisite Fail] PowerMode %d != 7(PM_RUNNING_2)", powerMode);
            return false;
        }
        else
        {
            LOGI("[RES Prerequisite Success] PowerMode == 7(PM_RUNNING_2)");
        }
    }
    else
    {
        m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_PWR, ((powerMode < PM_ACCESSORY_1) ? STR_OK : STR_FAIL));
        if(powerMode >= PM_ACCESSORY_1)
        {
            LOGI("[RES Prerequisite Fail] PowerMode %d >= 4(PM_ACCESSORY_1)", powerMode);
            //svc::vehiclePowerModeNotCorrect_chosen => 9
            return false;
        }
        else
        {
            LOGI("[RES Prerequisite Success] PowerMode %d < 4(PM_ACCESSORY_1)", powerMode);
        }
    }

    return true;
}

bool RemoteService_RES::RES_getRESstatus(RS_ServiceType serviceType)
{
    bool remoteStatus = m_RemoteServiceApp.getCurrRemoteStartStatus();


    if(serviceType == RS_ENGINE_STOP)
    {
        m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_RES, (remoteStatus ? STR_OK : STR_FAIL));
        if(!remoteStatus)
        {
            LOGI("[RES Prerequisite Fail] Not performing the RES");
            //svc::conditionsNotCorrect_chosen => 10
            return false;
        }
        else
        {
            LOGI("[RES Prerequisite Success] performing the RES");
        }
    }
    else
    {
        m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_RES, (!remoteStatus ? STR_OK : STR_FAIL));
        if(remoteStatus)
        {
            LOGI("[RES Prerequisite Fail] performing the RES");
            //svc::conditionsNotCorrect_chosen => 10
            return false;
        }
        else
        {
            LOGI("[RES Prerequisite Success] Not performing the RES");
        }
    }
    return true;
}

bool RemoteService_RES::RES_getFuelLevel(RS_ServiceType serviceType)
{
    uint16_t fuelLevel = m_RemoteServiceApp.getPropertyInt(PROP_FUEL_LEVEL_INDICATED_HS);
    uint16_t resFuelLevelStartMin = m_RemoteServiceApp.getUint16fromConfig(RES_FUEL_LEVEL_START_MIN);
    LOGI("RES_getFuelLevel  fuelLevel:%d resFuelLevelStartMin:%d ",fuelLevel,resFuelLevelStartMin);

    m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_FUEL, ((fuelLevel >= resFuelLevelStartMin) ? STR_OK : STR_FAIL));

    if(fuelLevel < resFuelLevelStartMin)
    {
        LOGI("[RES Prerequisite Fail] Fuel level failure");
        //svc::fuelLevelTooLow_chosen => 12

        m_RemoteServiceApp.setLogfuelLevel(0x01); //low

        return false;
    }
    else
    {
        LOGI("[RES Prerequisite Success] Fuel level OK");

        m_RemoteServiceApp.setLogfuelLevel(0x00); //normal

        return true;
    }
}

bool RemoteService_RES::RES_getRemainingRuntime(RS_ServiceType serviceType)
{
    uint16_t remainingTime_s = m_RemoteServiceApp.getUint16fromConfig(RVC_RES_RUNMAX);
    uint16_t resRunTimeMin_s = m_RemoteServiceApp.getUint16fromConfig(RES_MIN_REMAINING_TIME);

    m_RemoteServiceApp.setRVCproperty(serviceType, NULL, (RVCproperty)SLDD_PROP_RUNTIME, ((remainingTime_s < resRunTimeMin_s) ? STR_FAIL : STR_OK));

    if(remainingTime_s < resRunTimeMin_s)
    {
        LOGI("[RES Prerequisite Fail] RES remaining runtime failure");
        return false;
    }
    else
    {
        LOGI("[RES Prerequisite Success] RES remaining runtime OK");
        return true;
    }
}

bool RemoteService_RES::RES_getFFHOperatingStatus(RS_ServiceType serviceType)
{
    uint8_t ffhOperatingStatus = m_RemoteServiceApp.getFFHOperatingStatus();

    if(serviceType != RS_FFH_PREREQ)
    {
        LOGI("[RES Prerequisite Success] serviceType:%d != RS_FFH_PREREQ ", serviceType);
        return true;
    }

    if(ffhOperatingStatus == FFH_OFF)
    {
        LOGI("[FFH Prerequisite Success] FFHOperatingStatus == FFH_OFF");
        return true;
    }
    else
    {
        LOGI("[FFH Prerequisite Fail] FFHOperatingStatus:%d != FFH_OFF", ffhOperatingStatus);
        return false;
    }
}

bool RemoteService_RES::RemoteService_PrereqCheck(RS_ServiceType remoteServiceType)
{
    LOGV("RemoteService_PrereqCheck() serviceType:%d", remoteServiceType);

    bool result = true;

    if((remoteServiceType == RS_ENGINE_START) && (m_RemoteServiceApp.IsVehicleWithFFH()))
    {
        /*
        check e-mail from Ashisi :
        2016.11.17 Re: Please draw RES start sequence diagram with FFH is fitted separately from without FFH

        - Does TCU should check TCU preReq. again before sending RES request after FFH ends.
             [AP-17Nov] -  No, TCU PreReq check again after FFH ends is NOT required. TCU shall directly send RES request after FFH ends.
        */
        LOGV("RemoteService_PrereqCheck() skip: RES after FFH");
        return true;
    }

    if( remoteServiceType == RS_ENGINE_START || remoteServiceType == RS_FFH_PREREQ )
    {
        /*
        check e-mail from Ashisi :
        2016.12.06 Re: Please draw RES start sequence diagram with FFH is fitted separately from without FFH

        - should Tcu send engine heat request if FFHOperatuonStatus != 0 ?
        AP - Yes. As per RPC SRD requirements, for Park Climate Vent / Heat / Auto TCU needs to check FFHOperatingStatus and send the request to HVAC only if FFHOperatingStatus = 0.
           BUT for Engine Heat Request as part of RES, it is NOT required to check FFHOperatingStatus and TCU shall send engine heat request just based on ParkClimateMode signal.

        if(!RES_getFFHOperatingStatus(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
        }
        */
        if(remoteServiceType == RS_FFH_PREREQ)
            remoteServiceType = RS_ENGINE_START;

        if(!m_RemoteServiceApp.getSVTstatus(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(theftEvent_chosen);
        }
        if(!RES_getPowerMode(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
        }
        if(!RES_getRESstatus(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
        }
        if(!m_RemoteServiceApp.getCrashStatus(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(crashEvent_chosen);
        }
        if(!RES_getFuelLevel(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(fuelLevelTooLow_chosen);
        }
        if(!RES_getRemainingRuntime(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(runTimeExpiry_chosen);
        }
    }


    if(remoteServiceType == RS_ENGINE_STOP)
    {
        if(!RES_getPowerMode(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
        }
        if(!RES_getRESstatus(remoteServiceType))
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen);
        }
    }

    if(remoteServiceType == RS_FFH_STOP)
    {
        int8_t parkClimateMode = m_RemoteServiceApp.getCurrParkClimateModeTCU();
        LOGV("RemoteService_PrereqCheck RS_FFH_STOP  parkClimateMode:%d ", remoteServiceType);
        if( (RES_getPowerMode(RS_ENGINE_STOP) == false)
            && (RES_getRESstatus(RS_ENGINE_STOP) == false)
            && (parkClimateMode == CLIMATE_ENGINE_HEATING) )
        { // stop request whist FFH
            result = true;
        }
        else
        {
            result = false;
            m_RemoteServiceApp.setNGTPerrorCode(climateModeNotCorrect_chosen);
        }
    }

    // RS_FFH_START use responseStatus instead of prereq.

    if(!result)
    {
        m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_SUCCESS, STR_FAIL);
        m_RemoteServiceApp.setRVCproperty(remoteServiceType, NULL, (RVCproperty)SLDD_PROP_CAUSE, STR_PRE_FAIL);

        if((remoteServiceType == RS_ENGINE_STOP) && (m_RemoteServiceApp.getRESstopReason() != RES_STOP_TSP))
            return result;

        if(remoteServiceType == RS_ENGINE_START)
            m_RemoteServiceApp.setCurrState(RS_FFH_PREREQ, RS_READY);

        if(remoteServiceType == RS_FFH_STOP)
        {
            remoteServiceType = RS_ENGINE_STOP;
        }
        m_RemoteServiceApp.setCurrState(remoteServiceType, RS_READY);
        m_RemoteServiceApp.sendNackMsgToTSP(remoteServiceType);
    }

    return result;
}


