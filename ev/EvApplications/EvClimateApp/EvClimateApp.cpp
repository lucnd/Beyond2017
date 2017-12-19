//EVC Feature Static Class.
#include "EvClimateApp.h"
#include "EvClimateAppHandler.h"
#include "EvClimateAppTimer.h"
#include "EvtUtil.h"
#include "EvServiceManager.h"

#define LOG_TAG "[EVT]EvClimateApp"
#include "Log.h"

#define NOT_AVAIL -1
#define SEND_NGTP(A, B) mHandler->obtainMessage(A, B)->sendToTarget()

EccTimer* t_timer;

EvClimateApp::EvClimateApp()
{
    state = ECC_OFF;
    m_Looper = SLLooper::myLooper();
    mHandler = new EvClimateAppHandler(m_Looper, *this);

    mPreconditionSettings = EvtEVPreconditionSettings::GetInstance();
    mEvVif = EvtVifManager::GetInstance();
    mEvConfig = EvConfigData::GetInstance();
    mEvNgtp = EvtNgtpMsgManager::GetInstance();
}

void EvClimateApp::onStart()
{
    // write Startup code.
    setState(ECC_IDLE);

    ECC_Timers.fill(0);
    t_timer = new EccTimer(*this);
}

void EvClimateApp::initialSetup() // for FIRST Time Setting.
{
    //TODO : 6.4.11 First time setup requirements.
    ;
}

//////////////////// Receive NGTP Message - Precondition Start Scenario. ////////////////////////
bool EvClimateApp::rcv_PreconditionStartReq(uint32_t _targetTemperatureCelsiusX2)
{
    // SRD_ReqECC0006_v1 : Reject all incomming TSP Message.
    if(getState() == ECC_OFF)
    {
        LOGE("[Warnning] ECC is not Actived! -> Ingore TSP Message (SRD_ReqECC0006_v1)");
        return false;
    }

    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));

    //SRD_ReqECC0041, SRD_ReqECC0042
    uint8_t _currentCondition = check_ClimateControlCondition();
    if( _currentCondition != E_OK)
    {
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_START_RESP, _currentCondition);
        return false;
    }

    if(getState() != ECC_IDLE || IsPreconditioningAlreayInProgress() == true)
    {
        LOGE("[ERROR] ECC is already in processing. (Not IDLE state)");
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_START_RESP, svc::ErrorCode::executionFailure_chosen);
        return false;
    }

    LOGV("send ParkClimateReqTCU as 3[Auto Requested]");
    sendCanSignal(Signal_ParkClimateReqTCU_TX, 3); //NOTE: (CAN SPEC) ESIS_TCU_B ParkClimateReqTCU type. 3 - Auto Requested.
    //SRD_ReqECC0040
    if(20 <= _targetTemperatureCelsiusX2 && _targetTemperatureCelsiusX2 <= 83)
    {
        LOGV("send TempTargetTCU as [%d]", _targetTemperatureCelsiusX2);
        sendCanSignal(Signal_TempTargetTCU_TX, _targetTemperatureCelsiusX2);
    }

    setTimer(ECC_CAN_STATUS_IGNORE_TIMER, mEvConfig->getEvCfgData("ECC_CAN_STATUS_IGNORE_TIME"), true);
    setState(ECC_WAIT_CAN_TIMER);

    //BUS HOLE START!!
    EvVehicleStatus::GetInstance()->evKeepCANbus(__func__);

    setTimer(ECC_NM_TIMEOUT_TIMER, mEvConfig->getEvCfgData("ECC_NM_TIMEOUT"), true);

    return true;
}

void EvClimateApp::timeout_ECC_CAN_STATUS_IGNORE_TIMER()
{
    Mutex::Autolock autolock(mLock);
    // move status to 'Listen CAN signal'. ECC_LISTEN_CAN_SIG
    setState(ECC_LISTEN_CAN_SIG);
}

void EvClimateApp::send_NGTP_PreconditionStartResp(int preClimateStatus) // Shoul be Private Method.
{
    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));

    sendCanSignal(Signal_ParkClimateReqTCU_TX, (uint16_t)0); //SRD_ReqECC0055_v1
    cancelTimer(ECC_NM_TIMEOUT_TIMER);
    LOGV("[Cancel Timer] ECC_NM_TIMEOUT_TIMER has canceled!");

    if(getErrorResultFromPreClimateStatus(preClimateStatus) == E_OK)
    {
        setTimer(ECC_DIRECT_PRECONDITION_TIMER, mEvConfig->getEvCfgData("ECC_DIRECT_PRECONDTIME"),false);
        setState(ECC_DIRECT_PRECONDI);
    }
    else
    {
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
    }

    mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_START_RESP, getErrorResultFromPreClimateStatus(preClimateStatus));
    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
}

void EvClimateApp::timeout_ECC_NM_TIMEOUT_TIMER()
{
    Mutex::Autolock autolock(mLock);
    if(getState() == ECC_IDLE)
    {
        return;
    }

    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
    send_NGTP_PreconditionStartResp(mEvVif->queryCanData(Signal_PreClimateStatus_RX));
    //Release BUS
    if(getState() != ECC_DIRECT_PRECONDI)
    {
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
    }
}

