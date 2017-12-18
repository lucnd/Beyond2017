/**
* \file    RemoteServiceApp.cpp
* \brief     Implementation of RemoteServiceApp
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

#define LOG_TAG "RVC"
#include "Log.h"

#include "RemoteServiceApp.h"
#include "RemoteService_RES.h"
#include "RemoteService_RCC.h"
#include "RemoteService_RPC.h"
#include "RemoteService_RVI.h"
#include "RemoteService_RISM.h"

android::sp<Application> gApp;


const char* propertyList[SLDD_REQ_MAX][SLDD_PROP_MAX] = {
//RVI
    {"RDUPrePowermode",   "RDUPreResStatus",   "RDUAuthStatus",   "RDUSuccess",   "RDUFailReason",
      NULL, NULL, NULL, NULL},
    {"RDLPrePowermode",   "RDLPreResStatus",   "RDLAuthStatus",   "RDLSuccess",   "RDLFailReason",
      NULL, NULL, NULL, NULL},
    {"ALOFFPrePowermode", "ALOFFPreResStatus", "ALOFFAuthStatus", "ALOFFSuccess", "ALOFFFailReason",
      NULL, NULL, NULL, NULL},
    {"HBLFPrePowermode",  "HBLFPreResStatus",  "HBLFAuthStatus",  "HBLFSuccess",  "HBLFFailReason",
      NULL, NULL, NULL, NULL},
//RES
    {"REONPrePowermode",  "REONPreResStatus",  "REONAuthStatus",  "REONSuccess",  "REONFailReason",
     "REONPreSVTstatus",  "REONPreFuellevel",  "REONPreCrashStatus",  "REONPreResRuntime"},
    {"REOFFPrePowermode", "REOFFPreResStatus", "REOFFAuthStatus", "REOFFSuccess", "REOFFFailReason",
     "REOFFPreSVTstatus", "REOFFPreFuellevel", "REOFFPreCrashStatus", "REOFFPreResRuntime"}
    //{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
};


RemoteServiceApp::RemoteServiceApp(){
    LOGE("RemoteServiceApp()");

    m_RESstartTimer = NULL;
    m_RESstopTimer = NULL;
    m_RESprereqTimer = NULL;
    m_FFHmaxRunTimer = NULL;
    m_RESmaxRunTimer = NULL;
    m_ParkClimateReqTimer = NULL;
    m_FFHuserIrqTimer = NULL;
    m_unlockVehicleTimer = NULL;
    m_secureVehicleTimer = NULL;
    m_alarmResetTimer = NULL;
    m_TCUauthSeqTimer = NULL;
    m_parkClimateStartTimer = NULL;
    m_parkClimateStopTimer = NULL;
    m_RPCstatusWaitTimer = NULL;
    m_RPCheatVentWaitTimer = NULL;
    m_RISMseatReadyTimer = NULL;
    m_RISMseatMoveTimer = NULL;
    m_NgtpRetryTimer = NULL;
    m_RISMcancelMoveTimer = NULL;
    m_MILfaultDetectTimer = NULL;
    m_RegSVTstatusTimer = NULL;
    m_skipWarmupVariableTimer = NULL;
    m_parkClimateShutdownTimer = NULL;
    m_PSIMLockTimer = NULL;
    m_parkClimateMinStatusWaitTimer = NULL;
    m_updateResRemainingTimer = NULL;
    m_parkClimateShutdownTimerForFfhStop = NULL;
    m_RISMupdateReleaseTimer = NULL;
    m_keepPowerReleaseTimer = NULL;
    mKeepPowerCnt = 0;
    mScheduledWakeUpSecond = 0;
    mCanVariant = 0;
}


RemoteServiceApp::~RemoteServiceApp(){
    LOGE("~RemoteServiceApp()");
}


void RemoteServiceApp::onCreate()
{
    LOGE("RemoteServiceApp onCreate()");

    //mHandler = new MyHandler(looper(), *this);
    mLooper = SLLooper::myLooper();
    if(mHandler == NULL){
        mHandler = new MyHandler(mLooper, *this);
    }

    createTimer();
    initTimerStatus();

    m_remoteService[RS_RES] = new RemoteService_RES(*this);
    m_remoteService[RS_RCC] = new RemoteService_RCC(*this);
    m_remoteService[RS_RPC] = new RemoteService_RPC(*this);
    m_remoteService[RS_RVI] = new RemoteService_RVI(*this);
    m_remoteService[RS_RISM] = new RemoteService_RISM(*this);
    m_remoteServiceLog = new RemoteService_log();

    setCurrParkClimateReqTCU(RPC_REQ_PARK_DEFAULT);//reqDefaultParkClimate();
    setParkClimateModeWhenReqEngineHeating(-1);
    setCurrBCMauthService(RS_SERVICE_TYPE_MAX);
    setAllServiceState(RS_READY);

    if(m_debugMgrReceiver == NULL)
        m_debugMgrReceiver = new DebugMgrReceiver(*this);

    if(m_vifReceiver == NULL)
        m_vifReceiver = new VifReceiver(*this);

    if(m_ngtpReceiver == NULL)
        m_ngtpReceiver = new NgtpReceiver(*this);

    if(m_powerStateReceiver == NULL)
        m_powerStateReceiver = new PowerStateReceiver(*this);

    if(m_powerModeReceiver == NULL)
        m_powerModeReceiver = new PowerModeReceiver(*this);

    if(m_canBusStateReceiver == NULL)
        m_canBusStateReceiver = new CanBusStateReceiver(*this);

    if(m_diagReceiver == NULL)
        m_diagReceiver = new DiagMgrReceiver(*this);

    if(m_configReceiver == NULL)
        m_configReceiver = new ConfigurationReceiver(*this);

    if(m_TelephonyListener == NULL)
        m_TelephonyListener = new TelephonyListener(*this);

    if(m_TsimLock == NULL)
        m_TsimLock = new TelephonyManager::TsimLock();

    if(m_PsimLock == NULL)
        m_PsimLock = new TelephonyManager::PsimLock();

    if(m_CallbackAlarmListener == NULL)
        m_CallbackAlarmListener = new CallbackAlarmExpireListener(*this);

    registerReceiver();
    setCurrPowerMode(m_powerMgrService->getCurrentPowerMode());

    mCanVariant = getCanVariant();

// move forward    setAllServiceState(RS_READY);
//    initRISMServiceState();

    setDBGcmdActiveStatus(false);
    setSLDDcmdActiveStatus(false);
    setSVTactive(false);
    setSubscriptSVT(false);
    setRESstopReason(RES_STOP_MAX);
    //initRESremainingTime();
    updateFromConfig_sunroofExist();
    updateFromConfig_movingWindows();

    setRDUngtpTimeSec(0);
    setRDLngtpTimeSec(0);
    setHBLFngtpTimeSec(0);
    setALOFFngtpTimeSec(0);
    setRDUngtpTimeMil(0);
    setRDLngtpTimeMil(0);
    setHBLFngtpTimeMil(0);
    setALOFFngtpTimeMil(0);

    setRDUTsimLockState(false);
    setRESTsimLockState(false);

    asn1_set_error_handling(asn1_legacy_error_handler);

    registerSVTpostReceiver();

    setVehicleStatusInit();

    updateFromProperty_RPCvariables();

    setDoorOpenStatus(63);
    updateFromCan_DoorStatusHS();

    setHFSCommand(false);
    updateFromCan_HFSCommand();

    setHRWCommand(false);
    updateFromCan_HRWCommand();

    setCurrRemoteStartStatus(0);
    updateFromCan_RemoteStartStatusMS();

    setCurrAlarmStatus(ALARM_IGNORE);
    updateFromCan_AlarmModeHS();

    setRESSequenceStatus(0);

// int for TF2 logging
    setRESWindowStatus(LOG_NOT_AVAILABLE);
    setRESVehicleLockStatus(LOG_NOT_AVAILABLE);
    setRESAlarmStatus(LOG_NOT_AVAILABLE);
    setRESHazardSwitchStatus(LOG_NOT_AVAILABLE);
    setRESBrakePedalStatus(LOG_NOT_AVAILABLE);
    setRESVehStationaryStatus(LOG_NOT_AVAILABLE);
    setRESTransmissionStatus(LOG_NOT_AVAILABLE);
    setRESEngineRunningStatus(LOG_NOT_AVAILABLE);
    setLogserviceType(LOG_NOT_AVAILABLE);
    setLogfuelLevel(LOG_NOT_AVAILABLE);
    setLogTransistionFromREStoNormal(LOG_NOT_AVAILABLE);
    setLogPrerequisiteCheckRequestStatus(LOG_NOT_AVAILABLE);
    setLogPowerModeWhenRequestReceived(LOG_NOT_AVAILABLE);
    setLogffhStatusWhenFFHStopped(getFFHOperatingStatus());
    setLogRESSequenceStatusPrevious(0); // unknown
    setLogFfhRunTimer(true);

    mErrorIdx = 0;
    initNGTPerrorCode();
#ifdef KEEP_POWER
    mKeepPowerCnt = 0;
#endif

    if(m_remoteServiceLog->readLogfromFile() == false )
    {
        LOGE("TF2 logging file not exist");

        m_remoteServiceLog->setLogIndex(0);
        for(int i = 0 ; i < LOG_INDEX_MAX ; i++ )
        {
            m_remoteServiceLog->initLogData(i);
        }
        m_remoteServiceLog->writeLogToFile();
        m_remoteServiceLog->setLogIndex(LOG_INDEX_MAX -1);
    }
    else
    {
        m_remoteServiceLog->findLogIndex();
    }

    if(RemoteService_IsCanBusWaking() == true)
    {
        onNotifyFromCANBusStateMgr(1);
    }

    if((getCurrRemoteStartStatus() == true)
        && (RemoteService_getPowerMode() == PM_RUNNING_2))
    { // to recovery RES running status from crash
        LOGE("RES looks like running.. start timer to recover remaining time");
        startTimerId(MyTimer::TIMER_RES_MAX_RUN);
        startTimerId(MyTimer::TIMER_UPDATE_RES_REMAINING);
        RemoteService_requestHandler(RS_RCC, RS_TARGET_TEMP);
    }
}


void RemoteServiceApp::onDestroy()
{
    LOGE("RemoteServiceApp onDestroy()");

    m_remoteServiceLog->writeLogToFile();

    setAllServiceState(RS_NOT_READY);
    unregisterReceiver();
    initTimerStatus();
    deleteTimer();
    if(m_remoteService[RS_RES] != NULL)
        delete m_remoteService[RS_RES];
    if(m_remoteService[RS_RCC] != NULL)
        delete m_remoteService[RS_RCC];
    if(m_remoteService[RS_RPC] != NULL)
        delete m_remoteService[RS_RPC];
    if(m_remoteService[RS_RVI] != NULL)
        delete m_remoteService[RS_RVI];
    if(m_remoteService[RS_RISM] != NULL)
        delete m_remoteService[RS_RISM];
    if(m_remoteServiceLog != NULL)
        delete m_remoteServiceLog;
    ReadyToDestroy();
}


void RemoteServiceApp::MyHandler::handleMessage(const sp<sl::Message>& msg)
{
    const int32_t what = msg->what;

//    LOGE("MyHandler hanleMessage: %d", what);

    sp<Buffer> buf = new Buffer();
    buf->setTo(msg->buffer.data(), msg->buffer.size());

    switch(what)
    {
        case RECV_MSG_FROM_MCU:
        {
            mApplication.RemoteService_receiveMessageFromMCU(msg->arg1, (uint8_t*)buf->data(), buf->size());
        }
            break;

        case RECV_MSG_FROM_DM:
        {
            LOGE("[RECV_MSG_FROM_DM] arg1:0x%x arg2:0x%x arg3:0x%x", msg->arg1, msg->arg2, msg->arg3);
            mApplication.RemoteService_receiveMessageFromDM(msg->arg1, msg->arg2, msg->arg3);
        }
            break;

        case RECV_MSG_FROM_HMI:
        {
            if(msg->arg1 == HMI_TYPE_TRIGGER_VEHICLE_UNLOCK)   //Temporary Trigger for V0.95
            {
                if(/*msg->arg2 == HMI_TRIGGER_ACTION_UNLOCK &&*/ mApplication.getCurrState(RS_UNLOCK_VEHICLE) == RS_READY)
                {
                    LOGE("[REQUEST] VEHICLE UNLOCK !!");
                    mApplication.RemoteService_receiveMessageFromTSP(RS_UNLOCK_VEHICLE);
                }
            }
            else if(msg->arg1 == HMI_TYPE_AIRBACK_DETECTED && msg->arg2 == HMI_TYPE_DETECTED)
            {
                LOGV("[RECV_MSG_FROM_HMI] AIRBAG CRASH DETECTED");
                mApplication.setProperty(STR_CURR_CRASH, STR_TRUE);
            }
        }
            break;

        case RECV_MSG_FROM_TSP: {
            InternalNGTPData *request = (InternalNGTPData*)msg->buffer.data();
            LOGE("[RECV_MSG_FROM_TSP] serviceType:%d messageType:%d creationTime_seconds:%d creationTime_millis:%d ", request->serviceType, request->messageType ,request->creationTime_seconds, request->creationTime_millis);
            if(request->messageType == dspt::downServiceData)
            {
                if(request->serviceType == NGTP_ST_RISM)
                {
                    if(mApplication.isRISMAppEnabled() == true)
                    {
                        bool ret = mApplication.decodeNGTPSvcData(request->encodedSvcData, request->encodedSize);

                        if(ret)
                            mApplication.startRequestService(request->serviceType, request->creationTime_seconds, request->creationTime_millis);
                        else
                            LOGE("handleMessage RISM decode fail.");
                    }
                    else
                    {
                        LOGE("handleMessage RISM DO NOT take action : AppModeRISM NOT enabled");
                    }
                }
                else if(request->serviceType == NGTP_ST_SCHEDULEDWAKUP)
                {
                    bool ret = mApplication.decodeNGTPSvcData_scheduledWakeup(request->encodedSvcData, request->encodedSize);

                    if(ret)
                        mApplication.startRequestService(request->serviceType, request->creationTime_seconds, request->creationTime_millis);
                    else
                        LOGE("handleMessage scheduled wakeup decode fail.");
                }
                else
                    mApplication.startRequestService(request->serviceType, request->creationTime_seconds, request->creationTime_millis);
            }
            else if((request->messageType == dspt::dsptAck) && (request->serviceType == NGTP_ST_RISM))
            {
                //RISM_Update message dsptAck
                if((request->creationTime_seconds == mApplication.getRISMUpdateNgtpTimeSec()) && (request->creationTime_millis == mApplication.getRISMUpdateNgtpTimeMil()))
                {
                    LOGV("[RECV_MSG_FROM_TSP] dsptAck for RISM_Update message");
                    //mApplication.setRISMUpdateNgtpTimeSec(0);
                    //mApplication.setRISMUpdateNgtpTimeMil(0);
                    mApplication.stopTimerId(MyTimer::TIMER_RISM_RELEASE_KEEPPOWER);
                    mApplication.RvcKeepPowerRelease();
                }
            }
            else
               ;
        } break;

        case RECV_MSG_FROM_PM:
        {
            //int32_t newPowerState = msg->arg1;
            //LOGE("[RECV_MSG_FROM_PM] POWER MODE CHANGED !!");
            // if newPowerState ==
            // ignition off -> writeNVM Fuellevel, crash status,
            // if RemoteStartStatus == RES Runtime
            mApplication.RemoteService_receiveMessageFromPM(msg->arg1);
        }
            break;

        case RECV_MSG_FROM_DIAG:
        {
            mApplication.RemoteService_receiveMessageFromDiag(msg->arg1, msg->arg2, msg->arg3, buf);
        }
            break;

        case RECV_MSG_FROM_CONFIG:
            mApplication.RemoteService_receiveMessageFromConfig(msg);
            break;

        //case RECV_MSG_PSIM_LOCK_STATE_CHANGED:  //FALL-THROUGH
        //case RECV_MSG_TSIM_LOCK_STATE_CHANGED:  //FALL-THROUGH
        case RECV_MSG_ACTIVESIM_STATE_CHANGED:
            mApplication.RemoteService_receiveMessageFromTelephony(what, msg->arg1);
            break;

        case RECV_MSG_ALARM_EXPIRED:
            mApplication.RemoteService_receiveMessageFromCallbackAlarmExpireListener(what, msg->arg1);
            break;

        case RECV_MSG_CAN_BUS_STATE:
            mApplication.RemoteService_receiveMessageFromCanBusStateReceiver(what, msg->arg1);
            break;

        case RECV_MSG_FROM_MCU_FRAME:
            mApplication.RemoteService_receiveMessageFromMCUFrame(what, msg->arg1);
            break;

        default:
            break;
    }
}


