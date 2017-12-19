#include "EvHBDApp.h"
#include "EvHBDAppHandler.h"
#include "EvHBDAppTimer.h"
#include "EvtUtil/EvtUtil.h"
#include "EvServiceManager.h"
#define LOG_TAG "[EVT]EvHBDApp"
#include "Log.h"

EvHBDApp::EvHBDApp()
{
    state = HBD_OFF;
    m_Looper = SLLooper::myLooper();
    mHandler = new EvHBDAppHandler(m_Looper, *this);
    bTimeToDataRecord = false;
    bTimeToUpdateTSP = false;

}

void EvHBDApp::onStart()
{
    LOGV("EvHBDApp::onStart");
    setState(HBD_ON);
    for(int i=0 ; i < HBD_END_TIMER ; i++)
    {
        HBD_Timers[i] = NULL;
    }
    determineHBDOperation(EvVehicleStatus::GetInstance()->getCurPowerState());
}

void EvHBDApp::toStringState(unsigned char state, unsigned char newState)
{
    std::string EvHBDState[HBD_END_ENUM];
    EvHBDState[HBD_OFF]                    = "HBD_OFF";
    EvHBDState[HBD_ON]                     = "HBD_ON";
    EvHBDState[HBD_OPER_RECORD_AND_UPDATE] = "HBD_OPER_RECORD_AND_UPDATE";
    EvHBDState[HBD_STOP_RECORD]            = "HBD_STOP_RECORD";
    EvHBDState[HBD_OPER_RECORD_ONLY]       = "HBD_OPER_RECORD_ONLY";
    EvHBDState[HBD_OTHER]                  = "HBD_OTHER";

    if(state < 0 && state>= HBD_END_ENUM)
    {
        return;
    }

    if(newState < 0 && newState >= HBD_END_ENUM)
    {
        return;
    }
    LOGE("EvHBDApp state is changed : [%s] => [%s]", EvHBDState[state].c_str(), EvHBDState[newState].c_str());
}

bool EvHBDApp::rcv_CAN_Signals(sp<Buffer>& buf)
{
    bool bSuccess = false;
    // TODO CSW CanSignal Rcv Impl
    LOGE("EvHBDApp::rcv_CAN_Signals() ==> To be Implemented");
    return bSuccess;
}

void EvHBDApp::rcv_CAN_Frame(uint16_t FrameID)
{
    // TODO CSW CanFrame Signal Rcv Impl
    LOGE("EvHBDApp::rcv_CAN_Frame() ==> To be Implemented");
    // TODO Call Functions like below
    // m_mapFrameFunc[_FrameID]();
}

void EvHBDApp::setTimer(uint8_t timerName, uint16_t expireTime, bool isMsTime)
{
    // Timer Setting.
    if(expireTime <= 0)
    {
        LOGE("[ERROR] Timer[%d] expireTime is invalid", timerName);
        return;
    }

    EvHBDTimer* t_timer = new EvHBDTimer(*this);
    HBD_Timers[timerName] = new Timer( t_timer, timerName );

    if(isMsTime)
    {
        HBD_Timers[timerName]->setDurationMs(expireTime, 0);
    }
    else
    {
        HBD_Timers[timerName]->setDuration(expireTime, 0);
    }
    HBD_Timers[timerName]->start();
}

void EvHBDApp::cancelTimer(uint8_t timerName)
{
    if(getRemainTimerTime(timerName) != 0)
    {
        LOG_EV("[%d]Timer has canceled!", timerName);
        HBD_Timers[timerName]->stop();
    }
    resetTimer(timerName);
}

void EvHBDApp::resetTimer(uint8_t timerName)
{
    HBD_Timers[timerName] = NULL;
}

void EvHBDApp::cancelAllTimer()
{
    LOG_EV("[EvHBDApp]Cancel All Timer!");
    for(int i=0 ; i < HBD_END_TIMER ; i++)
    {
        cancelTimer(i);
    }
}

uint32_t EvHBDApp::getRemainTimerTime(uint8_t timerName)
{
    if(HBD_Timers[timerName] == NULL)
    {
        return 0;
    }
    double _remainTime = HBD_Timers[timerName]->getRemainingTimeMs();
    LOGI("remainingRuntimeSecond = [%lf]", _remainTime);
    return _remainTime;
}