void EvClimateApp::timeout_ECC_DIRECT_PRECONDITION_TIMER()
{
    Mutex::Autolock autolock(mLock);
    //No Spec. --> Requested Vrushali 20160719
    //TODO!!
    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
    setState(ECC_IDLE);
}


//////////////////// Receive NGTP Message - Precondition STOP Scenario. ////////////////////////
bool EvClimateApp::rcv_PreconditionStopReq()
{
    // SRD_ReqECC0006_v1 : Reject all incomming TSP Message.
    if(getState() == ECC_OFF)
    {
        LOGE("[Warnning] ECC is not Actived! -> Ingore TSP Message (SRD_ReqECC0006_v1)");
        return false;
    }

    //SRD_ReqECC0020_v1
    if(getState() == ECC_WAIT_PRE_STOP)
    {
        mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_STOP_RESP, svc::ErrorCode::executionFailure_chosen);
        return false;
    }

    if(IsPreconditioningAlreayInProgress() == false)
    {
        mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_STOP_RESP, svc::ErrorCode::climateModeNotCorrect_chosen);
        sendCanSignal(Signal_ParkClimateReqTCU_TX, (uint32_t)0);  //SRD_ReqECC0111_v1
        cancelAllTimer();
        return false;
    }

    LOGV("send ParkClimateReqTCU as 4[Stop Request]");
    sendCanSignal(Signal_ParkClimateReqTCU_TX, 4); //NOTE: (CAN SPEC) ESIS_TCU_B ParkClimateReqTCU type. 4 - Stop Requested.

    cancelAllTimer();
    setTimer(ECC_HVAC_NO_RESP_TIMEOUT_TIMER, mEvConfig->getEvCfgData("ECC_HVAC_NO_RESP_TIMEOUT"));

    setState(ECC_WAIT_PRE_STOP);
    return true;
}

void EvClimateApp::timeout_ECC_HVAC_NO_RESP_TIMEOUT_TIMER()
{
    Mutex::Autolock autolock(mLock);
    if(getState() == ECC_IDLE)
    {
        return;
    }

    sendCanSignal(Signal_ParkClimateReqTCU_TX, (uint32_t)0);  //SRD_ReqECC0107
    if(getState() == ECC_WAIT_PRE_STOP)  // The state is 'waiting ack of precondition stop from hvac.'
    {
        mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_STOP_RESP, svc::ErrorCode::hvacTimeout_chosen);
    }
    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
    setState(ECC_IDLE);
}

bool EvClimateApp::rcv_PreconditionSettingReq(int8_t _fuelFiredHeaterSetting, int8_t _prioritySetting, uint32_t _msgCreationTime)
{
    LOGV("Start ECC Precondition Setting Request!");

    // SRD_ReqECC0006_v1 : Reject all incomming TSP Message.
    if(getState() == ECC_OFF)
    {
        LOGE("[Warnning] ECC is not Actived! -> Ingore TSP Message (SRD_ReqECC0006_v1)");
        return false;
    }

    if(EvVehicleStatus::GetInstance()->getCurPowerMode() >= 7)
    {
        LOGE("[Warnning] Status [%s] : PowerMode is not less then 7. (%d)", EccState[getState()].c_str(), EvVehicleStatus::GetInstance()->getCurPowerMode());
        mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, svc::ErrorCode::conditionsNotCorrect_chosen);
        return false;
    }

    if(getState() != ECC_IDLE || IsPreconditioningAlreayInProgress() == true)
    {
        LOGE("[Warnning] Status [%s] : HAVC is already in preconditioning progress!", EccState[getState()].c_str());
        mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, svc::ErrorCode::conditionsNotCorrect_chosen);
        return false;
    }

    //Store Precondition Setting Reqeust information.
    mEvNgtp->decodeMsg_PreconditionSettings(_fuelFiredHeaterSetting, _prioritySetting, _msgCreationTime);

    //BUS Hold
    EvVehicleStatus::GetInstance()->evKeepCANbus(__func__);

    setTimer(ECC_PRESET_NM_TIMEOUT_TIMER, mEvConfig->getEvCfgData("ECC_NM_TIMEOUT"), true);

    //////////////////////// ASN lib enum value. ////////////////////////////
    // EVPreconditionSettings::fuelFiredHeaterSetting::enabled  = 0
    // EVPreconditionSettings::fuelFiredHeaterSetting::disabled = 1
    // EVPreconditionSettings::prioritySetting::prioritize_comfort = 0
    // EVPreconditionSettings::prioritySetting::prioritize_range   = 1
    /////////////////////////////////////////////////////////////////////////

    //////// CAN SPEC enum value. ////////////
    // 0 = Range Prioritised
    // 1 = Comfort Prioritised
    // 0 = FFH disabled
    // 1 = FFH enabled
    //////////////////////////////////////////

    if(_fuelFiredHeaterSetting != NOT_AVAIL)
    {
        uint8_t _CAN_fuelFiredHeaterSetting = (_fuelFiredHeaterSetting == EVPreconditionSettings::fuelFiredHeaterSetting::enabled) ? 1 /*FFH enabled*/ : 0 /*FFH disabled*/;
        LOGV("[PHEV] Send FFHPreClimReqTCU(%d) and FFHPreClimSetTCU(1)", _CAN_fuelFiredHeaterSetting);
        sendCanSignal(Signal_FFHPreClimSetTCU_TX, (uint32_t)1);
        sendCanSignal(Signal_FFHPreClimReqTCU_TX, (uint32_t)_CAN_fuelFiredHeaterSetting);
    }

    if(_prioritySetting != NOT_AVAIL)
    {
        uint8_t _CAN_prioritySetting = (_prioritySetting == EVPreconditionSettings::prioritySetting::prioritize_comfort) ? 1 /*Comfort Prioritised*/ : 0 /*Range Prioritised*/;
        LOGV("Send PreClimRngCmfReqTCU(%d) PreClimRngCmfSetTCU(1)", _CAN_prioritySetting);
        sendCanSignal(Signal_PreClimRngCmfSetTCU_TX, (uint32_t)1);
        sendCanSignal(Signal_PreClimRngCmfReqTCU_TX, (uint32_t)_CAN_prioritySetting);
    }

    setTimer(ECC_CAN_SETTING_IGNORE_TIMER, mEvConfig->getEvCfgData("ECC_CAN_SETTING_IGNORE_TIME"), true);
    setTimer(ECC_SETTING_TRIGGER_TIMER, mEvConfig->getEvCfgData("ECC_SETTING_TRIGGER_TIME"), true);

    setState(ECC_PRESET_WAIT_CAN);
    return true;
}