void RemoteServiceApp::RemoteService_receiveBCMauthStatus(RS_ServiceType reqType, uint8_t* buf, uint32_t size)
{
    uint8_t seqStatus = buf[0];

    LOGI("receiveBCMauthStatus reqType:%d BCM authentication:%s", reqType, ((seqStatus == BCM_AUTH_OK) ? STR_SUCCESS : STR_FAIL));

    if((reqType != RS_SEAT_MOVE_INIT) && (reqType != RS_SEAT_MOVE_COMPLETE))
    {
        if(reqType == RS_FFH_PREREQ)
            setRVCproperty(RS_ENGINE_START, NULL, (RVCproperty)PROP_AUTH, (seqStatus == BCM_AUTH_OK) ? STR_SUCCESS : STR_FAIL);
        else
            setRVCproperty(reqType, NULL, (RVCproperty)PROP_AUTH, (seqStatus == BCM_AUTH_OK) ? STR_SUCCESS : STR_FAIL);
    }

    stopTimerId(MyTimer::TIMER_TCU_AUTH_SEQ);

    if(seqStatus == BCM_AUTH_OK)
    {
        RemoteService_receiveBCMauthStatus_ok(reqType, buf, size);
    }
    else
    {
        RemoteService_receiveBCMauthStatus_ng(reqType, buf, size);
    }
    setCurrBCMauthService(RS_SERVICE_TYPE_MAX);
}

