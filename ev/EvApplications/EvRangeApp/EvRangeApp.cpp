//EVR Feature Static Class
#include "EvRangeApp.h"
#include "EvRangeAppHandler.h"

#define LOG_TAG "[EVT]EVRangeApp"
#include "Log.h"

EvRangeApp::EvRangeApp()
{
    state = EVR_OFF;
    m_Looper = SLLooper::myLooper();
    mHandler = new EvRangeAppHandler(m_Looper, *this);
    // TODO CSW Map inline functions like below - refer to EVChargeApp
    // m_mapFrameFunc[SignalFrame_GWM_PMZ_AD_Hybrid_RX] = &rcv_Notification_GWM_PMZ_AD_Hybrid;
}

void EvRangeApp::onStart()
{
    if(EvVehicleStatus::GetInstance()->getVehicleType() != EvVehicleStatus::BEV)
    {
        LOGE("Note: EV Range application shall be active when vehicle type is BEV and EV Range application is provisioned (as described in ReqEVR0060 and ReqEVR0070)");
        LOGI("End EV Range application. Bye JLR~!");
    }
    setState(EVR_IDLE);
}

void EvRangeApp::initialSetupStart()
{
    //TODO CSW
}

void EvRangeApp::initialSetupEnd()
{
    //TODO CSW
}

void EvRangeApp::toStringState(unsigned char state, unsigned char newState)
{
    std::string EvAppState[EVR_END_ENUM];
    EvAppState[EVR_OFF] = "EVR_OFF";
    EvAppState[EVR_IDLE] = "EVR_IDLE";

    if(state < 0 && state>= EVR_END_ENUM)
    {
        return;
    }

    if(newState < 0 && newState >= EVR_END_ENUM)
    {
        return;
    }

    LOGE("EvRangeApp state is changed : [%s] => [%s]", EvAppState[state].c_str(), EvAppState[newState].c_str());
}

bool EvRangeApp::rcv_CAN_Signals(sp<Buffer>& buf)
{
    bool bSuccess = false;
    // TODO CSW CanSignal Rcv Impl
    LOGE("EvRangeApp::rcv_CAN_Signals() ==> To be Implemented");
    return bSuccess;
}

void EvRangeApp::rcv_CAN_Frame(uint16_t FrameID)
{
    // TODO CSW CanFrame Signal Rcv Impl
    LOGE("EvRangeApp::rcv_CAN_Frame() ==> To be Implemented");
    // TODO Call Functions like below
    // m_mapFrameFunc[_FrameID]();
}

void EvRangeApp::setTimer(uint8_t timerName, uint16_t expireTime)
{
    // Timer Setting.
}