bool IsPreconditioningSettingChanged(uint8_t _ASN_Value, uint8_t _CAN_Value)
{
    if(_ASN_Value == 0 && _CAN_Value == 1)
    {
        return true;
    }

    if(_ASN_Value == 1 && _CAN_Value == 0)
    {
        return true;
    }

    return false;
}
void EvClimateApp::timeout_ECC_CAN_SETTING_IGNORE_TIMER()
{
    Mutex::Autolock autolock(mLock);
    if(getState() == ECC_IDLE)
    {
        return;
    }
    //////////////////////// ASN lib enum value. ////////////////////////////
    // EVPreconditionSettings::fuelFiredHeaterSetting::enabled  = 0
    // EVPreconditionSettings::fuelFiredHeaterSetting::disabled = 1
    // EVPreconditionSettings::prioritySetting::prioritize_comfort = 0
    // EVPreconditionSettings::prioritySetting::prioritize_range   = 1
    /////////////////////////////////////////////////////////////////////////

    //////// CAN SPEC enum value. ////////////
    // 0 = Range Prioritised
    // 1 = Comfort Prioritised
    // 0 = FFH disabled
    // 1 = FFH enabled
    //////////////////////////////////////////
    setState(ECC_PRESET_LISTEN_CAN);

    //EvtEVPreconditionSettings *_preSetting = mPreconditionSettings;
    if(mPreconditionSettings->fuelFiredHeaterSetting_field != NOT_AVAIL)
    {
        if( IsPreconditioningSettingChanged(mPreconditionSettings->fuelFiredHeaterSetting_field, mEvVif->queryCanData(Signal_FFHPreClimSetting_RX)) == false)
        {
            LOG_EV("Signal_FFHPreClimSetting_RX[%d] is not changed yet.", mEvVif->queryCanData(Signal_FFHPreClimSetting_RX));
            return;
        }
    }

    if(mPreconditionSettings->prioritySetting_field != NOT_AVAIL)
    {
        if( IsPreconditioningSettingChanged(mPreconditionSettings->prioritySetting_field, mEvVif->queryCanData(Signal_PreClimRangeComfort_RX)) == false)
        {
            LOG_EV("Signal_PreClimRangeComfort_RX[%d] is not changed yet.", mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
            return;
        }
    }

    LOG_EV("PreconditionSettings Request is success. [fuelFiredHeaterSetting:%d][FFHPreClimSetting:%d] [prioritySetting:%d][PreClimRangeComfort:%d]",
         mPreconditionSettings->fuelFiredHeaterSetting_field,
         mEvVif->queryCanData(Signal_FFHPreClimSetting_RX),
         mPreconditionSettings->prioritySetting_field,
         mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));

    cancelAllTimer();
    sendCanSignal(Signal_FFHPreClimSetTCU_TX, (uint32_t)0);
    sendCanSignal(Signal_PreClimRngCmfSetTCU_TX, (uint32_t)0);

    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
    mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, E_OK);

    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
    setState(ECC_IDLE);

    //check signal
    // 1. check FFHPreClimSetting
    // 2. check PreClimRangeComfort
    // 3. if value has been chanaged, cancel ECC_SETTING_TRIGGER_TIMER
    //                                cancel ECC_NM_TIMEOUT_TImer.
    //                                or cancel All Timer
    //      and send signal( Signal_FFHPreClimSetTCU_TX or Signal_PreClimRngCmfSetTCU_TX)
    //      and send PreconditionSettingsReponse Ack.
    //      and send NMMessage release.
    // 4. else, ingore all.
    //change state/
}