void RemoteServiceApp::RemoteService_receiveBCMauthStatus_ok(RS_ServiceType reqType, uint8_t* buf, uint32_t size)
{
    switch(reqType) {
        case RS_ENGINE_START: {
            setRVCproperty(reqType, NULL, (RVCproperty)PROP_SUCCESS, STR_SUCCESS);
            startTimerId(MyTimer::TIMER_RES_START);
            startTimerId(MyTimer::TIMER_RES_PREREQ, true); // to check BCM PreReq Check PM0
        } break;

        case RS_ENGINE_STOP: {
            if(isTimerOn(MyTimer::TIMER_RES_MAX_RUN))  //for 1 minute less case
                RemoteService_recalcurateResRunMax();
            setRVCproperty(reqType, NULL, (RVCproperty)PROP_SUCCESS, STR_SUCCESS);
            //sendAckMsgToTSP(reqType, svc::actionResponse);  //sendAckMessage
            startTimerId(MyTimer::TIMER_RES_STOP);
            setCurrState(RS_ENGINE_STOP, RS_WAIT_RESPONSE);
        } break;

        case RS_FFH_PREREQ: {
            setRVCproperty(RS_ENGINE_START, NULL, (RVCproperty)PROP_SUCCESS, STR_SUCCESS);
            startTimerId(MyTimer::TIMER_RES_PREREQ, true);
        } break;

        case RS_UNLOCK_VEHICLE: {
            sendAckMsgToTSP(reqType, svc::acknowledge);  //sendAckMessage
            startTimerId(MyTimer::TIMER_UNLOCK_VEHICLE);
            setCurrState(reqType, RS_WAIT_RESPONSE);
        } break;

        case RS_SECURE_VEHICLE: {
            sendAckMsgToTSP(reqType, svc::acknowledge);  //sendAckMessage
            startTimerId(MyTimer::TIMER_SECURE_VEHICLE);
            setCurrState(reqType, RS_WAIT_RESPONSE);
        } break;

        case RS_ALARM_RESET: {
            sendAckMsgToTSP(reqType, svc::acknowledge);  //sendAckMessage
            startTimerId(MyTimer::TIMER_ALARM_RESET);
            setCurrState(reqType, RS_WAIT_RESPONSE);
        } break;

        case RS_HONK_FLASH: {
            sp<Buffer> buf = new Buffer();
            uint8_t *payload = new uint8_t;
            payload[0] = DBG_BCM_AUTH_SUCCESS;

            setRVCproperty(reqType, NULL, (RVCproperty)PROP_SUCCESS, STR_SUCCESS);
            if(getDBGcmdActiveStatus()) {
                buf->setTo(payload, 0x01);
                RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_LOCATION, DBG_CMD_TYPE_REQUEST, buf);
                setDBGcmdActiveStatus(false);
            } else {
                sendAckMsgToTSP(RS_HONK_FLASH, svc::actionResponse);
            }
            setCurrState(reqType, RS_READY);
            delete payload;
            // does not need other TCU feedback.
        } break;

        case RS_SEAT_MOVE_INIT: {
            startTimerId(MyTimer::TIMER_RISM_SEAT_READY);
            sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, true);
            //setCurrState(RS_RISM, RS_WAIT_RESPONSE);
        } break;

        case RS_SEAT_MOVE_COMPLETE: {
            stopTimerId(MyTimer::TIMER_RISM_SEAT_MOVE);
            sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::actionResponse, true);  //request type check and Nack message check.
            resetSeatMoveReq();
            setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE->READY
        } break;

        default:
            break;
    }
}

void RemoteServiceApp::RemoteService_receiveBCMauthStatus_ng(RS_ServiceType reqType, uint8_t* buf, uint32_t size)
{
    uint8_t seqStatus = buf[0];
    sp<Buffer> resp = new Buffer();
    uint8_t *dbgResult = new uint8_t;
    dbgResult[0] = DBG_BCM_AUTH_FAIL;
    uint8_t dbgCmd[] = { DBG_CMD_VEHICLE_UNLOCK, DBG_CMD_VEHICLE_SECURE, DBG_CMD_ALARM_RESET, DBG_CMD_VEHICLE_LOCATION };

    if((reqType != RS_SEAT_MOVE_INIT) && (reqType != RS_SEAT_MOVE_COMPLETE))
    {
        if(reqType == RS_FFH_PREREQ) {
            setRVCproperty(RS_ENGINE_START, NULL, (RVCproperty)PROP_SUCCESS, STR_FAIL);
            setRVCproperty(RS_ENGINE_START, NULL, (RVCproperty)PROP_CAUSE, STR_BCM_FAIL);
        } else {
            setRVCproperty(reqType, NULL, (RVCproperty)PROP_SUCCESS, STR_FAIL);
            setRVCproperty(reqType, NULL, (RVCproperty)PROP_CAUSE, STR_BCM_FAIL);
        }
    }

    LOGE("receiveBCMauthStatus reqType:%d BCM authentication fail cause:%d", reqType, seqStatus);

    setCurrState(reqType, RS_READY);

    if(getDBGcmdActiveStatus() && (reqType >= RS_UNLOCK_VEHICLE && reqType <= RS_HONK_FLASH)) {
        resp->setTo(dbgResult, 0x01);
        RemoteService_sendDBGcmdResponse(dbgCmd[reqType], DBG_CMD_TYPE_REQUEST, resp);
        setDBGcmdActiveStatus(false);
        delete dbgResult;
        return;
    }

    switch(reqType)
    {
        case RS_UNLOCK_VEHICLE: //FALL-THROUGH
        case RS_SECURE_VEHICLE: //FALL-THROUGH
        case RS_ALARM_RESET:    //FALL-THROUGH
        case RS_HONK_FLASH: {
            setNGTPerrorCode(executionFailure_chosen);
            sendNackMsgToTSP(reqType);
        } break;

        case RS_ENGINE_START: {
            if(IsVehicleWithFFH()) // RES after FFH
            {
                RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_FAILURE);
            }
            else // RES WITHOUT FFH
            {
                setNGTPerrorCode(executionFailure_chosen);
                sendNackMsgToTSP(reqType);
            }
        } break;

        case RS_ENGINE_STOP: {
            if(isTimerOn(MyTimer::TIMER_RES_MAX_RUN))  //for 1 minute less case
                RemoteService_recalcurateResRunMax();
            if(getRESstopReason() == RES_STOP_TSP)
                RemoteService_askUpdateMessageToRVM(REMOTE_ENGIEN_STOP_FAILURE);
        } break;

        case RS_FFH_PREREQ: {
            setNGTPerrorCode(executionFailure_chosen);
            sendNackMsgToTSP(RS_ENGINE_START);
        } break;

        case RS_SEAT_MOVE_INIT:
        {
            sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, executionFailure_chosen); // JLRTCU-2366
            sendNackMsgToTSP_RISM(RS_RISM_MOVE, executionFailure_chosen);
            setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE->READY
        }
        break;

        case RS_SEAT_MOVE_COMPLETE:
        {
            stopTimerId(MyTimer::TIMER_RISM_SEAT_MOVE);
            sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, executionFailure_chosen); // JLRTCU-2366
            sendNackMsgToTSP_RISM(RS_RISM_MOVE, executionFailure_chosen);
            resetSeatMoveReq();
            setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE->READY
        }
        break;

        default:
            break;
    }
    delete dbgResult;
}

