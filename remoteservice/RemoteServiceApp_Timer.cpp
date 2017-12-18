
#define LOG_TAG "RVC"
#include "Log.h"

#include "RemoteServiceApp.h"


void RemoteServiceApp::createTimer()
{
    LOGE("createTimer()");
    m_myTimer = new MyTimer(*this);
    m_RESstartTimer = new Timer( m_myTimer , MyTimer::TIMER_RES_START );
    m_RESstopTimer = new Timer( m_myTimer , MyTimer::TIMER_RES_STOP );
    m_RESprereqTimer = new Timer( m_myTimer , MyTimer::TIMER_RES_PREREQ );
    m_FFHmaxRunTimer = new Timer( m_myTimer , MyTimer::TIMER_FFH_MAX_RUN );
    m_RESmaxRunTimer = new Timer( m_myTimer , MyTimer::TIMER_RES_MAX_RUN );
    m_ParkClimateReqTimer = new Timer( m_myTimer , MyTimer::TIMER_CLIMATE_REQ );
    m_FFHuserIrqTimer = new Timer( m_myTimer , MyTimer::TIMER_FFH_USER_STOP );
    m_unlockVehicleTimer = new Timer( m_myTimer , MyTimer::TIMER_UNLOCK_VEHICLE );
    m_secureVehicleTimer = new Timer( m_myTimer , MyTimer::TIMER_SECURE_VEHICLE );
    m_alarmResetTimer = new Timer( m_myTimer , MyTimer::TIMER_ALARM_RESET );
    m_TCUauthSeqTimer = new Timer( m_myTimer, MyTimer::TIMER_TCU_AUTH_SEQ );
    m_parkClimateStartTimer = new Timer( m_myTimer, MyTimer::TIMER_PARK_CLIMATE_START );
    m_parkClimateStopTimer = new Timer( m_myTimer, MyTimer::TIMER_PARK_CLIMATE_STOP );
    m_RPCstatusWaitTimer = new Timer( m_myTimer, MyTimer::TIMER_RPC_STATUS_WAIT );
    m_RPCheatVentWaitTimer = new Timer( m_myTimer, MyTimer::TIMER_PARKHEATVENT_WAIT );
    m_RISMseatReadyTimer = new Timer( m_myTimer, MyTimer::TIMER_RISM_SEAT_READY );
    m_RISMseatMoveTimer = new Timer( m_myTimer, MyTimer::TIMER_RISM_SEAT_MOVE );
    m_NgtpRetryTimer = new Timer( m_myTimer, MyTimer::TIMER_NGTP_RETRY );
    m_RISMcancelMoveTimer = new Timer( m_myTimer, MyTimer::TIMER_RISM_CANCEL_MOVE );
    m_MILfaultDetectTimer = new Timer( m_myTimer, MyTimer::TIMER_MIL_FAULT_DETECT );
    m_RegSVTstatusTimer = new Timer( m_myTimer, MyTimer::TIMER_REG_SVT_STATUS );
    m_skipWarmupVariableTimer = new Timer( m_myTimer, MyTimer::TIMER_SKIP_WARMUP_VALIABLE );
    m_parkClimateShutdownTimer = new Timer( m_myTimer, MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN );
    m_PSIMLockTimer = new Timer( m_myTimer, MyTimer::TIMER_PSIM_LOCK );
    m_parkClimateMinStatusWaitTimer = new Timer( m_myTimer, MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT );
    m_updateResRemainingTimer = new Timer( m_myTimer, MyTimer::TIMER_UPDATE_RES_REMAINING );
    m_parkClimateShutdownTimerForFfhStop = new Timer( m_myTimer, MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP );
    m_RISMupdateReleaseTimer = new Timer( m_myTimer, MyTimer::TIMER_RISM_RELEASE_KEEPPOWER );
    m_keepPowerReleaseTimer = new Timer( m_myTimer, MyTimer::TIMER_KEEPPOWER_RELEASE );
    m_parkClimateStatusTimeout = new Timer( m_myTimer, MyTimer::TIMER_PARK_CLIMATE_STATUS_TIMEOUT );
}

void RemoteServiceApp::deleteTimer()
{
    delete m_myTimer;
    delete m_RESstartTimer;
    delete m_RESstopTimer;
    delete m_RESprereqTimer;
    delete m_FFHmaxRunTimer;
    delete m_RESmaxRunTimer;
    delete m_ParkClimateReqTimer;
    delete m_FFHuserIrqTimer;
    delete m_unlockVehicleTimer;
    delete m_secureVehicleTimer;
    delete m_alarmResetTimer;
    delete m_TCUauthSeqTimer;
    delete m_parkClimateStartTimer;
    delete m_parkClimateStopTimer;
    delete m_RPCstatusWaitTimer;
    delete m_RPCheatVentWaitTimer;
    delete m_RISMseatReadyTimer;
    delete m_RISMseatMoveTimer;
    delete m_NgtpRetryTimer;
    delete m_RISMcancelMoveTimer;
    delete m_MILfaultDetectTimer;
    delete m_RegSVTstatusTimer;
    delete m_skipWarmupVariableTimer;
    delete m_parkClimateShutdownTimer;
    delete m_PSIMLockTimer;
    delete m_parkClimateMinStatusWaitTimer;
    delete m_updateResRemainingTimer;
    delete m_parkClimateShutdownTimerForFfhStop;
    delete m_RISMupdateReleaseTimer;
    delete m_keepPowerReleaseTimer;
    delete m_parkClimateStatusTimeout;
}