void EvClimateApp::timeout_ECC_SETTING_TRIGGER_TIMER()
{
    Mutex::Autolock autolock(mLock);
    if(getState() == ECC_IDLE)
    {
        return;
    }

    bool _preSettingResult = true;

    if(mPreconditionSettings->fuelFiredHeaterSetting_field != NOT_AVAIL)
    {
        if( IsPreconditioningSettingChanged(mPreconditionSettings->fuelFiredHeaterSetting_field, mEvVif->queryCanData(Signal_FFHPreClimSetting_RX)) == false)
        {
            LOG_EV("Signal_FFHPreClimSetting_RX[%d] is not changed yet2.", mEvVif->queryCanData(Signal_FFHPreClimSetting_RX));
            _preSettingResult = false;
        }
    }

    if(mPreconditionSettings->prioritySetting_field != NOT_AVAIL)
    {
        if( IsPreconditioningSettingChanged(mPreconditionSettings->prioritySetting_field, mEvVif->queryCanData(Signal_PreClimRangeComfort_RX)) == false)
        {
            LOG_EV("Signal_PreClimRangeComfort_RX[%d] is not changed yet2.",mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
            _preSettingResult = false;
        }
    }

    sendCanSignal(Signal_FFHPreClimSetTCU_TX, (uint32_t)0);
    sendCanSignal(Signal_PreClimRngCmfSetTCU_TX, (uint32_t)0);

    if(_preSettingResult == true)
    {
        LOG_EV("PreconditionSettings Request is success2. [fuelFiredHeaterSetting:%d][FFHPreClimSetting:%d] [prioritySetting:%d][PreClimRangeComfort:%d]",
         mPreconditionSettings->fuelFiredHeaterSetting_field,
         mEvVif->queryCanData(Signal_FFHPreClimSetting_RX),
         mPreconditionSettings->prioritySetting_field,
         mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));

        cancelAllTimer();
        mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, E_OK);
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
    }

    //TODO list --> Need to Spec clarification.
    // 1. check FFHPreClimSetting
    // 2. check PreClimRangeComfort
    // 3. if value has been chanaged, cancel All timer.
    //      and send signal( Signal_FFHPreClimSetTCU_TX or Signal_PreClimRngCmfSetTCU_TX)
    //      and send PreconditionSettingsReponse Ack.
    //      and send NMMessage release.

}