void RemoteServiceApp::onNotifyFromMCU(uint16_t Sigid, sp<Buffer>& buf)
{
    //LOGE("onNotifyFromMCU Sigid:%d", Sigid);

    sp<sl::Message> message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_MCU, Sigid);

    if(buf->size() > 0)
    {
        message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

void RemoteServiceApp::onNotifyFromMCUFrame(uint16_t FrameId, sp<vifCANFrameSigData>& FrameSigData)
{
    mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_MCU_FRAME, FrameId)->sendToTarget();
}

void RemoteServiceApp::onNotifyFromTSP(InternalNGTPData *data, uint32_t size)
{
    //LOGV("onNotifyFromTSP");
    if(data != NULL)
    {
        sp<sl::Message> messsage = mHandler->obtainMessage(
                MyHandler::RECV_MSG_FROM_TSP, data->serviceType, data->creationTime_seconds, data->encodedSize);

        messsage->buffer.setTo((uint8_t*)data, sizeof(InternalNGTPData));
        messsage->sendToTarget();
    }
    else {
        LOGE("FATAL:NGTP Data is NULL");
    }
}

void RemoteServiceApp::onNotifyFromPowerStateMgr(int32_t newState, int32_t reason)
{
    LOGI("onNotifyFromPowerStateMgr() newState:%d reason:%d", newState, reason);
    switch(newState)
    {
/*        case PS_SLEEP:
            break;
            */
        case PS_NORMAL:
            //startTimerId(MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT);
            break;
        default:
            break;
    }
}

void RemoteServiceApp::onNotifyFromPowerModeMgr(int8_t newMode)
{
    //LOGI("onNotifyFromPowerModeMgr");
    mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_PM, newMode)->sendToTarget();
}

void RemoteServiceApp::onNotifyFromCANBusStateMgr(int8_t newState)
{
    //LOGI("onNotifyFromCANBusStateMgr newMode:%d", newState);
    mHandler->obtainMessage(MyHandler::RECV_MSG_CAN_BUS_STATE, newState)->sendToTarget();
}

void RemoteServiceApp::onHMIReceived(const uint32_t type, const uint32_t action)
{
    LOGE("onHMIReceived type:0x%02x, action:0x%02x", type, action);
    mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_HMI, type, action)->sendToTarget();
}

void RemoteServiceApp::onPostReceived(const sp<Post>& post)
{
    LOGV("onPostReceived what:%d arg1:%d", post->what, post->arg1);
    uint16_t postRequest = (uint16_t) post->arg1;
    RS_ServiceType requestServiceType = (RS_ServiceType)(postRequest & SLDD_REQ_MASK);

    if(post->what == RVC_SLDD_COMMAND)
    {
        if( (SLDD_RISM_START <= postRequest) && (postRequest <= SLDD_RISM_END) )
        {
            onPostReceived_slddRism(post);
        }
        else if(postRequest >= SLDD_REQ_RDU && postRequest <= SLDD_REQ_RH_HEAT)
        {
            setSLDDcmdActiveStatus(true);

            LOGE("postRequest:%d serviceType: %d", postRequest, requestServiceType);
            if(postRequest >= SLDD_REQ_RDU && postRequest <= SLDD_REQ_REOFF)
                 initSLDDstatusProperty(requestServiceType);
            if( (SLDD_REQ_RH_AUTO <= postRequest) && (postRequest <= SLDD_REQ_RH_HEAT) )
                initSLDDstatusProperty_RH();
            if(requestServiceType == RS_ENGINE_STOP)
                setRESstopReason(RES_STOP_SLDD);
            RemoteService_receiveMessageFromTSP(requestServiceType);
        }
        else if(postRequest >= SLDD_RESET_RDU && postRequest <= SLDD_RESET_REOFF)
        {
            initSLDDstatusProperty(requestServiceType);
        }
        else if(SLDD_RESET_RH == postRequest)
        {
            initSLDDstatusProperty_RH();
        }
        else if(postRequest == SLDD_RESET_RVC)
        {
            resetRVC();
        }
        else
        {
            LOGE("receive unknown post arg1:%d", post->arg1);
        }
    }
    else if(post->what == (appid_t)APP_ID_SVT)
    {
        if(getUint8fromConfig(APP_MODE_SVT) == STATUS_ENABLE)
        {
            setSubscriptSVT(true);
        }
        else
        {
            setSubscriptSVT(false);
        }
        if((post->arg1 == RES_SVT_ALERT_STATUS) || (post->arg1 == RES_SVT_ALARM_STATUS))
        {
            setSVTactive(true);
        }
        else
        {
            setSVTactive(false);
        }
    }
    else
        LOGV("ignore unknown post message what:%d arg1:%d", post->what, post->arg1);
}

void RemoteServiceApp::onPostReceived_slddRism(const sp<Post>& post)
{

    switch(post->arg1)
    {
        case SLDD_REQ_FOLD:
            foldRequestBySldd(post->arg2);
            return;
        case SLDD_REQ_UNFOLD:
            unfoldRequestBySldd(post->arg2);
            return;
        default:
            LOGI("onPostReceived_slddRism not implemented cmd.. post->arg1:%d ", post->arg1);
            break;
    }
}