void RemoteServiceApp::MyTimer::handlerFunction( int timer_id )
{
    mApplication.setTimerStatus(timer_id, false);

    switch(timer_id)
    {
        case TIMER_RES_START:                   handlerFunction_ResStart();                     break;
        case TIMER_RES_STOP:                    handlerFunction_ResStop();                      break;
        case TIMER_RES_PREREQ:                  handlerFunction_ResPrereq();                    break;
        case TIMER_FFH_MAX_RUN:                 handlerFunction_FfhMaxRun();                    break;
        case TIMER_RES_MAX_RUN:                 handlerFunction_ResMaxRun();                    break;
        case TIMER_CLIMATE_REQ:                 handlerFunction_ClimateReq();                   break;
        case TIMER_FFH_USER_STOP:               handlerFunction_FfhUserStop();                  break;
        case TIMER_UNLOCK_VEHICLE:              handlerFunction_UnlockVehicle();                break;
        case TIMER_SECURE_VEHICLE:              handlerFunction_SecureVehicle();                break;
        case TIMER_ALARM_RESET:                 handlerFunction_AlarmReset();                   break;
        case TIMER_TCU_AUTH_SEQ:                handlerFunction_TcuAuthSeq();                   break;
        case TIMER_PARK_CLIMATE_START:          handlerFunction_ParkClimateStart();             break;
        case TIMER_PARK_CLIMATE_STOP:           handlerFunction_ParkClimateStop();              break;
        case TIMER_RISM_SEAT_READY:             handlerFunction_RismSeatReady();                break;
        case TIMER_RISM_SEAT_MOVE:              handlerFunction_RismSeatMove();                 break;
        case TIMER_NGTP_RETRY:                  handlerFunction_NgtpRetry();                    break;
        case TIMER_RISM_CANCEL_MOVE:            handlerFunction_RismCancelMove();               break;
        case TIMER_MIL_FAULT_DETECT:            handlerFunction_MilFaultDetect();               break;
        case TIMER_REG_SVT_STATUS:              handlerFunction_RegSvtStatus();                 break;
        case TIMER_SKIP_WARMUP_VALIABLE:        handlerFunction_skipWarmupVariable();           break;
        case TIMER_PARKHEATVENT_WAIT:           handlerFunction_parkHeatVentSampleWaitTime();   break;
        case TIMER_PARK_CLIMATE_SHUTDOWN:       handlerFunction_parkClimateShutdownTime();      break;
        case TIMER_PSIM_LOCK:                   handlerFunction_PSIMLock();                     break;
        case TIMER_PARK_CLIMATE_MIN_STATUS_WAIT:handlerFunction_parkClimateMinStatusWaitTime(); break;
        case TIMER_UPDATE_RES_REMAINING:        handlerFunction_updateResRemainingTime();       break;
        case TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP:  handlerFunction_parkClimateShutdownTimeForFfhStop();   break;
        case TIMER_RISM_RELEASE_KEEPPOWER:      handlerFunction_RISMrelease();                  break;
        case TIMER_KEEPPOWER_RELEASE:
            handlerFunction_releaseKeepPower();
            break;
        case TIMER_PARK_CLIMATE_STATUS_TIMEOUT: handlerFunction_parkClimateStatusTimeout();     break;
        case TIMER_RPC_STATUS_WAIT: //FALL-THROUGH
        default:    break;
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_ResStart()
{
    LOGI("[TIMER_RES_START] timer expired.");
    mApplication.setCurrState(RS_ENGINE_START, RS_READY);

    if(mApplication.getRESSequenceStatus() != 0x07) //Engine Running
    {
        mApplication.makeLog(); //SRD_ReqRES0113_V2 Case G
        mApplication.RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_FAILURE);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_ResStop()
{
    LOGI("[TIMER_RES_STOP] timer expired.");
    mApplication.setCurrState(RS_ENGINE_STOP, RS_READY);
    mApplication.stopRESrunTimer();

    // SRD_ReqRES0121_V2
    if(!(mApplication.RemoteService_getPowerMode() < PM_IGNITION_ON_2)
        || (mApplication.getCurrRemoteStartStatus() != false) )
    {
       mApplication.RemoteService_askUpdateMessageToRVM(REMOTE_ENGIEN_STOP_FAILURE);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_ResPrereq()
{
    LOGI("[TIMER_RES_PREREQ] timer expired. getCurrState(RS_FFH_PREREQ):%d getCurrState(RS_ENGINE_START):%d",
        mApplication.getCurrState(RS_FFH_PREREQ), mApplication.getCurrState(RS_ENGINE_START));

    bool isBcmPreReqPm04_FFH = false;
    if(mApplication.getCurrState(RS_FFH_PREREQ) == RS_ACTIVE)
    {
        isBcmPreReqPm04_FFH = true;
    }

    mApplication.setCurrState(RS_FFH_PREREQ, RS_READY);

    //if(mApplication.updateFromCanFrame_resRelevantValiables() == false) // SRD_ReqRES0104_V3
    {
        //mApplication.updateFFHPrereq(); // SRD_ReqRES0104_V3
        mApplication.updateResRelevantValiables(); // SRD_ReqRES0104_V3
    }
    if( mApplication.m_remoteService[RS_RES]->responseStatus(RS_FFH_PREREQ) == false )
    { // BCM preREQ CHECK pm0
        mApplication.stopTimerId(MyTimer::TIMER_RES_START);
        mApplication.sendNackMsgToTSP(RS_ENGINE_START); //The climate system did not respond to the start request.
        mApplication.makeLog();
        return;
    }

    if(isBcmPreReqPm04_FFH == false)
    {
        if(mApplication.IsVehicleWithFFH()) // RES after FFH
        {
            mApplication.RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, mApplication.getCurrParkClimateModeTCU());
        }
        else // RES WITHOUT FFH
        {
            mApplication.sendAckMsgToTSP(RS_ENGINE_START, svc::actionResponse);  //sendAckMessage
        }
        return;
    }
    else  // follow sequence for FFH before engine start
    {
        mApplication.sendAckMsgToTSP(RS_ENGINE_START, svc::actionResponse);  //sendAckMessage here

        /*
        3.8.2.2 TCU Request for Engine Heat Prior to RES
        SRD_ReqRES0105_V2: TCU shall monitor the CAN signal ParkClimateMode and proceed as
        follows according to its state
        */
        mApplication.updateFromCan_parkClimateMode();

        int8_t currParkClimateModeTCU = mApplication.getCurrParkClimateModeTCU();
        LOGI("handlerFunction_ResPrereq currParkClimateModeTCU:%d ", currParkClimateModeTCU);
        switch(currParkClimateModeTCU)
        {
            case CLIMATE_CABIN_HEATING: //FALL-THROUGH
            case CLIMATE_CABIN_VENTING:
                /*
                Case (a) If ParkClimateMode=3 (cabin heating), the TCU shall proceed with RES,
                according to the requirement "RES Request to BCM" .
                Case (b) If ParkClimateMode=4 (cabin venting), the TCU shall proceed with RES,
                according to the requirement "RES Request to BCM" .
                */
                mApplication.setCurrState(RS_FFH_START, RS_READY);
                mApplication.setCurrState(RS_ENGINE_START, RS_ACTIVE);
                mApplication.RemoteService_requestHandler(RS_RES, RS_ENGINE_START);
                break;
            case CLIMATE_ENGINE_HEATING:
                /*
                Case (c) If ParkClimateMode=2 (engine heating), the TCU shall wait for a maximum of
                RES_FFH_RUN_TIME_MAX for ParkClimateMode=0 (off)
                */
                mApplication.setCurrState(RS_FFH_START, RS_ACTIVE);
                mApplication.startTimerId(MyTimer::TIMER_FFH_MAX_RUN, false);
                mApplication.setLogFfhRunTimer(true);
                break;
            case CLIMATE_MODE_OFF: //FALL-THROUGH
            case CLIMATE_MODE_STARTUP:
                /*
                Case (d) If ParkClimateMode=1 (startup) or 0 (Off) the TCU shall send
                ParkClimateReqTCU=5 (engine heat requested (RES)) and proceed according to
                requirement ¡°TCU Interpretation of HVAC Response to Engine Heat Request¡±.
                */
                mApplication.setCurrState(RS_FFH_START, RS_WAIT_RESPONSE);
                mApplication.setParkClimateModeWhenReqEngineHeating(currParkClimateModeTCU);
                mApplication.RemoteService_requestHandler(RS_RES, RS_FFH_START);
                mApplication.startTimerId(MyTimer::TIMER_CLIMATE_REQ);
                break;
            default:
                break;
        }
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_FfhMaxRun()
{
    LOGI("[TIMER_FFH_MAX_RUN] timer expired. getCurrState(RS_FFH_START):%d ", mApplication.getCurrState(RS_FFH_START));
    /*
    3.8.2.2 TCU Request for Engine Heat Prior to RES
    Case (c)
    If TCU does not receive ParkClimateMode=0 (off) within RES_FFH_RUN_TIME_MAX,
    then TCU shall proceed with RES, according to the requirement "RES Request to BCM".
    */
    //Proceeed with RES
    if( (mApplication.getCurrState(RS_FFH_START) == RS_ACTIVE)
        || (mApplication.getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE) )
    {
        mApplication.setCurrState(RS_FFH_PREREQ, RS_READY);
        mApplication.setCurrState(RS_FFH_START, RS_READY);
        mApplication.setCurrState(RS_ENGINE_START, RS_ACTIVE);
        mApplication.RemoteService_requestHandler(RS_RES, RS_ENGINE_START);//mApplication.m_remoteService[RS_RES]->requestHandler(RS_ENGINE_START);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_ResMaxRun()
{
    LOGI("[TIMER_RES_MAX_RUN] timer expired.");
    if((mApplication.getCurrRemoteStartStatus() == true)
        && (mApplication.RemoteService_getPowerMode() == PM_RUNNING_2))
    {
        mApplication.setUint16toConfig(RVC_RES_RUNMAX, 0);
        mApplication.setRESstopReason(RES_STOP_RUNTIME);
        mApplication.RemoteService_receiveMessageFromTSP(RS_ENGINE_STOP);
        mApplication.stopTimerId(MyTimer::TIMER_UPDATE_RES_REMAINING);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_ClimateReq()
{
    uint8_t parkCimateReqTCU = mApplication.getCurrParkClimateReqTCU();

    LOGI("[TIMER_CLIMATE_REQ] timer expired. parkCimateReqTCU:%d state RS_FFH_START:%d RS_ENGINE_START:%d",
        parkCimateReqTCU, mApplication.getCurrState(RS_FFH_START), mApplication.getCurrState(RS_ENGINE_START) );

    if(parkCimateReqTCU == RES_ENGINE_HEAT_REQUEST)
    {
        mApplication.reqDefaultParkClimate();
    }
    if( (mApplication.getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE)
        || (mApplication.getCurrState(RS_FFH_START) == RS_ACTIVE) )
    {
        mApplication.setCurrState(RS_FFH_START, RS_READY);
        mApplication.setCurrState(RS_ENGINE_START, RS_ACTIVE);
        mApplication.RemoteService_requestHandler(RS_RES, RS_ENGINE_START);//mApplication.m_remoteService[RS_RES]->requestHandler(RS_ENGINE_START);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_FfhUserStop()
{
    LOGI("[TIMER_FFH_USER_STOP] timer expired. getFFHOperatingStatus():%d getCurrState(RS_FFH_START):%d",
        mApplication.getFFHOperatingStatus(), mApplication.getCurrState(RS_FFH_START));

    if(mApplication.TcuCheckForUserInterruptionDuringEngineHeat_FFHstopByUser() == false)
    {
        // 3.8.2.4 TCU Check for User Interruption during Engine Heat
        mApplication.setCurrState(RS_FFH_START, RS_READY);
        mApplication.setCurrState(RS_ENGINE_START, RS_ACTIVE);
        mApplication.RemoteService_requestHandler(RS_RES, RS_ENGINE_START);//mApplication.m_remoteService[RS_RES]->requestHandler(RS_ENGINE_START);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_UnlockVehicle()
{
    LOGI("[TIMER_UNLOCK_VEHICLE] timer expired.");
    // last check. DoorLatchStatus.
    mApplication.setCurrState(RS_UNLOCK_VEHICLE, RS_READY);

    mApplication.setRVCproperty(RS_UNLOCK_VEHICLE, NULL, (RVCproperty)PROP_SUCCESS, STR_FAIL);
    mApplication.setRVCproperty(RS_UNLOCK_VEHICLE, NULL, (RVCproperty)PROP_CAUSE, STR_TIMEOUT);

    if(mApplication.getDBGcmdActiveStatus()) {
        mApplication.RemoteService_DBGcmdFailResponse(RS_UNLOCK_VEHICLE);
        mApplication.setDBGcmdActiveStatus(false);
    } else {
        // send Nack msg to TSP with cause. timeout
        mApplication.setNGTPerrorCode(timeout_chosen);
        mApplication.sendNackMsgToTSP(RS_UNLOCK_VEHICLE);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_SecureVehicle()
{
    LOGI("[TIMER_SECURE_VEHICLE] timer expired.");
    mApplication.setCurrState(RS_SECURE_VEHICLE, RS_READY);

    mApplication.setRVCproperty(RS_SECURE_VEHICLE, NULL, (RVCproperty)PROP_SUCCESS, STR_FAIL);
    mApplication.setRVCproperty(RS_SECURE_VEHICLE, NULL, (RVCproperty)PROP_CAUSE, STR_TIMEOUT);

    if(mApplication.getDBGcmdActiveStatus()) {
        mApplication.RemoteService_DBGcmdFailResponse(RS_SECURE_VEHICLE);
        mApplication.setDBGcmdActiveStatus(false);
    } else {
        // send Nack msg to TSP with cause. timeout
        mApplication.setNGTPerrorCode(timeout_chosen);
        mApplication.sendNackMsgToTSP(RS_SECURE_VEHICLE);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_AlarmReset()
{
    LOGI("[TIMER_ALARM_RESET] timer expired.");
    // check. AlarmMode.
    mApplication.setCurrState(RS_ALARM_RESET, RS_READY);

    mApplication.setRVCproperty(RS_ALARM_RESET, NULL, (RVCproperty)PROP_SUCCESS, STR_FAIL);
    mApplication.setRVCproperty(RS_ALARM_RESET, NULL, (RVCproperty)PROP_CAUSE, STR_TIMEOUT);

    if(mApplication.getDBGcmdActiveStatus()) {
        mApplication.RemoteService_DBGcmdFailResponse(RS_ALARM_RESET);
        mApplication.setDBGcmdActiveStatus(false);
    } else {
        // send Nack msg to TSP with cause. timeout
        mApplication.setNGTPerrorCode(timeout_chosen);
        mApplication.sendNackMsgToTSP(RS_ALARM_RESET);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_TcuAuthSeq()
{
#if 1  // TODO: need "else" case check for PC tool dbg command
    RS_ServiceType serviceType = mApplication.getCurrBCMauthService();
    uint8_t buf[CAN_VALUES_MAX_BUF_SIZE] = {0,};
    buf[0] = BCM_AUTH_TIME_OUT;

    LOGE("[TIMER_TCU_AUTH_SEQ] timer expired. serviceType:%d", serviceType);
    mApplication.RemoteService_receiveBCMauthStatus_ng(serviceType, buf, 1);
    mApplication.setCurrBCMauthService(RS_SERVICE_TYPE_MAX);
#else
    RS_ServiceType serviceType;
    sp<Buffer> resp = new Buffer();
    uint8_t *dbgResult = new uint8_t;
    dbgResult[0] = DBG_BCM_AUTH_FAIL;

    //error: can't receive BCM authentication status signal.
    serviceType = mApplication.getCurrBCMauthService();
    mApplication.setCurrBCMauthService(RS_SERVICE_TYPE_MAX);
    //mApplication.m_vifManager->unregisterReceiver(Signal_Internal_Serv_BCM_AUTH_RESP, mApplication.getVifReceiver());

    mApplication.setCurrState(serviceType, RS_READY);

    if(serviceType == RS_FFH_PREREQ)
        serviceType = RS_ENGINE_START;
    mApplication.setRVCproperty(serviceType, NULL, (RVCproperty)PROP_AUTH, STR_TIMEOUT);
    mApplication.setRVCproperty(serviceType, NULL, (RVCproperty)PROP_SUCCESS, STR_FAIL);
    mApplication.setRVCproperty(serviceType, NULL, (RVCproperty)PROP_CAUSE, STR_BCM_FAIL);

    if(mApplication.getDBGcmdActiveStatus()) {
        //send to dbgMgr payload with 0x04
        resp->setTo(dbgResult, 0x01);
        if(serviceType == RS_UNLOCK_VEHICLE)
            mApplication.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_UNLOCK, DBG_CMD_TYPE_REQUEST, resp);
        else if(serviceType == RS_SECURE_VEHICLE)
            mApplication.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_SECURE, DBG_CMD_TYPE_REQUEST, resp);
        else if(serviceType == RS_ALARM_RESET)
            mApplication.RemoteService_sendDBGcmdResponse(DBG_CMD_ALARM_RESET, DBG_CMD_TYPE_REQUEST, resp);
        else if(serviceType == RS_HONK_FLASH)
            mApplication.RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_LOCATION, DBG_CMD_TYPE_REQUEST, resp);
        else
        { // nothing
        }
        mApplication.setDBGcmdActiveStatus(false);
    } else {
        // send Nack msg to TSP with cause.
        mApplication.setNGTPerrorCode(executionFailure_chosen);
        mApplication.sendNackMsgToTSP(serviceType, 4/*notAllowed_chosen*/);
    }
    delete dbgResult;
#endif
}

void RemoteServiceApp::MyTimer::handlerFunction_ParkClimateStart()
{
    LOGI("[TIMER_PARK_CLIMATE_START] timer expired.");

    mApplication.updateParkClimateStatusSampling();

    uint8_t parkCimateReqTCU = mApplication.getCurrParkClimateReqTCU();


    LOGI("  parkCimateReqTCU: %d getCurrParkClimateModeTCU:%d getFFHOperatingStatus:%d ",
        parkCimateReqTCU, mApplication.getCurrParkClimateModeTCU(), mApplication.getFFHOperatingStatus());

    if(mApplication.getCurrState(RS_PARK_CLIMATE_AUTO) == RS_WAIT_RESPONSE)
    {
        /*
        5.9.5 Park Climate Status Notification
        SRD_ReqRPC2121_V2
        Following a request to activate park climate, if at timer expiry ParkClimateModeTCU=0
        (off), the TCU shall, in the absence of any error message in FFHOperatingStatus,
        interpret it as a failure of HVAC
        */
        if(mApplication.getCurrParkClimateModeTCU() == CLIMATE_MODE_OFF)
        {
            switch(mApplication.getFFHOperatingStatus())
            {
                case FFH_FAIL_LOW_VOLTAGE:
                    mApplication.setNGTPerrorCode(batteryLevelTooLow_chosen);
                    break;
                case FFH_FAIL_LOW_FUEL:
                    mApplication.setNGTPerrorCode(fuelLevelTooLow_chosen);
                    break;
                case FFH_FAIL_MAX_TIME:
                    mApplication.setNGTPerrorCode(runTimeExpiry_chosen);
                    break;
                case FFH_FAIL_SVC_REQ:     //FALL-THROUGH
                case FFH_FAIL_SYS_FAILURE:
                    mApplication.setNGTPerrorCode(executionFailure_chosen);
                    break;
                case FFH_OFF:           //FALL-THROUGH
                case FFH_STARTINGUP:    //FALL-THROUGH
                case FFH_RUNNING:       //FALL-THROUGH
                case FFH_SHUTDOWN:      //FALL-THROUGH
                case FFH_STOP_BY_USER:  //FALL-THROUGH
                case FFH_RESERVED:      //FALL-THROUGH
                case FFH_STATUS_UNKNOWN:
                default:
                    mApplication.setNGTPerrorCode(hvacSystemFailure_chosen);
                    break;
            }
            mApplication.sendNackMsgToTSP(RS_PARK_CLIMATE_AUTO);
        }
        else
        {
            mApplication.setNGTPerrorCode(hvacTimeout_chosen);
            mApplication.sendNackMsgToTSP(RS_PARK_CLIMATE_AUTO);
        }
    }
    mApplication.setCurrState(RS_PARK_CLIMATE_AUTO, RS_READY);
    // TODO: requested RPC mode check.
    mApplication.setCurrState(RS_PARK_CLIMATE_VENT, RS_READY);
    mApplication.setCurrState(RS_PARK_CLIMATE_HEAT, RS_READY);

/*    if((parkCimateReqTCU >= RPC_REQ_PARK_VENT && parkCimateReqTCU <= RPC_REQ_PARK_AUTO)
        || (parkCimateReqTCU == RES_ENGINE_HEAT_REQUEST))
    {
        mApplication.reqDefaultParkClimate();
    }*/
    switch(parkCimateReqTCU)
    {
        case RPC_REQ_PARK_VENT: //FALL-THROUGH
        case RPC_REQ_PARK_HEAT: //FALL-THROUGH
        case RPC_REQ_PARK_AUTO: //FALL-THROUGH
        case RES_ENGINE_HEAT_REQUEST:
            mApplication.reqDefaultParkClimate();
            break;
        default:
            break;

    }
    /*5.9.5 Park Climate Status Notification
    SRD_ReqRPC2121_V2: The TCU shall notify the park climate status as follows:
    Upon timer expiry the TCU shall transmit the latest status (ParkClimateModeTCU,
    FFHOperatingStatus and ParkHeatVentTime, TimerActivationStatus, and, if enabled, the
    earlier of Timer1Time and Timer2Time) to the TSP.
    */
    mApplication.RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, mApplication.getCurrParkClimateModeTCU());
}

void RemoteServiceApp::MyTimer::handlerFunction_ParkClimateStop()
{
    uint8_t parkCimateReqTCU = mApplication.getCurrParkClimateReqTCU();
    LOGI("[TIMER_PARK_CLIMATE_STOP] timer expired. parkCimateReqTCU: %d getCurrParkClimateModeTCU:%d getFFHOperatingStatus:%d ",
        parkCimateReqTCU, mApplication.getCurrParkClimateModeTCU(), mApplication.getFFHOperatingStatus());

    mApplication.setCurrState(RS_PARK_CLIMATE_STOP, RS_READY);

    if( (mApplication.getCurrParkClimateModeTCU() != CLIMATE_MODE_OFF)
        && (mApplication.getFFHOperatingStatus() >= FFH_RESERVED) )
    {
        /*
        5.9.5 Park Climate Status Notification
        SRD_ReqRPC2121_V2
        Following a request to activate park climate, if at timer expiry ParkClimateModeTCU=0
        (off), the TCU shall, in the absence of any error message in FFHOperatingStatus,
        interpret it as a failure of HVAC
        */
        mApplication.setNGTPerrorCode(hvacSystemFailure_chosen);
        mApplication.sendNackMsgToTSP(RS_PARK_CLIMATE_STOP);
    }
    else
    {
        mApplication.setNGTPerrorCode(timeout_chosen);
        mApplication.sendNackMsgToTSP(RS_PARK_CLIMATE_STOP);
    }

    if(parkCimateReqTCU == RPC_REQ_PARK_STOP)
    {
        mApplication.reqDefaultParkClimate();
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_RismSeatReady()
{
    LOGI("[TIMER_RISM_SEAT_READY] timer expired. start Seat move request.");
    //current RS_RISM_MOVE status == RS_ACTIVE
    mApplication.startTimerId(MyTimer::TIMER_RISM_SEAT_MOVE, false);
    mApplication.RemoteService_requestHandler(RS_RISM, RS_RISM_MOVE);
}

void RemoteServiceApp::MyTimer::handlerFunction_RismSeatMove()
{
    // TODO: Maybe Seat move fail but how to report to TSP?
    LOGI("[TIMER_RISM_SEAT_MOVE] timer expired.");

    //if need then update seat status. use getCurrCANData(allSeat,)
    mApplication.setCurrState(RS_RISM_MOVE, RS_ACTIVE);  //WAIT -> ACTIVE
    mApplication.RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
}

void RemoteServiceApp::MyTimer::handlerFunction_NgtpRetry()
{
    LOGI("[TIMER_NGTP_RETRY] timer expired.");
    //resend msg to TSP.
}

void RemoteServiceApp::MyTimer::handlerFunction_RismCancelMove()
{
    LOGI("[TIMER_RISM_CANCEL_MOVE] timer expired.");
    //can't receive Seat move cancel status.
    //send Nack msg to TSP with reason
}

void RemoteServiceApp::MyTimer::handlerFunction_MilFaultDetect()
{
    LOGI("[TIMER_MIL_FAULT_DETECT] timer expired.");

    //send status update message to TSP with MIL status
    if(mApplication.getEngOBDMil())
    {
        LOGV("MIL fault detected.");
        // TODO:send update(alert?) message to TSP with MIL on
    }
    else
    {
        LOGV("MIL fault not detected.");
        // TODO:send update(alert?) message to TSP with MIL off
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_RegSvtStatus()
{
    LOGI("[TIMER_REG_SVT_STATUS] timer expired.");

    if((mApplication.getUint8fromConfig(APP_MODE_SVT) == STATUS_ENABLE)
        && (mApplication.getSubscriptSVT() == false) )
    {
        LOGV("Retry SVT post receiver registration.");
        mApplication.registerSVTpostReceiver();
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_skipWarmupVariable()
{
    LOGI("[TIMER_SKIP_WARMUP_VALIABLE] timer expired.");
    mApplication.updateWarmupVariable();
}

void RemoteServiceApp::MyTimer::handlerFunction_parkHeatVentSampleWaitTime()
{
    LOGI("[TIMER_PARKHEATVENT_WAIT] timer expired.");
    mApplication.updateFromCan_ParkHeatVentTime();
}

void RemoteServiceApp::MyTimer::handlerFunction_parkClimateShutdownTime()
{
    int8_t currParkClimateModeTCU = mApplication.getCurrParkClimateModeTCU();

    LOGI("[TIMER_PARK_CLIMATE_SHUTDOWN] timer expired. getCurrParkClimateReqTCU():%d, currParkClimateModeTCU:%d ",
        mApplication.getCurrParkClimateReqTCU(), currParkClimateModeTCU );
    if(mApplication.getCurrParkClimateReqTCU() == RPC_REQ_PARK_STOP)
    {
        /*
        5.7.2 Reset of ParkClimateReqTCU Request Value
        When sending a stop request (ParkClimateReqTCU = 4), the TCU shall maintain the request
        value until any of the following conditions occur. Then the TCU shall reset the value to the default
        of 0.
            1. CAN sleep occurs
            2. parkClimateModeTCU = 0
            3. PARK_CLIMATE_SHUTDOWN_TIME elapses.
        */
        mApplication.reqDefaultParkClimate();
    }
    mApplication.updateParkClimateStatusSampling();

    /*5.9.5 Park Climate Status Notification
    SRD_ReqRPC2121_V2: The TCU shall notify the park climate status as follows:
    Upon timer expiry the TCU shall transmit the latest status (ParkClimateModeTCU,
    FFHOperatingStatus and ParkHeatVentTime, TimerActivationStatus, and, if enabled, the
    earlier of Timer1Time and Timer2Time) to the TSP.
    */
    mApplication.RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, currParkClimateModeTCU);
}

void RemoteServiceApp::MyTimer::handlerFunction_PSIMLock()
{
    LOGI("[TIMER_PSIM_LOCK] timer expired.");
    if(TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM)
        mApplication.RvcPsimLockRelease();
}

void RemoteServiceApp::MyTimer::handlerFunction_parkClimateMinStatusWaitTime()
{
    /*
    5.9.2 Park Climate Status Sampling Upon Network Wakeup
    SRD_ReqRPC2117_V1: Following network wakeup, the TCU shall wait for the FFHOperatingStatus
    and parkClimateModeTCU at least time period defined in PARK_CLIMATE_MIN_STATUS_WAIT
    _TIME, retaining the most recent value, before taking any action based upon these values.
    This is to provide enough time for HVAC to reflect and send the correct status.

    5.9.9 Park Climate Status Sampling Upon Network Wakeup
    SRD_ReqRPC2145_V1: Following network wakeup, the TCU shall wait for the FFHOperatingStatus,
    ParkClimateMode, ParkHeatVentTime, TimerActivationStatus, Timer1Time, Timer2Time at least
    time period defined in PARK_CLIMATE_MIN_STATUS_WAIT _TIME, retaining the most recent
    value, before taking any action based upon these values. This is to provide enough time for
    HVAC to reflect and send the correct status.
    */

    RS_ServiceState currState_RS_PARK_CLIMATE_AUTO = mApplication.getCurrState(RS_PARK_CLIMATE_AUTO);
    RS_ServiceState currState_RS_PARK_CLIMATE_STOP = mApplication.getCurrState(RS_PARK_CLIMATE_STOP);
    RS_ServiceState currState_RS_PARK_CLIMATE_VENT = mApplication.getCurrState(RS_PARK_CLIMATE_VENT);
    RS_ServiceState currState_RS_PARK_CLIMATE_HEAT = mApplication.getCurrState(RS_PARK_CLIMATE_HEAT);

    uint8_t FFHOperatingStatus_before = mApplication.getFFHOperatingStatus();
    uint8_t parkClimateModeTCU_before = mApplication.getCurrParkClimateModeTCU();

    LOGI("[TIMER_PARK_CLIMATE_MIN_STATUS_WAIT] timer expired. currState auto:%d stop:%d vent:%d heat:%d",
        currState_RS_PARK_CLIMATE_AUTO, currState_RS_PARK_CLIMATE_STOP, currState_RS_PARK_CLIMATE_VENT, currState_RS_PARK_CLIMATE_HEAT);


    bool sigRvced = mApplication.updateParkClimateStatusSampling();
    if(sigRvced == true)
    {
        if(mApplication.isTimerOn(TIMER_PARK_CLIMATE_STATUS_TIMEOUT))
        {
            /*
            5.9.4 Park Climate Status Timeout
            SRD_ReqRPC2119_V2:
            The TCU shall cancel the timer upon receipt of the required signals from HVAC (receipt of those
            signals must also conform where applicable to requirement ¡°Park Climate Status Sampling upon
            Network Wakeup¡±.
            */
            mApplication.stopTimerId(TIMER_PARK_CLIMATE_STATUS_TIMEOUT);

            /*
            5.9.3 Park Climate Status Check
            SRD_ReqRPC2118_V2: Upon receipt of any remote park climate request from the TSP when ALL OF
            the following conditions are met the TCU shall wake up the vehicle CAN bus, if not already awake,
            and receive the latest status in FFHOperatingStatus and parkClimateModeTCU:
            PARK_CLIMATE_SYSTEM_PRESENT = true (FBH is fitted)
            AND
            PARK_CLIMATE_REMOTE_ENABLED = true (RPC is provisioned)
            AND
            PowerMode = 0
            The TCU shall then proceed with the remote park climate request as specified in the following
            requirements: ¡°Park Heating Invocation¡±, ¡°Park Ventilation Invocation¡±, ¡°Park Climate (Auto)
            Invocation¡± and ¡°Park Climate Stop¡±.
            */
            RS_ServiceType serviceType = RS_SERVICE_TYPE_MAX;
            if(currState_RS_PARK_CLIMATE_AUTO == RS_ACTIVE)
            {
                serviceType = RS_PARK_CLIMATE_AUTO;
            }
            else if(currState_RS_PARK_CLIMATE_STOP == RS_ACTIVE)
            {
                serviceType = RS_PARK_CLIMATE_STOP;
            }
            else if(currState_RS_PARK_CLIMATE_VENT == RS_ACTIVE)
            {
                serviceType = RS_PARK_CLIMATE_VENT;
            }
            else if(currState_RS_PARK_CLIMATE_HEAT == RS_ACTIVE)
            {
                serviceType = RS_PARK_CLIMATE_HEAT;
            }
            else
            {
                return;
            }
            if(serviceType != RS_SERVICE_TYPE_MAX)
            {
                mApplication.RemoteService_requestHandler(RS_RPC, serviceType);
            }
        }
        else
        {
            uint8_t FFHOperatingStatus_after = mApplication.getFFHOperatingStatus();
            uint8_t parkClimateModeTCU_after = mApplication.getCurrParkClimateModeTCU();

            if(FFHOperatingStatus_before != FFHOperatingStatus_after)
            {
                LOGI("FFHOperatingStatus changed.. %d -> %d", FFHOperatingStatus_before, FFHOperatingStatus_after);
                mApplication.rcvMCU_action_FFHOperatingStatusRX(FFHOperatingStatus_after);
            }
            if(parkClimateModeTCU_before != parkClimateModeTCU_after)
            {
                LOGI("parkClimateReqTCU changed.. %d -> %d", parkClimateModeTCU_before, parkClimateModeTCU_after);
                mApplication.rcvMCU_action_ParkClimateModeRX(parkClimateModeTCU_after);
            }
        }
    }
}


void RemoteServiceApp::MyTimer::handlerFunction_updateResRemainingTime()
{
    LOGI("[TIMER_UPDATE_RES_REMAINING] timer expired.");
    if(mApplication.isTimerOn(TIMER_RES_MAX_RUN))
    {
        mApplication.startTimerId(TIMER_UPDATE_RES_REMAINING);
        mApplication.RemoteService_recalcurateResRunMax();
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_parkClimateShutdownTimeForFfhStop()
{
    LOGI("[TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP] timer expired. getCurrParkClimateReqTCU():%d getCurrParkClimateModeTCU():%d",
        mApplication.getCurrParkClimateReqTCU(), mApplication.getCurrParkClimateModeTCU() );

    mApplication.setCurrState(RS_FFH_STOP, RS_READY);

    if(mApplication.getCurrParkClimateModeTCU() != CLIMATE_MODE_OFF)
    {
        mApplication.RemoteService_askUpdateMessageToRVM(REMOTE_ENGIEN_STOP_FAILURE);
    }
}

void RemoteServiceApp::MyTimer::handlerFunction_RISMrelease()
{
    LOGI("[TIMER_RISM_RELEASE_KEEPPOWER] timer expired.");
    mApplication.RvcKeepPowerRelease();
}

void RemoteServiceApp::MyTimer::handlerFunction_releaseKeepPower()
{
    LOGE("[TIMER_KEEPPOWER_RELEASE] timer expired. This is abnormal case.");
    mApplication.RvcAllKeepPowerRelease();
}

void RemoteServiceApp::MyTimer::handlerFunction_parkClimateStatusTimeout()
{
    /* 5.9.4 Park Climate Status Timeout
    SRD_ReqRPC2119_V2
    If the timer expires without the above signals being received from the HVAC the TCU shall abort
    the request and send a specific error response to the TSP.
    */
    RS_ServiceState currState_RS_PARK_CLIMATE_AUTO = mApplication.getCurrState(RS_PARK_CLIMATE_AUTO);
    RS_ServiceState currState_RS_PARK_CLIMATE_STOP = mApplication.getCurrState(RS_PARK_CLIMATE_STOP);
    RS_ServiceState currState_RS_PARK_CLIMATE_VENT = mApplication.getCurrState(RS_PARK_CLIMATE_VENT);
    RS_ServiceState currState_RS_PARK_CLIMATE_HEAT = mApplication.getCurrState(RS_PARK_CLIMATE_HEAT);

    LOGI("[TIMER_PARK_CLIMATE_STATUS_TIMEOUT] timer expired. currState auto:%d stop:%d vent:%d heat:%d",
        currState_RS_PARK_CLIMATE_AUTO, currState_RS_PARK_CLIMATE_STOP, currState_RS_PARK_CLIMATE_VENT, currState_RS_PARK_CLIMATE_HEAT);

    RS_ServiceType serviceType = RS_SERVICE_TYPE_MAX;
    if(currState_RS_PARK_CLIMATE_AUTO == RS_ACTIVE)
    {
        serviceType = RS_PARK_CLIMATE_AUTO;
    }
    else if(currState_RS_PARK_CLIMATE_STOP == RS_ACTIVE)
    {
        serviceType = RS_PARK_CLIMATE_STOP;
    }
    else if(currState_RS_PARK_CLIMATE_VENT == RS_ACTIVE)
    {
        serviceType = RS_PARK_CLIMATE_VENT;
    }
    else if(currState_RS_PARK_CLIMATE_HEAT == RS_ACTIVE)
    {
        serviceType = RS_PARK_CLIMATE_HEAT;
    }
    else
    {
        return;
    }

    if(mApplication.updateParkClimateStatusSampling() == true)
    {
        mApplication.RemoteService_requestHandler(RS_RPC, serviceType);
    }
    else
    {
        mApplication.setNGTPerrorCode(hvacTimeout_chosen);
        mApplication.sendNackMsgToTSP(serviceType);
        mApplication.setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, "parkClimateMode fail");
        mApplication.setCurrState(serviceType, RS_READY);
    }
}


void RemoteServiceApp::startTimer(RS_ServiceType serviceType)   //arg2: timeOut, arg3 cycle
{
    switch(serviceType)
    {
        case RS_PARK_CLIMATE_AUTO: //FALL-THROUGH
        case RS_PARK_CLIMATE_VENT: //FALL-THROUGH
        case RS_PARK_CLIMATE_HEAT:
            startTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
            break;
        case RS_PARK_CLIMATE_STOP:
            startTimerId(MyTimer::TIMER_PARK_CLIMATE_STOP);
            break;
        case RS_SEAT_MOVE_INIT:
            startTimerId(MyTimer::TIMER_RISM_SEAT_READY);
            break;
        case RS_SEAT_MOVE_COMPLETE:
            startTimerId(MyTimer::TIMER_RISM_SEAT_MOVE);
            break;
        //case RS_FFH_STOP:
            //startTimerId(MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP);
            //break;
        default:
            LOGD("startTimer not implemented!! serviceType:%d ",serviceType);
            break;
    }
}

void RemoteServiceApp::stopTimer(RS_ServiceType serviceType)
{
    switch(serviceType)
    {
        case RS_ENGINE_START:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_RES_START);
            break;
        case RS_ENGINE_STOP:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_RES_STOP);
            break;
        case RS_UNLOCK_VEHICLE:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_UNLOCK_VEHICLE);
            break;
        case RS_SECURE_VEHICLE:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_SECURE_VEHICLE);
            break;
        case RS_PARK_CLIMATE_AUTO: //FALL-THROUGH
        case RS_PARK_CLIMATE_VENT: //FALL-THROUGH
        case RS_PARK_CLIMATE_HEAT:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
            break;
        case RS_PARK_CLIMATE_STOP:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_STOP);
            break;
        case RS_ALARM_RESET:
            setCurrState(serviceType, RS_READY);
            stopTimerId(MyTimer::TIMER_ALARM_RESET);
            break;
        case RS_SEAT_MOVE_INIT:
            stopTimerId(MyTimer::TIMER_RISM_SEAT_READY);
            break;
        case RS_SEAT_MOVE_COMPLETE:
            stopTimerId(MyTimer::TIMER_RISM_SEAT_MOVE);
            break;
        default:
            LOGD("stopTimer not implemented!! serviceType:%d ",serviceType);
            break;
    }
}

void RemoteServiceApp::startTimerId(uint32_t timerId, bool bRestart)   //arg2: timeOut, arg3 cycle
{
    uint32_t duration = 0;

//    LOGD("startTimerId  timerId:%d bRestart:%d ", timerId, bRestart);
    if(isTimerOn(timerId))
    {
        if(bRestart == false)
        {
            LOGD("startTimerId RETURN: timer %d is running", timerId);
            return;
        }
        else
        {
            LOGD("startTimerId timer %d is running, stop and start again", timerId);
            stopTimerId(timerId);
        }
    }

    switch(timerId)
    {
        case MyTimer::TIMER_RES_START:
            //duration = RES_ENG_START_TIME;
            duration = getUint16fromConfig(CONFIG_RES_ENG_START_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, RES_ENG_START_TIME);
                duration = RES_ENG_START_TIME;
            }
            m_RESstartTimer->setDuration(duration, 0);
            m_RESstartTimer->start();
            LOGD("startTimerId  TIMER_RES_START(%d) duration(ClimateControlResEngineStartTimer_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_RES_STOP:
            duration = RES_STOP_REQ_RESP_TIME;
            //duration = getUint16fromConfig(CONFIG_RES_STOP_REQ_RESP_TIME);
            m_RESstopTimer->setDurationMs(duration, 0);
            m_RESstopTimer->start();
            LOGD("startTimerId  TIMER_RES_STOP(%d) duration(RES_STOP_REQ_RESP_TIME):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_RES_PREREQ:
            //duration = RES_PREREQ_RESPONSE_TIME;
            duration = getUint16fromConfig(CONFIG_RES_PREREQ_RESPONSE_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, RES_PREREQ_RESPONSE_TIME);
                duration = RES_PREREQ_RESPONSE_TIME;
            }
            m_RESprereqTimer->setDurationMs(duration, 0);
            m_RESprereqTimer->start();
            LOGD("startTimerId  TIMER_RES_PREREQ(%d) duration(ClimateControlResPreReqRespTimer_ms):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_FFH_MAX_RUN:
            //duration = RES_FFH_RUN_TIME_MAX;
            duration = getUint16fromConfig(CONFIG_RES_FFH_RUN_TIME_MAX);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, RES_FFH_RUN_TIME_MAX);
                duration = RES_FFH_RUN_TIME_MAX;
            }
            #if 0
            m_FFHmaxRunTimer->setDuration(duration, 0);
            m_FFHmaxRunTimer->start();
            #else
            m_alarmManager->setAlarmFromNow(APP_ID_REMOTESERVICE, duration*1000, false, m_CallbackAlarmListener);
            #endif
            LOGD("startTimerId  TIMER_FFH_MAX_RUN(%d) duration(ClimateControlResFfhRunTimeMax_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_RES_MAX_RUN:
            duration = getUint16fromConfig(RVC_RES_RUNMAX);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d ", duration);
                duration = getUint16fromConfig(RES_ENGINE_RUN_MAX);
                if(duration <= 0)
                {
                    LOGE("duration ERROR %d ", duration);
                    duration = 18000;
                }
            }
            m_RESmaxRunTimer->setDuration(duration, 0);
            m_RESmaxRunTimer->start();
            LOGD("startTimerId  TIMER_RES_MAX_RUN(%d) duration(config RvcResRunTime_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_CLIMATE_REQ:
            //duration = PARK_CLIMATE_STARTUP_TIME;
            duration = getUint16fromConfig(CONFIG_PARK_CLIMATE_STARTUP_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, PARK_CLIMATE_STARTUP_TIME);
                duration = PARK_CLIMATE_STARTUP_TIME;
            }
            m_ParkClimateReqTimer->setDuration(duration, 0);
            m_ParkClimateReqTimer->start();
            LOGD("startTimerId  TIMER_CLIMATE_REQ(%d) duration(ClimateControlParkClimateStartupTime_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_FFH_USER_STOP:
            //duration = RES_FFH_STOPPEDBY_USER_DETECTION_TIME;
            duration = getUint16fromConfig(CONFIG_RES_FFH_STOPPEDBY_USER_DETECTION_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, RES_FFH_STOPPEDBY_USER_DETECTION_TIME);
                duration = RES_FFH_STOPPEDBY_USER_DETECTION_TIME;
            }
            m_FFHuserIrqTimer->setDuration(duration, 0);
            m_FFHuserIrqTimer->start();
            LOGD("startTimerId  TIMER_FFH_USER_STOP(%d) duration(ClimateControlResUserInterruptionTimer_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_UNLOCK_VEHICLE:
            //duration = DOOR_UNLOCK_TIMEOUT;
            duration = getUint32fromConfig(CONFIG_DOOR_UNLOCK_TIMEOUT);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, DOOR_UNLOCK_TIMEOUT);
                duration = DOOR_UNLOCK_TIMEOUT;
            }
            m_unlockVehicleTimer->setDurationMs(duration, 0);
            m_unlockVehicleTimer->start();
            LOGD("startTimerId  TIMER_UNLOCK_VEHICLE(%d) duration(CONFIG_DOOR_UNLOCK_TIMEOUT):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_SECURE_VEHICLE:
            //duration = REMOTE_VEHICLE_SECURE_TIMEOUT;
            duration = getUint32fromConfig(CONFIG_REMOTE_VEHICLE_SECURE_TIMEOUT);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, REMOTE_VEHICLE_SECURE_TIMEOUT);
                duration = REMOTE_VEHICLE_SECURE_TIMEOUT;
            }
            m_secureVehicleTimer->setDurationMs(duration, 0);
            m_secureVehicleTimer->start();
            LOGD("startTimerId  TIMER_SECURE_VEHICLE(%d) duration(CONFIG_REMOTE_VEHICLE_SECURE_TIMEOUT):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_ALARM_RESET:
            //duration = REMOTE_VEHICLE_ALARM_RESET_TIMEOUT;
            duration = getUint32fromConfig(CONFIG_REMOTE_VEHICLE_ALARM_RESET_TIMEOUT);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, REMOTE_VEHICLE_ALARM_RESET_TIMEOUT);
                duration = REMOTE_VEHICLE_ALARM_RESET_TIMEOUT;
            }
            m_alarmResetTimer->setDurationMs(duration, 0);
            m_alarmResetTimer->start();
            LOGD("startTimerId  TIMER_ALARM_RESET(%d) duration(RvcAlarmResetExecutionTimer):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_TCU_AUTH_SEQ:
            //duration = 15; //temporary value: see JLR_TCU_Auth_AE1.doc 4.2 Time Out
            m_TCUauthSeqTimer->setDuration(RVC_BCM_AUTHTIME, 0);
            m_TCUauthSeqTimer->start();
            LOGD("startTimerId  TIMER_TCU_AUTH_SEQ(%d) duration:%d bRestart:%d ", timerId, RVC_BCM_AUTHTIME, bRestart);
            break;
        case MyTimer::TIMER_PARK_CLIMATE_START:
            //duration = getUint16fromConfig(RPC_STARTUP_TIME);
            duration = getUint16fromConfig(CONFIG_PARK_CLIMATE_STARTUP_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, PARK_CLIMATE_STARTUP_TIME);
                duration = PARK_CLIMATE_STARTUP_TIME;
            }
            m_parkClimateStartTimer->setDuration(duration, 0);
            m_parkClimateStartTimer->start();
            LOGD("startTimerId  TIMER_PARK_CLIMATE_START(%d) duration(ClimateControlParkClimateStartupTime_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_PARK_CLIMATE_STOP:
            //duration = getUint16fromConfig(RPC_SHUTDOWN_TIME);
            duration = getUint16fromConfig(CONFIG_PARKHEATVENTTIME_SAMPLE_WAIT_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, 4);
                duration = 4;
            }
            m_parkClimateStopTimer->setDuration(duration, 0);
            m_parkClimateStopTimer->start();
            LOGD("startTimerId  TIMER_PARK_CLIMATE_STOP(%d) duration(ClimateControlParkClimateShutdownTime_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_PARKHEATVENT_WAIT:
            duration = PARKHEATVENTTIME_SAMPLE_WAIT_TIME;
            m_RPCheatVentWaitTimer->setDurationMs(duration, 0);
            m_RPCheatVentWaitTimer->start();
            LOGD("startTimerId  TIMER_PARKHEATVENT_WAIT(%d) duration(PARKHEATVENTTIME_SAMPLE_WAIT_TIME):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_RISM_SEAT_READY:
            duration = getUint8fromConfig(RISM_SEAT_READY_TIME);
            m_RISMseatReadyTimer->setDuration(duration, 0);
            m_RISMseatReadyTimer->start();
            LOGD("startTimerId  TIMER_RISM_SEAT_READY(%d) duration(config %s):%d bRestart:%d ", timerId, RISM_SEAT_READY_TIME, duration, bRestart);
            break;
        case MyTimer::TIMER_RISM_SEAT_MOVE:
            duration = getUint8fromConfig(RISM_SEAT_MOVE_TIME);
            m_RISMseatMoveTimer->setDuration(duration, 0);
            m_RISMseatMoveTimer->start();
            LOGD("startTimerId  TIMER_RISM_SEAT_MOVE(%d) duration(config %s):%d bRestart:%d ", timerId, RISM_SEAT_MOVE_TIME, duration, bRestart);
            break;
        case MyTimer::TIMER_RISM_CANCEL_MOVE:
            //duration = RISM_CANCEL_MOVE_TIME;
            //m_RISMcancelMoveTimer->setDuration(duration, 0);
            //m_RISMcancelMoveTimer->start();
            LOGD("startTimerId  TIMER_RISM_CANCEL_MOVE(%d) duration(RISM_CANCEL_MOVE_TIME):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_MIL_FAULT_DETECT:
            duration = MIL_FAULT_DETECTION_TIME;
            m_MILfaultDetectTimer->setDuration(duration, 0); //getCCF()
            m_MILfaultDetectTimer->start();
            LOGD("startTimerId  TIMER_MIL_FAULT_DETECT(%d) duration(MIL_FAULT_DETECTION_TIME):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_REG_SVT_STATUS:
            duration = 3;  // 3sec: temporary time
            m_RegSVTstatusTimer->setDuration(duration, 0);
            m_RegSVTstatusTimer->start();
            LOGD("startTimerId  TIMER_REG_SVT_STATUS(%d) duration:%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_SKIP_WARMUP_VALIABLE:
            duration = 4; // temporary time
            m_skipWarmupVariableTimer->setDuration(duration, 0);
            m_skipWarmupVariableTimer->start();
            LOGD("startTimerId  TIMER_SKIP_WARMUP_VALIABLE(%d) duration:%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN:
            //duration = PARK_CLIMATE_SHUTDOWN_TIME;
            duration = getUint16fromConfig(CONFIG_PARKHEATVENTTIME_SAMPLE_WAIT_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, PARK_CLIMATE_SHUTDOWN_TIME);
                duration = PARK_CLIMATE_SHUTDOWN_TIME;
            }
            m_parkClimateShutdownTimer->setDuration(duration, 0);
            m_parkClimateShutdownTimer->start();
            LOGD("startTimerId  TIMER_PARK_CLIMATE_SHUTDOWN(%d) duration(ClimateControlParkClimateShutdownTime_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_PSIM_LOCK:
            duration = getUint16fromConfig(REMOTE_VEHICLE_SECURE_PSIM_LOCK_TIMER);
            if(duration == 0)
                duration = 120;  //default value
            m_PSIMLockTimer->setDuration(duration, 0);
            m_PSIMLockTimer->start();
            LOGD("startTimerId  TIMER_PSIM_LOCK(%d) duration(REMOTE_VEHICLE_SECURE_PSIM_LOCK_TIMER):%d bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT:
            //duration = PARK_CLIMATE_MIN_STATUS_WAIT_TIME;
            duration = getUint16fromConfig(CONFIG_PARK_CLIMATE_MIN_STATUS_WAIT_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, PARK_CLIMATE_MIN_STATUS_WAIT_TIME);
                duration = PARK_CLIMATE_MIN_STATUS_WAIT_TIME;
            }
            m_parkClimateMinStatusWaitTimer->setDurationMs(duration, 0);
            m_parkClimateMinStatusWaitTimer->start();
            LOGD("startTimerId  TIMER_PARK_CLIMATE_MIN_STATUS_WAIT(%d) duration(ClimateControlCanReadDelay_ms):%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;
        case MyTimer::TIMER_UPDATE_RES_REMAINING:
            {
                //duration = getUint16fromConfig(RVC_RES_RUNMAX);
                //m_updateResRemainingTimer->setDuration((uint32_t)duration, 60); // update every 1min
                duration = 60; // update every 1min
                m_updateResRemainingTimer->setDuration(duration, 0);
                m_updateResRemainingTimer->start();
                LOGD("startTimerId  TIMER_UPDATE_RES_RMAINING (%d) duration:%d cyclicTimeout:60 bRestart:%d ", timerId, duration, bRestart);
            }
            break;
        case MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP:
            duration = getUint16fromConfig(CONFIG_PARKHEATVENTTIME_SAMPLE_WAIT_TIME);
            if(duration <= 0)
            {
                LOGE("duration ERROR %d -> %d ", duration, PARKHEATVENTTIME_SAMPLE_WAIT_TIME);
                duration = PARKHEATVENTTIME_SAMPLE_WAIT_TIME;
            }
            m_parkClimateShutdownTimerForFfhStop->setDuration(duration, 0);
            m_parkClimateShutdownTimerForFfhStop->start();
            LOGD("startTimerId  TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP(%d) duration(ClimateControlParkClimateShutdownTime_s):%d bRestart:%d ", timerId, duration, bRestart);
            break;

        case MyTimer::TIMER_RISM_RELEASE_KEEPPOWER:
            duration = RVC_NGTP_UDP_RETRY_CNT * RVC_NGTP_UDP_RETRY_DELAY;
            m_RISMupdateReleaseTimer->setDurationMs(duration, 0);
            m_RISMupdateReleaseTimer->start();
            LOGD("startTimerId  TIMER_RISM_RELEASE_KEEPPOWER(%d) duration:%d(ms) bRestart:%d ", timerId, duration, bRestart);
            break;

        case MyTimer::TIMER_KEEPPOWER_RELEASE:
            m_keepPowerReleaseTimer->setDuration(MAX_KEEPPOWER_TIME, 0);
            m_keepPowerReleaseTimer->start();
            LOGD("startTimerId  TIMER_KEEPPOWER_RELEASE(%d) duration:%d bRestart:%d ", timerId, MAX_KEEPPOWER_TIME, bRestart);
            break;

        case MyTimer::TIMER_PARK_CLIMATE_STATUS_TIMEOUT:
            duration = PARK_CLIMATE_STARTUP_TIME;
            m_parkClimateStatusTimeout->setDuration(duration, 0);
            m_parkClimateStatusTimeout->start();
            LOGD("startTimerId  TIMER_PARK_CLIMATE_STATUS_TIMEOUT (%d) duration:%d bRestart:%d ", timerId, duration, bRestart);
            break;

        case MyTimer::TIMER_RPC_STATUS_WAIT:    //FALL-THROUGH
        case MyTimer::TIMER_NGTP_RETRY:         //FALL-THROUGH
        default:
            LOGD("startTimerId not implemented!! timerId:%d ",timerId);
            break;
    }
    setTimerStatus(timerId, true);
//    LOGD("startTimerId  timerId:%d duration:%d bRestart:%d ", timerId, duration, bRestart);
}

void RemoteServiceApp::stopTimerId(uint32_t timerId)
{
    LOGD("stopTimerId  timerId:%d ", timerId);
    if(!isTimerOn(timerId))
    {
        LOGV("[%d] timer not active state", timerId);
        return;
    }

    setTimerStatus(timerId, false);

    switch(timerId)
    {
        case MyTimer::TIMER_RES_START:
            m_RESstartTimer->stop();
            break;
        case MyTimer::TIMER_RES_STOP:
            m_RESstopTimer->stop();
            break;
        case MyTimer::TIMER_RES_PREREQ:
            m_RESprereqTimer->stop();
            break;
        case MyTimer::TIMER_FFH_MAX_RUN:
            #if 0
            m_FFHmaxRunTimer->stop();
            #else
            m_alarmManager->cancel(m_CallbackAlarmListener);
            #endif
            break;
        case MyTimer::TIMER_RES_MAX_RUN:
            m_RESmaxRunTimer->stop();
            break;
        case MyTimer::TIMER_CLIMATE_REQ:
            m_ParkClimateReqTimer->stop();
            break;
        case MyTimer::TIMER_FFH_USER_STOP:
            m_FFHuserIrqTimer->stop();
            break;
        case MyTimer::TIMER_UNLOCK_VEHICLE:
            m_unlockVehicleTimer->stop();
            break;
        case MyTimer::TIMER_SECURE_VEHICLE:
            m_secureVehicleTimer->stop();
            break;
        case MyTimer::TIMER_ALARM_RESET:
            m_alarmResetTimer->stop();
            break;
        case MyTimer::TIMER_TCU_AUTH_SEQ:
            m_TCUauthSeqTimer->stop();
            break;
        case MyTimer::TIMER_PARK_CLIMATE_START:
            m_parkClimateStartTimer->stop();
            break;
        case MyTimer::TIMER_PARK_CLIMATE_STOP:
            m_parkClimateStopTimer->stop();
            break;
        case MyTimer::TIMER_RPC_STATUS_WAIT:
            m_RPCstatusWaitTimer->stop();
            break;
        case MyTimer::TIMER_PARKHEATVENT_WAIT:
            m_RPCheatVentWaitTimer->stop();
            break;
        case MyTimer::TIMER_RISM_SEAT_READY:
            m_RISMseatReadyTimer->stop();
            break;
        case MyTimer::TIMER_RISM_SEAT_MOVE:
            m_RISMseatMoveTimer->stop();
            break;
        case MyTimer::TIMER_NGTP_RETRY:
            m_NgtpRetryTimer->stop();
            break;
        case MyTimer::TIMER_RISM_CANCEL_MOVE:
            m_RISMcancelMoveTimer->stop();
            break;
        case MyTimer::TIMER_MIL_FAULT_DETECT:
            m_MILfaultDetectTimer->stop();
            break;
        case MyTimer::TIMER_REG_SVT_STATUS:
            m_RegSVTstatusTimer->stop();
            break;
        case MyTimer::TIMER_SKIP_WARMUP_VALIABLE:
            m_skipWarmupVariableTimer->stop();
            break;
        case MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN:
            m_parkClimateShutdownTimer->stop();
            break;
        case MyTimer::TIMER_PSIM_LOCK:
            m_PSIMLockTimer->stop();
            break;
        case MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT:
            m_parkClimateMinStatusWaitTimer->stop();
            break;
        case MyTimer::TIMER_UPDATE_RES_REMAINING:
            m_updateResRemainingTimer->stop();
            break;
        case MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP:
            m_parkClimateShutdownTimerForFfhStop->stop();
            break;
        case MyTimer::TIMER_RISM_RELEASE_KEEPPOWER:
            m_RISMupdateReleaseTimer->stop();
            break;
        case MyTimer::TIMER_KEEPPOWER_RELEASE:
            m_keepPowerReleaseTimer->stop();
            break;
        case MyTimer::TIMER_PARK_CLIMATE_STATUS_TIMEOUT:
            m_parkClimateStatusTimeout->stop();
            break;
        default:
            LOGD("stopTimerId not implemented!! timerId:%d ",timerId);
            break;
    }
}


void RemoteServiceApp::setTimerStatus(uint32_t timerId, bool bSet)
{
    if(timerId >= MyTimer::TIMER_END_NUM)
    {
        LOGE("setTimerStatus ERROR  timerId:%d ", timerId);
        return;
    }
    timerStatus[timerId] = bSet;
}

void RemoteServiceApp::initTimerStatus()
{
    LOGE("initTimerStatus");
    for(int i = 0 ; i < MyTimer::TIMER_END_NUM ; i++ )
    {
        timerStatus[i] = false;
    }
}

bool const RemoteServiceApp::isTimerOn(uint32_t timerId)
{
    if(timerId >= MyTimer::TIMER_END_NUM)
    {
        LOGE("isTimerOn ERROR  timerId:%d ", timerId);
        return false;
    }
    return timerStatus[timerId];
}