void EvClimateApp::timeout_ECC_PRESET_NM_TIMEOUT_TIMER()
{
    Mutex::Autolock autolock(mLock);
    if(getState() == ECC_IDLE)
    {
        return;
    }
    cancelAllTimer();
    sendCanSignal(Signal_FFHPreClimSetTCU_TX, (uint32_t)0);
    sendCanSignal(Signal_PreClimRngCmfSetTCU_TX, (uint32_t)0);

    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
    mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, svc::ErrorCode::timeout_chosen/*SRD_ReqECC0122*/);
    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
    setState(ECC_IDLE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t EvClimateApp::getRemainTimerTime(uint8_t timerName)
{
    if(ECC_Timers[timerName] == NULL)
    {
        return 0;
    }

    uint32_t remainingRuntime = ECC_Timers[timerName]->getRemainingTimeMs();
    LOG_EV("[%s]remainingRuntimeMs = [%d]", getEccTimerName[timerName].c_str(), remainingRuntime);
    return remainingRuntime;
}

/////////////////// Undefined CAN SINGAL /////////////////////////
bool EvClimateApp::rcv_CAN_PreClimateStatus()
{
    int32_t _preClimateStatusData = mEvVif->queryCanData(Signal_PreClimateStatus_RX);

    EvtEVPreconditionStatus::GetInstance()->setPreconditionOperatingStatusField(_preClimateStatusData);
    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));

    if(getState() == ECC_WAIT_CAN_TIMER || getState() == ECC_OFF || getState() == ECC_PRESET_WAIT_CAN)
    {
        LOGI("[Warnning] PreClimateStatus signal has been locked. Current Status : [%s]", EccState[getState()].c_str());
        return false;
    }

    if(getState() == ECC_LISTEN_CAN_SIG)
    {
        send_NGTP_PreconditionStartResp(_preClimateStatusData);
        return false;
    }

    if(getState() == ECC_WAIT_PRE_STOP && 10 == _preClimateStatusData)
    {
        cancelTimer(ECC_HVAC_NO_RESP_TIMEOUT_TIMER);
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_STOP_RESP, E_OK);
        sendCanSignal(Signal_ParkClimateReqTCU_TX, 0);
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
        return true;
    }

    //SRD_ReqECC0008_v2
    if(0 == _preClimateStatusData && getState() == ECC_DIRECT_PRECONDI && EvVehicleStatus::GetInstance()->getCurPowerMode() == 7)
    {
        LOG_EV("Change to Normal mode. PreClimateStatus is 0, PM=7");
        cancelTimer(ECC_DIRECT_PRECONDITION_TIMER);
        cancelTimer(ECC_HVAC_NO_RESP_TIMEOUT_TIMER);
        SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, svc::AlertData::preconditioningRemoteToNormal_chosen);
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
        return true;
    }

    //SRD_ReqECC0106_v2 && PCR040
    if(0 == _preClimateStatusData && getState() == ECC_WAIT_PRE_STOP)
    {
        cancelTimer(ECC_HVAC_NO_RESP_TIMEOUT_TIMER);
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_STOP_RESP, E_OK);
        sendCanSignal(Signal_ParkClimateReqTCU_TX, 0);
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
        return true;
    }

    //SRD_ReqECC0030_v2 && PCR040 (2017.09.21)
    if(0 == _preClimateStatusData)
    {
        LOG_EV("Change to Normal mode. PreClimateStatus is 0");
        cancelTimer(ECC_DIRECT_PRECONDITION_TIMER);
        cancelTimer(ECC_HVAC_NO_RESP_TIMEOUT_TIMER);
        SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, svc::AlertData::preconditioningStopped_chosen);
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
        return true;
    }

    if(10 == _preClimateStatusData)
    {
        LOG_EV("PreClimateStatus is 10");
        if(getRemainTimerTime(ECC_DIRECT_PRECONDITION_TIMER) != 0)
        {
            cancelTimer(ECC_DIRECT_PRECONDITION_TIMER);
            SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, getAlertResultFromPreClimateStatus(_preClimateStatusData));
        }
        else
        {
            cancelAllTimer();
            sendCanSignal(Signal_ParkClimateReqTCU_TX, 0);
        }
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
        return true;
    }

    if(14 == _preClimateStatusData)
    {
        LOG_EV("PreClimateStatus is 14");
        cancelTimer(ECC_DIRECT_PRECONDITION_TIMER);

        uint16_t _result = IsAvailableRES();
        if(_result == E_OK)
        {
            SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, svc::AlertData::preconditioningSocCritical_chosen);
            EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
            setState(ECC_IDLE);
        }
        else
        {
            SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, _result);
            sendCanSignal(Signal_ParkClimateReqTCU_TX, 4); //NOTE: (CAN SPEC) ESIS_TCU_B ParkClimateReqTCU type. 4 - Stop Requested.

            setTimer(ECC_HVAC_NO_RESP_TIMEOUT_TIMER, mEvConfig->getEvCfgData("ECC_HVAC_NO_RESP_TIMEOUT"));
        }
        return true;
    }

    if((getState() == ECC_DIRECT_PRECONDI)
       && ((_preClimateStatusData == 11) || (_preClimateStatusData == 12)))
    {
        LOGV("Precondition is stopped by HAVC. [%d]",_preClimateStatusData);
        cancelTimer(ECC_DIRECT_PRECONDITION_TIMER);
        EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
        setState(ECC_IDLE);
    }

    // Normal Case.
    SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, getAlertResultFromPreClimateStatus(_preClimateStatusData));
    return true;
}

bool EvClimateApp::rcv_CAN_HVBattHeatingStatus(Buffer& buf)
{
    uint8_t *canBlock = buf.data();
    EvtEVPreconditionStatus *tData = EvtEVPreconditionStatus::GetInstance();
    if(canBlock[0]>2)
    {
        LOGE("Unknown HVBattHeatingStatus Singal Value [%d]", canBlock[0]);
        return false;
    }
    uint8_t tField = (canBlock[0] == 0) ? canBlock[0] : 1; /*Refer SRD_ReqEcc0024 1: heating, 0: off*/
    tData->setBatteryPreconditioningStatusField(tField);

    if(getState() == ECC_WAIT_CAN_TIMER || getState() == ECC_OFF || getState() == ECC_PRESET_WAIT_CAN)
    {
        LOGI("[Warnning] CAN_HVBattHeatingStatus has been locked. Current Status : [%s]", EccState[getState()].c_str());
        return false;
    }

    send_NGTP_PreconditionUpdateAlert();
    return true;
}

bool EvClimateApp::rcv_CAN_HVBattCoolingStatusExt(Buffer& buf)
{
    uint8_t *canBlock = buf.data();
    EvtEVPreconditionStatus *tData = EvtEVPreconditionStatus::GetInstance();
    if(canBlock[0]>4)
    {
        LOGE("Unknown HVBattCoolingStatusExt Singal Value [%d]", canBlock[0]);
        return false;
    }
    uint8_t tField = (canBlock[0] > 1) ? 2 : 0; /*Refer SRD_ReqEcc0024 2: cooling, 0: off*/
    tData->setBatteryPreconditioningStatusField(tField);

    if(getState() == ECC_WAIT_CAN_TIMER || getState() == ECC_OFF || getState() == ECC_PRESET_WAIT_CAN)
    {
        LOGI("[Warnning] CAN_HVBattCoolingStatusExt has been locked. Current Status : [%s]", EccState[getState()].c_str());
        return false;
    }
    send_NGTP_PreconditionUpdateAlert();
    return true;
}