void RemoteServiceApp::onNotifyFromDebugMgr(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
{
    LOGE("onNotifyFromDebugMgr id:0x%x cmd:0x%x cmd2:0x%x", id, cmd, cmd2);

    sp<sl::Message> message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_DM, id, cmd, cmd2);

    if(buf->size() > 0)
    {
        message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

void RemoteServiceApp::onNotifyFromDiagMgr(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
{
    sp<sl::Message> message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_DIAG, id, cmd, cmd2);

    if(buf->size() > 0)
    {
        message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

void RemoteServiceApp::onNotifyFromConfiguration(sp<Buffer>& buf)
{
    sp<sl::Message> message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_CONFIG);

    if(buf->size() > 0)
    {
        message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

void RemoteServiceApp::onPsimLockStateChanged(bool locked)
{
    LOGV("onPsimLockStateChanged : %d", locked);
    mHandler->obtainMessage(MyHandler::RECV_MSG_PSIM_LOCK_STATE_CHANGED, locked)->sendToTarget();
}

void RemoteServiceApp::onTsimLockStateChanged(bool locked)
{
    LOGV("onTsimLockStateChanged : %d", locked);
    mHandler->obtainMessage(MyHandler::RECV_MSG_TSIM_LOCK_STATE_CHANGED, locked)->sendToTarget();
}

void RemoteServiceApp::onActiveSimStateChanged(int32_t activeSim)
{
    LOGV("onActiveSimStateChanged : %d", activeSim);
    mHandler->obtainMessage(MyHandler::RECV_MSG_ACTIVESIM_STATE_CHANGED, activeSim)->sendToTarget();
}

void RemoteServiceApp::onAlarmExpired(const int32_t param)
{
    LOGV("onAlarmExpired : %d ", param );
    mHandler->obtainMessage(MyHandler::RECV_MSG_ALARM_EXPIRED, param)->sendToTarget();
//    m_myTimer->handlerFunction(MyTimer::TIMER_FFH_MAX_RUN);
}


//RS_ServiceState RemoteServiceApp::RemoteService_getServiceState(){

//    return  RS_NOT_READY;
//}

bool RemoteServiceApp::RemoteService_getRemoteStartStatus()
{
    bool remoteStartStatus = getCurrRemoteStartStatus();
    LOGE("RemoteService_getRemoteStartStatus():%d", remoteStartStatus);
    return remoteStartStatus;
}

RS_PowerMode RemoteServiceApp::RemoteService_getPowerMode()
{
    // Read Engine power mode.
    RS_PowerMode powerMode = (RS_PowerMode)m_powerMgrService->getCurrentPowerMode();
    LOGE("RemoteService_getPowerMode():%d", powerMode);
    return powerMode;
}

int8_t RemoteServiceApp::RemoteService_getCANBusState()
{
    // 1:wakeup 0:sleep -1:unknoen
    int8_t canBusState = m_powerMgrService->getCurrentCANBusState();
    LOGE("RemoteService_getCANBusState():%d", canBusState);
    return canBusState;
}

bool RemoteServiceApp::RemoteService_IsCanBusWaking()
{
    if(RemoteService_getCANBusState() == 1)
    {
        return true;
    }
    return false;
}

void RemoteServiceApp::RemoteService_DBGcmdFailResponse(RS_ServiceType serviceType)
{
    LOGE("RemoteService_DBGcmdFailResponse()");

    sp<Buffer> resp = new Buffer();
    uint8_t *dbgResult = new uint8_t;

    dbgResult[0] = DBG_RESP_FAIL;
    resp->setTo(dbgResult, 0x01);

    switch(serviceType)
    {
        case RS_UNLOCK_VEHICLE:
        {
            RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_UNLOCK, DBG_CMD_TYPE_REQUEST, resp);
        }
            break;

        case RS_SECURE_VEHICLE:
        {
            RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_SECURE, DBG_CMD_TYPE_REQUEST, resp);
        }
            break;

        case RS_ALARM_RESET:
        {
            RemoteService_sendDBGcmdResponse(DBG_CMD_ALARM_RESET, DBG_CMD_TYPE_REQUEST, resp);
        }
            break;

        case RS_HONK_FLASH:
        {
            RemoteService_sendDBGcmdResponse(DBG_CMD_VEHICLE_LOCATION, DBG_CMD_TYPE_REQUEST, resp);
        }
            break;

        default:
            break;
    }
    delete dbgResult;
}

void RemoteServiceApp::RemoteService_sendDBGcmdResponse(uint8_t cmd, uint8_t cmd2, sp<Buffer> &buf)
{
    LOGE("RemoteService_sendDBGcmdResponse()");

    int32_t id = getCurrDBGcmdId();
    m_debugMgrService->responseData(id, cmd, cmd2, buf);
}

error_t RemoteServiceApp::RemoteService_reqSendCANdata(uint8_t nSig, sp<vifCANContainer> &outdata)
{
    LOGV("RemoteService_reqSendCANdata(PowerState:%d)", m_powerMgrService->getCurrentPowerState());
#if 0
    ///[Sungwoo.oh] Temporary solution to improve stability of Remote Service
    LOGE("RemoteService_reqSendCANdata(pm:%d)", m_powerMgrService->getCurrentPowerState());
    if(m_powerMgrService->getCurrentPowerState() != 2/**NORMAL*/)
        usleep(6*1000*1000);
    //else
        //usleep(3*1000*1000);
    ///[Sungwoo.oh] END
#endif
    uint8_t *sigData = outdata->buf->data();
    uint8_t retryMax = 12;
    uint16_t sigId = 0;
    error_t result = E_OK;

    sigId |= ((uint16_t) sigData[2] << 8);
    sigId |= sigData[1];

    LOGI("RemoteService_reqSendCANdata() Signal id:%d value[0]:0x%x value[1]:0x%x", sigId, sigData[3], sigData[4]);

    //TX CAN signal retry 3times (interval 3seconds)
    do {
        if((m_powerMgrService->getCurrentPowerState() != PS_NORMAL) || (result != E_OK))
            usleep(3*1000*1000);

        result = m_vifManager->send_CANdataToMicom(nSig, outdata);
        retryMax--;
        LOGI("TX CAN Send result:0x%x retryMax:%d getCurrentPowerState():%d", result, retryMax, m_powerMgrService->getCurrentPowerState());
    } while((result != E_OK) && (retryMax != INIT_VALUE_ZERO));

    if(result != E_OK)
    {
        //if need: send Nack message
        LOGE("CAN signal %d SEND FAIL. Error cause:0x%x (PowerState:%d)", sigId, result, m_powerMgrService->getCurrentPowerState());
    }
    return result;
}


void RemoteServiceApp::reqBCMauthentication(RS_ServiceType service)
{
    LOGV("reqBCMauthentication()");

    // request BCM authentication
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    setCurrBCMauthService(service);

    buf->setData(Signal_Internal_Serv_BCM_AUTH_REQ, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_Internal_Serv_BCM_AUTH_REQ), 0x00);
    result = RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    if(result == E_OK)
    {
        LOGV("serviceType:%d BCM auth request success", service);
        startTimerId(MyTimer::TIMER_TCU_AUTH_SEQ);
    }
    else
    {
        uint8_t buf[CAN_VALUES_MAX_BUF_SIZE] = {0,};
        buf[0] = BCM_AUTH_TIME_OUT;
        LOGE("serviceType:%d BCM auth request fail", service);
        RemoteService_receiveBCMauthStatus_ng(service, buf, 1);
        setCurrBCMauthService(RS_SERVICE_TYPE_MAX);
    }
}


void RemoteServiceApp::RemoteService_requestHandler(RS_RemoteService rs_service, RS_ServiceType serviceType)
{
    switch (rs_service)
    {
        case RS_RES:    m_remoteService[RS_RES]->requestHandler(serviceType);   break;
        case RS_RCC:    m_remoteService[RS_RCC]->requestHandler(serviceType);   break;
        case RS_RPC:    m_remoteService[RS_RPC]->requestHandler(serviceType);   break;
        case RS_RVI:    m_remoteService[RS_RVI]->requestHandler(serviceType);   break;
        case RS_RISM:   m_remoteService[RS_RISM]->requestHandler(serviceType);  break;
        default: break;
    }
}
void RemoteServiceApp::RemoteService_responseHandler(RS_RemoteService rs_service, RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size)
{
    switch (rs_service)
    {
        case RS_RES:    m_remoteService[RS_RES]->responseHandler(remoteServiceType, sigId, buf, size);   break;
        case RS_RCC:    m_remoteService[RS_RCC]->responseHandler(remoteServiceType, sigId, buf, size);   break;
        case RS_RPC:    m_remoteService[RS_RPC]->responseHandler(remoteServiceType, sigId, buf, size);   break;
        case RS_RVI:    m_remoteService[RS_RVI]->responseHandler(remoteServiceType, sigId, buf, size);   break;
        case RS_RISM:   m_remoteService[RS_RISM]->responseHandler(remoteServiceType, sigId, buf, size);  break;
        default: break;
    }
}

void RemoteServiceApp::setCurrParkClimateModeTCU(int8_t status)
{
    uint8_t currParkClimateReqTCU = getCurrParkClimateReqTCU();
    LOGV("setCurrParkClimateModeTCU() parkClimateModeTCU:%d, parkClimateReqTCU:%d",
        status, currParkClimateReqTCU);

    setPrevParkClimateModeTCU(getCurrParkClimateModeTCU());

    switch(status)
    {
        case CLIMATE_MODE_OFF:
            mCurrParkClimateModeTCU = status;
            if(currParkClimateReqTCU == RPC_REQ_PARK_STOP)
            {
                reqDefaultParkClimate();
            }
            break;
        case CLIMATE_MODE_STARTUP:      //FALL-THROUGH
        case CLIMATE_ENGINE_HEATING:    //FALL-THROUGH
        case CLIMATE_CABIN_HEATING:     //FALL-THROUGH
        case CLIMATE_CABIN_VENTING:
            mCurrParkClimateModeTCU = status;
            if( (currParkClimateReqTCU == RPC_REQ_PARK_VENT)
               ||(currParkClimateReqTCU == RPC_REQ_PARK_HEAT)
               ||(currParkClimateReqTCU == RPC_REQ_PARK_AUTO)
               ||(currParkClimateReqTCU == RES_ENGINE_HEAT_REQUEST) )
            {
                reqDefaultParkClimate();
            }
            break;
        default:
            mCurrParkClimateModeTCU = -1;
            break;
    }
    saveProperty_RPCvariables(Signal_ParkClimateMode_RX, mCurrParkClimateModeTCU);
}
void RemoteServiceApp::setTimer1Time(uint32_t time)
{
    mTimer1Time = time;

    mTimer1Month = (time >> 24) & 0xFF;
    mTimer1Day = (time >> 16) & 0xFF;
    mTimer1Hour = (time >> 8) & 0xFF;
    mTimer1Minute = time & 0xFF;

    if(mTimer1Minute > 59)
        mTimer1Minute = 0;

    if(mTimer1Hour > 23)
        mTimer1Hour = 0;

    if(mTimer1Day > 31)
        mTimer1Day = 0;

    if(mTimer1Month > 12)
        mTimer1Month = 0;

    LOGV("setTimer1Time() Month:%d Day:%d Hour:%d Min:%d", mTimer1Month, mTimer1Day, mTimer1Hour, mTimer1Minute);
}

void RemoteServiceApp::setTimer2Time(uint32_t time)
{
    mTimer2Time = time;

    mTimer2Month = (time >> 24) & 0xFF;
    mTimer2Day = (time >> 16) & 0xFF;
    mTimer2Hour = (time >> 8) & 0xFF;
    mTimer2Minute = time & 0xFF;

    if(mTimer2Minute > 59)
        mTimer2Minute = 0;

    if(mTimer2Hour > 23)
        mTimer2Hour = 0;

    if(mTimer2Day > 31)
        mTimer2Day = 0;

    if(mTimer2Month > 12)
        mTimer2Month = 0;

    LOGV("setTimer2Time() Month:%d Day:%d Hour:%d Min:%d", mTimer2Month, mTimer2Day, mTimer2Hour, mTimer2Minute);
}

void RemoteServiceApp::setAllServiceState(RS_ServiceState state)
{
    #if 0
    setCurrState(RS_ENGINE_START, state);
    setCurrState(RS_ENGINE_STOP, state);
    setCurrState(RS_PARK_CLIMATE_AUTO, state);
    setCurrState(RS_PARK_CLIMATE_STOP, state);
    setCurrState(RS_PARK_CLIMATE_VENT, state);
    setCurrState(RS_PARK_CLIMATE_HEAT, state);
    setCurrState(RS_UNLOCK_VEHICLE, state);
    setCurrState(RS_SECURE_VEHICLE, state);
    setCurrState(RS_ALARM_RESET, state);
    setCurrState(RS_HONK_FLASH, state);
    setCurrState(RS_TARGET_TEMP, state);
    setCurrState(RS_FFH_PREREQ, state);
    setCurrState(RS_FFH_START, state);
    setCurrState(RS_FFH_STOP, state);
    #else
    for(int i = 0 ; i < RS_SERVICE_TYPE_MAX ; i++ )
    {
        setCurrState((RS_ServiceType)i, state);
    }
    #endif
}
/*
void RemoteServiceApp::initRISMServiceState()
{
    if(getUint8fromConfig(APP_MODE_RISM) == STATUS_DISABLE)
        setRISMServiceState(RS_NOT_READY);
    else
        setRISMServiceState(RS_READY);
}

void RemoteServiceApp::setRISMServiceState(RS_ServiceState state)
{
    setCurrState(RS_RISM_CANCEL, state);
    setCurrState(RS_RISM_MOVE, state);
}
*/
void RemoteServiceApp::registerReceiver()
{
    LOGV("registerReceiver");
    if(m_debugMgrService == NULL) {
        m_debugMgrService = interface_cast<IDebugManagerService>(defaultServiceManager()->getService(String16(DebugManagerService::getServiceName())));
        m_debugMgrService->attachReceiver((appid_t)APP_ID_REMOTESERVICE, m_debugMgrReceiver);
    }
    if(m_powerMgrService == NULL) {
        m_powerMgrService = interface_cast<IPowerManagerService>(defaultServiceManager()->getService(String16(PowerManagerService::getServiceName())));
    }
    if(m_diagManager == NULL) {
        m_diagManager = interface_cast<IDiagManagerService>(defaultServiceManager()->getService(String16(DiagManagerService::getServiceName())));

        m_diagManager->attachReceiver((appid_t)APP_ID_REMOTESERVICE, m_diagReceiver);
    }
    if(m_appManager == NULL)
        m_appManager = interface_cast<IApplicationManagerService>(defaultServiceManager()->getService(String16(ApplicationManagerService::getServiceName())));

    if(m_configManager == NULL)
    {
        m_configManager = interface_cast<IConfigurationManagerService>(defaultServiceManager()->getService(String16(ConfigurationManagerService::getServiceName())));
    }
    if(m_dataService == NULL)
    {
        m_dataService = interface_cast<IDataService>(defaultServiceManager()->getService(String16(DataService::getServiceName())));
    }
    if(m_alarmManager == NULL)
    {
        m_alarmManager = interface_cast<IAlarmManagerService>(defaultServiceManager()->getService(String16(AlarmManagerService::getServiceName())));
    }
    //register NGTP receiver
    if(m_ngtpManager == NULL)
    {
        m_ngtpManager = interface_cast<INGTPManagerService>(defaultServiceManager()->getService(String16(NGTPManagerService::getServiceName())));
    }
    //register vifManager receiver
    if(m_vifManager == NULL)
    {
        m_vifManager = interface_cast<IvifManagerService>(defaultServiceManager()->getService(String16(vifManagerService::getServiceName())));
    }

    m_powerMgrService->registerPowerStateReceiver(m_powerStateReceiver, 0);
    m_powerMgrService->registerPowerModeReceiver(m_powerModeReceiver);
    m_powerMgrService->registerCANBusStateReceiver(m_canBusStateReceiver, false);

    m_configManager->registerReceiver(COMMON_CONFIG_FILE, RCC_TARGET_TEMP, m_configReceiver);
    m_configManager->registerReceiver(COMMON_CONFIG_FILE, APP_MODE_SVT, m_configReceiver);
    m_configManager->registerReceiver(COMMON_CONFIG_FILE, APP_MODE_RISM, m_configReceiver);
    m_configManager->registerReceiver(COMMON_CONFIG_FILE, CONFIG_CONVENIENCE_FOLD, m_configReceiver);
    m_configManager->registerReceiver(COMMON_CONFIG_FILE, CONFIG_VEHICLE_TYPE, m_configReceiver);
    m_configManager->registerReceiver(COMMON_CONFIG_FILE, CONFIG_INTELLIGENT_SEAT_FOLD, m_configReceiver);
    m_configManager->registerReceiver(COMMON_CONFIG_FILE, RES_FUEL_LEVEL_MIN, m_configReceiver);

    TelephonyManager::listen(m_TelephonyListener, PhoneStateListener::LISTEN_PSIM_LOCK_STATE, false);
    TelephonyManager::listen(m_TelephonyListener, PhoneStateListener::LISTEN_TSIM_LOCK_STATE, false);
    TelephonyManager::listen(m_TelephonyListener, PhoneStateListener::LISTEN_ACTIVE_SIM_STATE, false);

    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_RDU);   //remote door unlock:19
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_RDL);   //remote door lock:20
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_HBLF);  //horn blow light flash:21
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_ALOFF); //remote alarm reset:37
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_REON);   //remote engine start:22
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_REOFF);  //remote engine stop:23
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_RHON);   //remote park climate start:39
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_RHOFF);  //remote part climate stop:40
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_RISM);  //remote Intelligent Seat Movement:135
    m_ngtpManager->registerReceiver(m_ngtpReceiver, NGTP_ST_SCHEDULEDWAKUP);  //scheduled wakeup, holiday mode:134

    m_vifManager->registerReceiver(Signal_Internal_Serv_BCM_AUTH_RESP, false, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DoorStatusHS_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DistanceToEmpty_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_OdometerMasterValue_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_ODODisplayedMiles_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_FuelGaugePosition_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_Internal_Serv_CAN_VARIANT, true, m_vifReceiver);

    //for RES
    m_vifManager->registerReceiver(Signal_CrashStatusRCM_RX, true, m_vifReceiver);  //1bit use, 0: No Crash 1: Crash