void EvHBDApp::timeout_HBD_CAN_READ_TIMER()
{
    LOGV("[timeout_HBD_CAN_READ_TIMER] expired. Battery Information should be trasmitted");
    //TODO Data Sampling - SetData from CAN Signal
    EvtHvBatteryPack::GetInstance()->sampleDataFromCAN();

    // Re-Start Timer
    setTimer(HBD_CAN_READ_TIMER, EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_READ_TIME"), false);
    LOGV("[timeout_HBD_CAN_READ_TIMER]HBD_CAN_READ_TIMER[%u]s Re-start!", EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_READ_TIME"));
}

void EvHBDApp::timeout_HBD_CAN_BUFFER_TIMER()
{
    LOGV("[timeout_HBD_CAN_BUFFER_TIMER] expired");
    //TODO Data Sampling - SetData from CAN Signal

    // Re-Start Timer
    setTimer(HBD_CAN_BUFFER_TIMER, EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_BUFFER_TIME"), false);
    LOGV("HBD_CAN_BUFFER_TIMER[%u]s start!", EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_BUFFER_TIME"));
}

void EvHBDApp::timeout_HBD_BUFFER_MAX_TIMER()
{
    LOGV("[timeout_HBD_BUFFER_MAX_TIMER] expired");
    // Stop Sampling and Cancel All Timer
}

void EvHBDApp::timeout_HBD_TCU_PUSH_TIMER()
{
    LOGV("[timeout_HBD_TCU_PUSH_TIMER] expired");
    //TODO NGTP-GetAvailability to check Connect status with Remote server

    //encode BATTERY_INFORMATION Data and send to TSP
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::BATTERY_INFORMATION_UPDATE, svc::ErrorCode::unselected);

    // Re-Start Timer
    setTimer(HBD_TCU_PUSH_TIMER, EvConfigData::GetInstance()->getEvCfgData("HBD_TCU_PUSH_TIME"), false);
    LOGV("HBD_TCU_PUSH_TIMER[%u]s Re-start!", EvConfigData::GetInstance()->getEvCfgData("HBD_TCU_PUSH_TIME"));
}


void EvHBDApp::determineHBDOperation(uint32_t toState)
{
    uint16_t curPowerMode = EvVehicleStatus::GetInstance()->getCurPowerMode();
    uint32_t isState      = EvVehicleStatus::GetInstance()->getCurPowerState();

    // SRD_ReqHBD0003_v1
    if(isTimeToDataRecord(curPowerMode, isState, toState))
    {
        if(isTimeToUpdateToTSP())// SRD_ReqHBD0004_v1
        {
            LOGI("[determineHBDOperation] TODO SampleData and Update to TSP");
            // TODO Cancel legacy timer
            cancelAllTimer();
            // Change State
            setState(HBD_OPER_RECORD_AND_UPDATE);

            // start HBD_CAN_READ_TIMER
            setTimer(HBD_CAN_READ_TIMER, EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_READ_TIME"), false);
            LOGV("HBD_CAN_READ_TIMER[%u]s start!", EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_READ_TIME"));

            // start HBD_TCU_PUSH_TIMER
            setTimer(HBD_TCU_PUSH_TIMER, EvConfigData::GetInstance()->getEvCfgData("HBD_TCU_PUSH_TIME"), false);
            LOGV("HBD_TCU_PUSH_TIMER[%u]s start!", EvConfigData::GetInstance()->getEvCfgData("HBD_TCU_PUSH_TIME"));
        }
        else // SRD_ReqHBD0005_v1
        {
            int32_t bufferMaxInSec;
            LOGI("[determineHBDOperation] TODO SampleData and Store to File");
            // TODO Cancel legacy timer
            cancelAllTimer();
            // Change State
            setState(HBD_OPER_RECORD_ONLY);

            // start HBD_CAN_BUFFER_TIMER
            setTimer(HBD_CAN_BUFFER_TIMER, EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_BUFFER_TIME"), false);
            LOGV("HBD_CAN_BUFFER_TIMER[%u]s start!", EvConfigData::GetInstance()->getEvCfgData("HBD_CAN_BUFFER_TIME"));

            // start HBD_BUFFER_MAX_TIMER
            bufferMaxInSec = convertDayToSec(EvConfigData::GetInstance()->getEvCfgData("HBD_BUFFER_MAX_TIME"));
            setTimer(HBD_BUFFER_MAX_TIMER, bufferMaxInSec, false);
            LOGV("HBD_BUFFER_MAX_TIMER[%u]s start!", bufferMaxInSec);
        }
    }
    else
    {
        LOGI("[determineHBDOperation] TODO Stop Sampling.");
        // TODO Cancel legacy timer
        cancelAllTimer();

        // Change State
        setState(HBD_STOP_RECORD);
    }
}

// SRD_ReqHBD0003_v1 , SRD_ReqHBD0004_v1
bool EvHBDApp::isTimeToDataRecord(uint16_t curPM, uint32_t isState, uint32_t toState)
{
    bool result = true;
    /**
    TODO
    Check Data Sampling Condition
    1.	When Vehicle Is Turned ON (POWERMODE > 0)
    OR
    2.	When PMZ network is awake in Power mode 0 (POWERMODE == 0 && Network Alive)
    */
    bTimeToDataRecord = result;
    return result;
}

bool EvHBDApp::isTimeToUpdateToTSP()
{
    bool result = true;
    // TODO Check NGTP Availability

    bTimeToUpdateTSP = result;
    return result;
}

int32_t EvHBDApp::convertDayToSec(int32_t valueDays)
{
    int32_t dayToSecFactor = 86400;// 60*60*24
    return (valueDays >= 0) ? valueDays*dayToSecFactor : -1;
}

unsigned char EvHBDApp::getState()
{
    return state;
}