void EvClimateApp::rcv_CAN_FFHOperatingStatus(Buffer& buf)
{
    if(EvVehicleStatus::GetInstance()->getVehicleType() != EvVehicleStatus::PHEV)
    {
        LOGE("[Warnning] CAN_FFHOperatingStatus has been locked. This vehicle is not PHEV!");
        return ;
    }

    if(getState() == ECC_WAIT_CAN_TIMER || getState() == ECC_OFF || getState() == ECC_PRESET_WAIT_CAN)
    {
        LOGI("[Warnning] CAN_FFHOperatingStatus has been locked. Current Status : [%s]", EccState[getState()].c_str());
        return ;
    }

    send_NGTP_PreconditionUpdateAlert();
}

bool EvClimateApp::rcv_CAN_PreClimateRequests(Buffer& buf)
{
    uint8_t *canBlock = buf.data();
    EvtEVPreconditionStatus *tData = EvtEVPreconditionStatus::GetInstance();
    tData->setPreconditioningModeField(canBlock[0]);

    if(getState() == ECC_WAIT_CAN_TIMER || getState() == ECC_OFF || getState() == ECC_PRESET_WAIT_CAN)
    {
        LOGI("[Warnning] CAN_PreClimateRequests has been locked. Current Status : [%s]", EccState[getState()].c_str());
        return ;
    }

    send_NGTP_PreconditionUpdateAlert(); //SRD_ReqECC0017_v1
    return true;
}

void EvClimateApp::rcv_CAN_FFHPreClimSetting(Buffer& buf)
{
    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
    if(EvVehicleStatus::GetInstance()->getVehicleType() != EvVehicleStatus::PHEV)
    {
        LOGE("[Warnning] rcv_CAN_FFHPreClimSetting has been locked. This vehicle is not PHEV!");
        return ;
    }

    if(getState() == ECC_PRESET_WAIT_CAN || getState() == ECC_WAIT_CAN_TIMER)
    {
        LOGV("[Warnning] rcv_CAN_FFHPreClimSetting has been locked. [%s] ", EccState[getState()].c_str());
        return;
    }

    //SRD_ReqECC0124_v1
    if(getState() != ECC_OFF)
    {
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_UPDATE, E_OK);
    }

    if(getState() == ECC_PRESET_LISTEN_CAN)
    {
        timeout_ECC_CAN_SETTING_IGNORE_TIMER();
    }
}

void EvClimateApp::rcv_CAN_PreClimRangeComfort(Buffer& buf)
{
    mPreconditionSettings->setCurPreconditionSettingSignal(mEvVif->queryCanData(Signal_FFHPreClimSetting_RX), mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
    if(getState() == ECC_PRESET_WAIT_CAN || getState() == ECC_WAIT_CAN_TIMER)
    {
        LOGV("[Warnning] rcv_CAN_PreClimRangeComfort has been locked. [%s] ", EccState[getState()].c_str());
        return;
    }

    //SRD_ReqECC0129_v1
    if(getState() != ECC_OFF)
    {
        mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_UPDATE, E_OK);
    }

    if(getState() == ECC_PRESET_LISTEN_CAN)
    {
        timeout_ECC_CAN_SETTING_IGNORE_TIMER();
    }
}
////////////////////////////////////////////////////////////////////

uint8_t EvClimateApp::check_ClimateControlCondition()
{
    //SRD_ReqECC0041, SRD_ReqECC0042
    // #1 : Power mode >= 4
    if(IsPowerModeHigerThanFour() == true)    // SRD_ReqECC0041_v1
    {
        LOGE("[Warnning] Climate Contiditon Failure. <<(condition : PM >= 4) PM[%d]>>", mEvConfig->getEvCfgData("POWER_MODE"));
        return svc::ErrorCode::vehiclePowerModeNotCorrect_chosen;
    }

    EvVehicleStatus* _evVehicleStatus = EvVehicleStatus::GetInstance();
    // #2 : No crash event
    if(_evVehicleStatus->IsCrashEventActivated() == true)
    {
        LOGE("[Warnning] Climate Contiditon Failure. <<Crash Event is actived.>>");
        return svc::ErrorCode::crashEvent_chosen;
    }

    // #3 : alarm is active
    if(_evVehicleStatus->IsAlarmActivated() == true)
    {
        LOGE("[Warnning] Climate Contiditon Failure. <<Alarm Event is activeed.>>");
        return svc::ErrorCode::alarmActive_chosen;
    }

    // #4 : SVT not active
    if(_evVehicleStatus->IsSVTActivated() == true)
    {
        LOGE("[Warnning] Climate Contiditon Failure. <<SVT Event is activeed.>>");
        return svc::ErrorCode::theftEvent_chosen;
    }

    // #5 : Preconditioning already in progresss.
    if(IsPreconditioningAlreayInProgress() == true)
    {
        LOGE("[Warnning] Climate Service Already Running");
        return svc::ErrorCode::serviceAlreadyRunning_chosen;
    }

    return E_OK;
}