//    m_vifManager->registerReceiver(Signal_FuelLevelIndicatedHS_RX, true, m_vifReceiver);
//    m_vifManager->registerReceiver(Signal_FuelLevelIndicatedQFHS_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_RemoteStartStatusMS_RX, true, m_vifReceiver);
    //m_vifManager->registerReceiver(Signal_RESVehStationaryStatus_RX, true, m_vifReceiver);  //1bit use, 0: not stationary 1: stationary
    //m_vifManager->registerReceiver(Signal_RESTransmissionStatus_RX, true, m_vifReceiver);  //1bit use, 0: not park 1: transmission in park
    m_vifManager->registerReceiver(Signal_EngOBDMil_RX, true, m_vifReceiver);  //1bit use, 0:MIL off, 1:MIL on
    //m_vifManager->registerReceiver(Signal_RESWindowStatus_RX, true, m_vifReceiver);
    //m_vifManager->registerReceiver(Signal_RESVehicleLockStatus_RX, true, m_vifReceiver);
    //m_vifManager->registerReceiver(Signal_RESAlarmStatus_RX, true, m_vifReceiver);
    //m_vifManager->registerReceiver(Signal_RESHazardSwitchStatus_RX, true, m_vifReceiver);
    //m_vifManager->registerReceiver(Signal_RESBrakePedalStatus_RX, true, m_vifReceiver);
    //m_vifManager->registerReceiver(Signal_RESSequenceStatus_RX, false, m_vifReceiver);

    //for RCC
    m_vifManager->registerReceiver(Signal_FrontSystemOnCmd_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HFSCommand_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HRWCommand_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatFLModeRequest_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatFLRequest_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatFRModeRequest_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatFRRequest_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatRLModeReq_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatRLReq_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatRRModeReq_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_HeatedSeatRRReq_RX, true, m_vifReceiver);

    //for RPC
    m_vifManager->registerReceiver(Signal_ParkClimateMode_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_ParkHeatVentTime_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_FFHOperatingStatus_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_TimerActivationStatus_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_Timer1Time_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_Timer2Time_RX, true, m_vifReceiver);

    //for RVI
    m_vifManager->registerReceiver(Signal_DoorLatchStatus_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_AlarmModeHS_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DriverWindowPosition_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_PassWindowPosition_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_RearDriverWindowPos_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_RearPassWindowPos_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DriverRearWindowPos_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_PassRearWindowPos_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_SunroofOpen_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_RoofPositionStatus_RX, true, m_vifReceiver);