void EvClimateApp::DBG_Precoondition(uint8_t value)
{
    switch(value)
    {
        case 1:
        {
            LOGV("[DBG] Trigger Precondition Start! ");
            rcv_PreconditionStartReq(0);
            break;
        }
        case 0:
        {
            LOGV("[DBG] Trigger Precondition Stop! ");
            rcv_PreconditionStopReq();
            break;
        }
        case 2:
        {
            LOGV("[DBG] Trigger (fuelFiredHeaterSetting - enabled) ");
            rcv_PreconditionSettingReq(0, -1, 0);
            break;
        }
        case 3:
        {
            LOGV("[DBG] Trigger (fuelFiredHeaterSetting - disabled) ");
            rcv_PreconditionSettingReq(1, -1, 0);
            break;
        }
        case 4:
        {
            LOGV("[DBG] Trigger (prioritySetting - prioritize-comfort)");
            rcv_PreconditionSettingReq(-1, 0, 0);
            break;
        }
        case 5:
        {
            LOGV("[DBG] Trigger (prioritySetting - prioritize-range) ");
            rcv_PreconditionSettingReq(-1, 1, 0);
            break;
        }
        default:
        {
            LOGE("Unknown Command #[%d]",value);
            break;
        }
    }
}

//////////////////PRIVATE Method////////////////////

void EvClimateApp::send_NGTP_PreconditionUpdateAlert()
{
    int _preClimateStatusData = mEvVif->queryCanData(Signal_PreClimateStatus_RX);
    SEND_NGTP(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, getAlertResultFromPreClimateStatus(_preClimateStatusData));
}

// ECC Timer.
void EvClimateApp::setTimer(uint8_t timerName, int32_t expireTime, bool isMsTime = false)
{
    // Timer Setting.
    if(expireTime <= 0)
    {
        LOGE("[ERROR] Timer[%s] expireTime[%d] is invalid", getEccTimerName[timerName].c_str(), expireTime);
        return;
    }

    LOGV("[%s] timer is started! [%d]%s", getEccTimerName[timerName].c_str(),
                                        expireTime,
                                        isMsTime ? "ms" : "sec");

    ECC_Timers[timerName] = new Timer( t_timer, timerName );

    if(isMsTime == true)
    {
        ECC_Timers[timerName]->setDurationMs(expireTime, 0);
    }
    else
    {
        ECC_Timers[timerName]->setDuration(expireTime, 0);
    }
    ECC_Timers[timerName]->start();
}

bool EvClimateApp::resetTimer(uint8_t timerName)
{
    if(ECC_Timers[timerName] == NULL)
    {
        return false;
    }

    ECC_Timers[timerName] = NULL;
    return true;
}

void EvClimateApp::cancelTimer(uint8_t timerName)
{
    if(ECC_Timers[timerName] != NULL)
    {
        LOGI("[%s]Timer is canceled!", getEccTimerName[timerName].c_str());
        ECC_Timers[timerName]->stop();
        ECC_Timers[timerName] = NULL;
    }
    resetTimer(timerName);
}

void EvClimateApp::cancelAllTimer()
{
    LOG_EV("Cancel All Timer!");

    for(int i=0 ; i<ECC_END_TIMER ; i++)
    {
        cancelTimer(i);
    }
}

error_t EvClimateApp::sendCanSignal(int sigID, const uint8_t data)
{
    return mEvVif->sendCanSignal(sigID, data);
}

uint8_t EvClimateApp::getErrorResultFromPreClimateStatus(int32_t data)
{
    switch(data)
    {
        //SRD_ReqECC0038
        case 2:     // PreClim with Engine Running
        case 3:     // PreClim Vent
        case 4:     // PreClim Heat
        case 5:     // PreClim Cool
        case 6:     // PreClim
        case 7:     // PreClim Vent & Batt Cool
        case 8:     // PreClim & Batt Cool
        case 9:     // Engine Heat
        {
            return E_OK;
        }

        //SRD_ReqECC0039
        case 1:     // StartUp
        case 0:     //PreClim OFF even after the completion of HVAC response timeout
        case -1:    // Tested by sldd & NM timeouted.
        {
            return svc::ErrorCode::hvacTimeout_chosen;
        }

        //SRD_ReqECC0058
        case 10:    // PreClim Finished
        case 13:    // conditionsNotCorrect
        {
            return svc::ErrorCode::conditionsNotCorrect_chosen;
        }

        //SRD_ReqECC0043
        case 11:    //Insufficient HV battery charge
        {
            return svc::ErrorCode::batteryLevelTooLow_chosen;
        }

        //SRD_ReqECC0044
        case 12:    // HVAC system failure
        {
            return svc::ErrorCode::hvacSystemFailure_chosen;
        }

        //SRD_ReqECC0056 --> SRD_ReqECC0056_v2 : SRD9 v55
        case 14:    //HV insufficient battery charge and RES is required
        {
            if(EvVehicleStatus::GetInstance()->getVehicleType() == EvVehicleStatus::BEV)
            {
                return svc::ErrorCode::batteryLevelTooLow_chosen;
            }
            return svc::ErrorCode::resRequired_chosen;
        }

        //SRD_ReqECC0057
        case 15:    // PreClim Grace Period
        {
            return svc::ErrorCode::serviceAlreadyRunning_chosen;
        }

        default:    // Unknown Data
        {
            LOGE("[ERROR] Unknown PreClimateStatus CAN signal value [%d]",data);
            return svc::ErrorCode::invalidData_chosen;
        }
    }
}

uint8_t EvClimateApp::getAlertResultFromPreClimateStatus(int32_t data)
{
    switch(data)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            return svc::AlertData::preconditioningStarted_chosen;
        case 10:
            return svc::AlertData::preconditioningStopped_chosen;
        case 11:
            return svc::AlertData::preconditioningNotOkSoc_chosen;
        case 12:
            return svc::AlertData::preconditioningNotOkHVACfailure_chosen;
        case 13:
            return svc::AlertData::preconditioningPartiallyCompleted_chosen;
        case 14:
            return svc::AlertData::preconditioningSocCritical_chosen;
        case 15:
            return svc::AlertData::preconditioningComfortLevelReached_chosen;
        case 0:
            return svc::AlertData::preconditioningNotOk_chosen;
        // Special Case.
        case svc::AlertData::preconditioningRemoteToNormal_chosen: /*69*/
            return svc::AlertData::preconditioningRemoteToNormal_chosen;
        default:
            return svc::AlertData::preconditioningNotOkFault_chosen;
    }
}

void EvClimateApp::toStringState(unsigned char state, unsigned char newState)
{
    if(state < 0 && state >= _END_ENUM_)
    {
        return ;
    }

    if(newState < 0 && newState >= _END_ENUM_)
    {
        return ;
    }

    LOG_EV("EvClimateApp state is changed : [%s] => [%s]", EccState[state].c_str(), EccState[newState].c_str());
}

uint16_t EvClimateApp::IsAvailableRES() //return Failure code. 0 = E_OK
{
    EvVehicleStatus* _evVehicleStatus = EvVehicleStatus::GetInstance();

    // Additional Check.
    //7. Is it PHEV?
    if(_evVehicleStatus->getVehicleType() == EvVehicleStatus::BEV)
    {
        LOGE("[Warnning] It is Vehicle of BEV type. <<SRD_ReqECC0097_v1>>");
        return svc::AlertData::preconditioningNotOkSoc_chosen;
    }

    //9. HVAC is TimedPrecondition State SRD_ReqECC0070_v1  --> change to more higer priority
    if(_evVehicleStatus->getVehicleType() == EvVehicleStatus::PHEV &&
       mEvVif->queryCanData(Signal_PreClimateRequests_RX) == 1 /*PreClimTimed*/)
    {
        LOGE("[PHEV] HVAC is TimedPrecondition state. <<SRD_ReqECC0070_v1>>");
        return svc::AlertData::preconditioningNotOkSoc_chosen;
    }

    //Condition check : SRD_ReqECC0051_v1
    //1. SVT is active
    if(_evVehicleStatus->IsSVTActivated() == true)
    {
        return svc::AlertData::preconditioningResNotOkSVTActive_chosen;
    }

    //2. Power Mode >= 4
    if(IsPowerModeHigerThanFour() == true)
    {
        return svc::AlertData::preconditioningResNotOkPowerMode_chosen;
    }

    //3. Crash event is detected
    if(_evVehicleStatus->IsCrashEventActivated() == true)
    {
        return svc::AlertData::preconditioningResNotOkCrashActive_chosen;
    }

    //4. Alarm is active
    if(_evVehicleStatus->IsAlarmActivated() == true)
    {
        return svc::AlertData::preconditioningResNotOkAlarmActive_chosen;
    }

    //5. Fuel level not sufficient
    if(_evVehicleStatus->IsFuelLevelNotSufficientForRES() == true)
    {
        return svc::AlertData::preconditioningResNotOkFuelLevelLow_chosen;
    }

    //6. RES max execution time out reached.
    if(_evVehicleStatus->IsMaxExecutionTimeOfRESTimedOut() == true)
    {
        return svc::AlertData::preconditioningResNotOkRemainingTime_chosen;
    }

    //8. RES is provisioned?
    if(IsRESActivated() == false)
    {
        LOGE("[ERROR] RES is not Provisioned!! return:<preconditioningResNotOkHVACfailure>");
        return svc::AlertData::preconditioningResNotOkHVACfailure_chosen;
    }

    LOGV("RES is available!!");
    return E_OK;
}

bool EvClimateApp::IsPowerModeHigerThanFour()
{
    if(EvVehicleStatus::GetInstance()->getCurPowerMode() > 4)    // SRD_ReqECC0041_v1
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EvClimateApp::IsPreconditioningAlreayInProgress()
{
    int _preClimateStatusData = mEvVif->queryCanData(Signal_PreClimateStatus_RX);
    if( (1 <= _preClimateStatusData && _preClimateStatusData <= 9 )
       || _preClimateStatusData == 15)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EvClimateApp::IsRESActivated()
{
    int32_t APP_MODE_RVC = mEvConfig->getEvCfgData("AppModeRVC");
    if(APP_MODE_RVC == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}