/* use frame noti   //for RISM
    m_vifManager->registerReceiver(Signal_DrvSeatInhib2ndRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_PasSeatInhib2ndRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DrvSeatInhib3rdRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_PasSeatInhib3rdRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DrvSeatPosition2ndRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_PasSeatPosition2ndRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_DrvSeatPosition3rdRow_RX, true, m_vifReceiver);
    m_vifManager->registerReceiver(Signal_PasSeatPosition3rdRow_RX, true, m_vifReceiver);
*/
    // to habdle Signal_FuelLevelIndicatedHS_RX, Signal_FuelLevelIndicatedQFHS_RX signals
    m_vifManager->registerReceiverFrame(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_M_RX, m_vifReceiver);
    m_vifManager->registerReceiverFrame(SignalFrame_MULTI_e::SignalFrame_IPC_PT_E_RX, m_vifReceiver);
    // to handle ResRelevantsignals
    m_vifManager->registerReceiverFrame(SignalFrame_PMZ_e::SignalFrame_BCM_PMZ_F_RX, m_vifReceiver);
    m_vifManager->registerReceiverFrame(SignalFrame_MULTI_e::SignalFrame_BCM_PT_G_RX, m_vifReceiver);
    // to handle seat status
    m_vifManager->registerReceiverFrame(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AI_RX, m_vifReceiver);
    m_vifManager->registerReceiverFrame(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AS_RX, m_vifReceiver);
}

void RemoteServiceApp::unregisterReceiver()
{
    LOGV("unregisterReceiver");
    if(m_ngtpManager != NULL)
    {
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_RDU);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_RDL);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_HBLF);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_ALOFF);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_REON);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_REOFF);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_RHON);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_RHOFF);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_RISM);
        m_ngtpManager->unregisterReceiver(m_ngtpReceiver, NGTP_ST_SCHEDULEDWAKUP);
    }

    if(m_vifManager != NULL)
    {
        m_vifManager->unregisterReceiver(Signal_Internal_Serv_BCM_AUTH_RESP, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DoorStatusHS_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DistanceToEmpty_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_OdometerMasterValue_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_ODODisplayedMiles_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_FuelGaugePosition_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_CrashStatusRCM_RX, m_vifReceiver);
//        m_vifManager->unregisterReceiver(Signal_FuelLevelIndicatedHS_RX, m_vifReceiver);
//        m_vifManager->unregisterReceiver(Signal_FuelLevelIndicatedQFHS_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_RemoteStartStatusMS_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESVehStationaryStatus_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESTransmissionStatus_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_EngOBDMil_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESWindowStatus_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESVehicleLockStatus_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESAlarmStatus_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESHazardSwitchStatus_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESBrakePedalStatus_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_FrontSystemOnCmd_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HFSCommand_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HRWCommand_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatFLModeRequest_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatFLRequest_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatFRModeRequest_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatFRRequest_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatRLModeReq_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatRLReq_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatRRModeReq_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_HeatedSeatRRReq_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_ParkClimateMode_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_ParkHeatVentTime_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_FFHOperatingStatus_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_TimerActivationStatus_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_Timer1Time_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_Timer2Time_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DoorLatchStatus_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_AlarmModeHS_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DriverWindowPosition_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_PassWindowPosition_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_RearDriverWindowPos_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_RearPassWindowPos_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DriverRearWindowPos_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_PassRearWindowPos_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_SunroofOpen_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_RoofPositionStatus_RX, m_vifReceiver);
        //m_vifManager->unregisterReceiver(Signal_RESSequenceStatus_RX, m_vifReceiver);
        /* use frame noti
        m_vifManager->unregisterReceiver(Signal_DrvSeatInhib2ndRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_PasSeatInhib2ndRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DrvSeatInhib3rdRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_PasSeatInhib3rdRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DrvSeatPosition2ndRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_PasSeatPosition2ndRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_DrvSeatPosition3rdRow_RX, m_vifReceiver);
        m_vifManager->unregisterReceiver(Signal_PasSeatPosition3rdRow_RX, m_vifReceiver);
        */
        m_vifManager->unregisterReceiver(Signal_Internal_Serv_CAN_VARIANT, m_vifReceiver);

        m_vifManager->unregisterReceiverFrame(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_M_RX, m_vifReceiver);
        m_vifManager->unregisterReceiverFrame(SignalFrame_MULTI_e::SignalFrame_IPC_PT_E_RX, m_vifReceiver);
        m_vifManager->unregisterReceiverFrame(SignalFrame_PMZ_e::SignalFrame_BCM_PMZ_F_RX, m_vifReceiver);
        m_vifManager->unregisterReceiverFrame(SignalFrame_MULTI_e::SignalFrame_BCM_PT_G_RX, m_vifReceiver);
        m_vifManager->unregisterReceiverFrame(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AI_RX, m_vifReceiver);
        m_vifManager->unregisterReceiverFrame(SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AS_RX, m_vifReceiver);
    }

    if(m_configManager != NULL)
    {
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, RCC_TARGET_TEMP, m_configReceiver);
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, APP_MODE_SVT, m_configReceiver);
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, APP_MODE_RISM, m_configReceiver);
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, CONFIG_CONVENIENCE_FOLD, m_configReceiver);
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, CONFIG_VEHICLE_TYPE, m_configReceiver);
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, CONFIG_INTELLIGENT_SEAT_FOLD, m_configReceiver);
        m_configManager->unregisterReceiver(COMMON_CONFIG_FILE, RES_FUEL_LEVEL_MIN, m_configReceiver);
    }

    if (m_powerMgrService != NULL)
    {
        m_powerMgrService->unregisterPowerStateReceiver(m_powerStateReceiver);
        m_powerMgrService->unregisterPowerModeReceiver(m_powerModeReceiver);
        m_powerMgrService->unregisterCANBusStateReceiver(m_canBusStateReceiver);
    }
}

void RemoteServiceApp::registerSVTpostReceiver()
{
    uint8_t appStatus = (uint8_t) m_configManager->get_int(COMMON_CONFIG_FILE, CONFIG_NAME_VAL, APP_MODE_SVT);
    LOGV("registerSVTpostReceiver SVT status subscript:%d appStatus:%d ", getSubscriptSVT(), appStatus);

    if((appStatus == STATUS_ENABLE) && !getSubscriptSVT())
    {
        sp<Post> post = Post::obtain((appid_t)APP_ID_REMOTESERVICE);
        m_appManager->sendPost((appid_t)APP_ID_SVT, post);

        startTimerId(MyTimer::TIMER_REG_SVT_STATUS);
    }
}

uint32_t RemoteServiceApp::get_NGTP_time()
{
    time_t timer;
    struct tm y2k = {0};
    uint32_t seconds = 0;

    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

    time(&timer);  /* get current time; same as: timer = time(NULL)  */

    seconds = difftime(timer,mktime(&y2k));

    return seconds;
}

void RemoteServiceApp::createRISMUpdateNgtpTime()
{
    time_t timer;
    struct tm y2k = {0};
    uint32_t seconds;

    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

    time(&timer);  /* get current time; same as: timer = time(NULL)  */

    seconds = difftime(timer,mktime(&y2k));

    struct timeval tm;
    long msec;
    gettimeofday(&tm, NULL);
    msec = tm.tv_usec/1000;

    setRISMUpdateNgtpTimeSec(seconds);
    setRISMUpdateNgtpTimeMil(msec);
}

void RemoteServiceApp::initSLDDstatusProperty(RS_ServiceType serviceType)
{
    uint8_t index;

    LOGV("initSLDDstatusProperty serviceType: %d", serviceType);

    if(serviceType >= RS_UNLOCK_VEHICLE && serviceType <= RS_ENGINE_STOP)
    {
        for(index = 0; index < SLDD_PROP_MAX; index++)
        {
            LOGV("setProperty(%s, PROP_NOT_AVAILABLE)", propertyList[serviceType][index]);
            if(propertyList[serviceType][index] != NULL)
                setProperty(propertyList[serviceType][index], STR_NOT_AVAILABLE);
        }
    }
}

void RemoteServiceApp::initSLDDstatusProperty_RH()
{
    setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_REQUEST, (RVCproperty)PROP_INVALID, STR_NOT_AVAILABLE);
    setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PREREQUSITE, (RVCproperty)PROP_INVALID, STR_NOT_AVAILABLE);
    setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, STR_NOT_AVAILABLE);
}

void RemoteServiceApp::setRVCproperty(RS_ServiceType serviceType, char* key, RVCproperty prop, const char* value)
{
    /*
    -. 각 모듈이 가질 수 있는 property 갯수 : 64개(8Kbytes)
    -. Key string max length : 32 bytes
    -. Value string max length : 92 bytes
    */

    if(serviceType == RS_SERVICE_TYPE_MAX)
    {
        setProperty(key, value);
    }
    else
    {
        if(serviceType >= RS_UNLOCK_VEHICLE && serviceType <= RS_ENGINE_STOP)
        {
            LOGI("save property:%s value:%s", propertyList[serviceType][prop], value);
            if(propertyList[serviceType][0] != NULL)
                setProperty(propertyList[serviceType][prop], value);
        }
    }
}

void RemoteServiceApp::stopRESrunTimer()
{
    if(isTimerOn(MyTimer::TIMER_RES_MAX_RUN))
    {
        RemoteService_recalcurateResRunMax();
    }
    stopTimerId(MyTimer::TIMER_RES_MAX_RUN);
    stopTimerId(MyTimer::TIMER_UPDATE_RES_REMAINING);
}

void RemoteServiceApp::resetRVC()
{
    LOGV("resetRVC");

    //current service state
    for(uint8_t serviceType = RS_UNLOCK_VEHICLE; serviceType <= RS_PARK_CLIMATE_HEAT; serviceType++)
        LOGV("getCurrState service:%d State:%d", serviceType, getCurrState((RS_ServiceType)serviceType));

    if(getCurrRemoteStartStatus())
    {
        LOGV("Can't reset RVC. RES is being operating. Can't reset RES remaining time");
        return;
    }

    //timer init.
    for(uint32_t timerId = 0; timerId < MyTimer::TIMER_END_NUM; timerId++)
        stopTimerId(timerId);

    //RES remaining time reset
    initRESremainingTime();

    //service state init
    setAllServiceState(RS_READY);
//    initRISMServiceState();

    //vehicle status init
    setVehicleStatusInit();

    //PSIM lock release
    if((TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM) && isTimerOn(MyTimer::TIMER_PSIM_LOCK))
    {
        if(RvcPsimLockRelease())
            stopTimerId(MyTimer::TIMER_PSIM_LOCK);
    }

    //TSIM lock release
    if(getRDUTsimLockState())
    {
        if(RvcTsimLockRelease())
            setRDUTsimLockState(false);
    }

    if(getRESTsimLockState())
    {
        if(RvcTsimLockRelease())
            setRESTsimLockState(false);
    }

    //Keep power release
#ifdef KEEP_POWER
    while(mKeepPowerCnt--)
    {
        mKeepPower.release();
        LOGV("KeepPower release count:%d", mKeepPowerCnt);
    }
#endif
    LOGV("resetRVC Done");
}

void RemoteServiceApp::TcuCheckForUserInterruptionDuringEngineHeat()
{
    if(TcuCheckForUserInterruptionDuringEngineHeat_FFHstopByUser() == false )
    {
        startTimerId(MyTimer::TIMER_FFH_USER_STOP, false);
    }
}
bool RemoteServiceApp::TcuCheckForUserInterruptionDuringEngineHeat_FFHstopByUser()
{
    if((getFFHOperatingStatus() == FFH_STOP_BY_USER)
        && (getCurrState(RS_FFH_START) == RS_ACTIVE || getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE) )
    {
        setCurrState(RS_FFH_START, RS_READY);
        stopTimerId(MyTimer::TIMER_CLIMATE_REQ);
        setNGTPerrorCode(executionFailure_chosen);
        sendNackMsgToTSP(RS_ENGINE_START);  //specific reason in climateOperatingStatus field.
        RemoteService_askUpdateMessageToRVM(CLIMATE_USER_INTERRUPTION);

        return true;
    }
    return false;
}

void RemoteServiceApp::stopRESuponLowFuel()
{
    uint16_t resFuelLevelMin = getUint16fromConfig(RES_FUEL_LEVEL_MIN);
    uint16_t fuelLevelIndicated = getFuelLevelIndicated();
    bool remoteStartStatus = getCurrRemoteStartStatus();
    LOGI("stopRESuponLowFuel resFuelLevelMin:%d fuelLevelIndicated:%d remoteStartStatus:%d ",
        resFuelLevelMin, fuelLevelIndicated, remoteStartStatus);

    if((fuelLevelIndicated < resFuelLevelMin) && (remoteStartStatus == true))
    {
        setLogfuelLevel(0x01); //low
        setRESstopReason(RES_STOP_FUELLOW);
        RemoteService_receiveMessageFromTSP(RS_ENGINE_STOP);
    }
}

extern "C" class Application* createApplication() {
    LOGE("createApplication() RemoteServiceApp");

    gApp = new RemoteServiceApp;
    return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
    delete (RemoteServiceApp*)application;
}


