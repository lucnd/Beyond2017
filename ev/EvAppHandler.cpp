#include "Typedef.h"
#include "services/ApplicationManagerService/IApplicationManagerService.h"
#include "services/ApplicationManagerService/ApplicationManagerService.h"

// EV Applications
#include "EHApp.h"
#include "EvChargeApp.h"
#include "EvClimateApp.h"
#include "EvRangeApp.h"
#include "EvHBDApp.h"

#include "EvAppHandler.h"
#include "EvServiceManager.h"
#include "EvtUtil/EvtUtil.h"
#include "JLRPowerMode.h"

#define LOG_TAG "[EVT]EvAppHandler"
#include "Log.h"

#include "vifManagerService/LGE_CANSignalFrameID.h"
using namespace SignalFrame_PMZ_e;

#include "EvCanFrame.h"

#define SERVICE_RETRY_TIMER 200

inline void triggerDebugCMD(uint32_t arg1, uint32_t arg2);
uint8_t counterOfRetryRegSVTmonitoring = 4;

// Handler for Asyn Signal Call aback pettern.
void EvAppHandler::handleMessage(const sp<sl::Message>& msg)
{
    switch(msg->what)
    {
        case RECV_DEBUG:
        {
            //TODO: Implement
            break;
        }
        case RECV_VIFCAN:
        {
            decode_CAN_Signal(msg->arg1, msg->buffer);
            break;
        }
        case RECV_NGTP:
        {
            receivedNGTP(msg->buffer);
            break;
        }
        case REVC_DIAG:
        {
            //TODO: Implement
            break;
        }
        case REVC_CONFIG:
        {
            // Store config Data.
            EvConfigData::GetInstance()->onChangeConfigData(std::string(msg->buffer.data()));
            // Check Provisioning.
            check_ProvisioningState(std::string(msg->buffer.data()));
            break;
        }
        case REVC_SLDD:
        {
            triggerDebugCMD(msg->arg1, msg->arg2);
            break;
        }
        case RECV_VIFCAN_FRAME:
        {
            decode_CAN_Frame(msg->obj);
            break;
        }
        case RECV_POWER_MODE:
        {
            receive_PM(msg->arg1);
        }
        case REG_SVT_MONITOR:
        {
            regSVTmonitor();
        }
        default:
        {
            break;
        }
    }
}

void EvAppHandler::receivedNGTP(Buffer& buf)
{
    if(EvVehicleStatus::GetInstance()->getCurPowerState() != STATE_NORMAL)
    {
        LOG_EV("PowerState is not Normal. Waiting PowerState to Normal. <RECV_NGTP>");
        sp<sl::Message> message = this->obtainMessage(RECV_NGTP);
        message->buffer.setTo(buf); // copy to message buffer.
        this->sendMessageDelayed(/*Message*/message, /*DelayTime*/SERVICE_RETRY_TIMER);
        return;
    }
    EvtNgtpMsgManager::GetInstance()->rcv_NgtpMessage(buf);
}

void EvAppHandler::regSVTmonitor()
{
    sp<IApplicationManagerService> m_appManager = SET_SERVICE(ApplicationManagerService);
    if(m_appManager->isEnabledApplication((appid_t)APP_ID_SVT) == true /*Enabled*/)
    {
        LOGI("Reg SVT status monitoring");
        sp<Post> post = Post::obtain((appid_t)APP_ID_EV);
        m_appManager->sendPost((appid_t)APP_ID_SVT, post);
    }

    if(counterOfRetryRegSVTmonitoring > 1)
    {
        counterOfRetryRegSVTmonitoring--;
        this->sendMessageDelayed(/*Message*/this->obtainMessage(/*What*/REG_SVT_MONITOR), /*DelayTime*/500);
    }
}

void EvAppHandler::receive_PM(uint16_t pm)
{
    EvVehicleStatus::GetInstance()->setCurPowerMode(pm);

    // SRD_ReqEVC0100_v2
    if(EvConfigData::GetInstance()->configData["AppModeEVC"] == ENABLED && isVehicleNormalMode(pm))
    {
        LOGV("[RECV_POWER_MODE] EVC is initiating.");
        EvChargeApp::GetInstance()->initialSetupStart();
    }
    else
    {
        LOGV("[RECV_POWER_MODE] EVC is not initiating" );
    }
}

void EvAppHandler::decode_CAN_Signal(uint16_t sigID, Buffer& buf)
{
    if(EvVehicleStatus::GetInstance()->getCurPowerState() != STATE_NORMAL)
    {
        LOG_EV("PowerState is not Normal. Waiting PowerState to Normal. <RECV_VIFCAN>");
        sp<sl::Message> message = this->obtainMessage(RECV_VIFCAN, sigID);
        message->buffer.setTo(buf); // copy to message buffer.
        this->sendMessageDelayed(/*Message*/message, /*DelayTime*/SERVICE_RETRY_TIMER);
        return ;
    }

    EvtVifManager::GetInstance()->receiveCAN(sigID, buf);
}

void EvAppHandler::decode_CAN_Frame(void* obj)
{
    EvCanFrame *tEvCanFrame = (EvCanFrame *)obj;
    uint16_t frameID = tEvCanFrame->mFrameID;

    // for(auto tCanInfo : tEvCanFrame->mCanList)
    // {
    //     LOGE("<<debug>>Signal:[%d] Data:[%d]",tCanInfo.mCanId, tCanInfo.mCanData[0]);
    // }

    if (EvVehicleStatus::GetInstance()->getCurPowerState() != STATE_NORMAL)
    {
        LOG_EV("PowerState is not Normal. Waiting PowerState to Normal. <RECV_VIFCAN_FRAME>");
        sp<sl::Message> message = this->obtainMessage(RECV_VIFCAN_FRAME, obj);
        this->sendMessageDelayed(/*Message*/message, /*DelayTime*/SERVICE_RETRY_TIMER);
        return ;
    }

    switch(frameID)
    {
        case SignalFrame_GWM_PMZ_AD_Hybrid_RX:
        case SignalFrame_GWM_PMZ_R_Hybrid_RX:
        case SignalFrame_GWM_PMZ_S_Hybrid_RX:
        case SignalFrame_GWM_PMZ_T_Hybrid_RX:
        case SignalFrame_GWM_PMZ_U_Hybrid_RX:
        case SignalFrame_PCM_PMZ_Q_Hybrid_RX:
        case SignalFrame_PCM_PMZ_S_Hybrid_RX:
        {
            EvtVifManager::GetInstance()->receiveCAN_Frame(tEvCanFrame);
            break;
        }
        default:
            break;
    }

    delete tEvCanFrame; // this pointer instance had been created at EvApp.cpp
}

void EvAppHandler::check_ProvisioningState(std::string lookup_data)
{
    if(lookup_data.compare("AppModeECC") == 0)
    {
        if(EvConfigData::GetInstance()->configData["AppModeECC"] == ENABLED)
        {
            LOG_EV("Ev Climate Control App was provisioned!");
            EvClimateApp::GetInstance()->onStart();
            EvClimateApp::GetInstance()->initialSetup();
        }
        return ;
    }
    if(lookup_data.compare("AppModeEVC") == 0)
    {
        if(EvConfigData::GetInstance()->configData["AppModeEVC"] == ENABLED)
        {
            LOG_EV("Ev Charging App was provisioned.");
            EvChargeApp::GetInstance()->onStart();
            EvChargeApp::GetInstance()->initialSetupStart();
        }
        return ;
    }
    if(lookup_data.compare("AppModeEVR") == 0)
    {
        //TODO:
        //1. check OFF-> On condition?
        //2. If No, return ;
        //3. If Yes, Trigger FirstTime Setting Scenario.
        return ;
    }
    if(lookup_data.compare("AppModeEH") == 0){
        if(EvConfigData::GetInstance()->configData["AppModeEH"] == ENABLED)
        {
            LOG_EV("Ev Charging App was provisioned.");
            EHApp::GetInstance()->onStart();
            //EHApp::GetInstance()->initialSetupStart();
        }
        return ;
    }
    if(lookup_data.compare("AppModeSVT") == 0){
        if(EvConfigData::GetInstance()->configData["AppModeSVT"] == ENABLED)
        {
            LOG_EV("SVT App was provisioned.");
            counterOfRetryRegSVTmonitoring = 5;
            this->sendMessageDelayed(/*Message*/this->obtainMessage(/*What*/REG_SVT_MONITOR), /*DelayTime*/500);
        }
        else
        {
            EvVehicleStatus::GetInstance()->setVehicleSVTstatus(0xFF);
        }
        return ;
    }
}

//SRD_ReqEVC0100_v2 - condition2
bool EvAppHandler::isVehicleNormalMode(int powerMode)
{
    uint16_t _RemoteStartStatus = EvtVifManager::GetInstance()->queryCanData(Signal_RemoteStartStatusMS_RX);
    if(powerMode != 7)
    {
        return false;
    }
    if(_RemoteStartStatus == 1) /*1: Remote Mode (Driver NOT present)*/
    {
        return false;
    }
    LOGV("[isVehicleNormalMode == true][RemoteStartStatue == %d]",_RemoteStartStatus);
    return true;
}

inline void triggerDebugCMD(uint32_t arg1, uint32_t arg2)
{
    switch(arg1 & 0xFF00)
    {
        case DBG_CHARGE_NOW_REQ:
        {
            EvChargeApp::GetInstance()->DBG_ChargeNowSetting(arg1 & 0xF);
            break;
        }
        case DBG_PRECONITION:
        {
            EvClimateApp::GetInstance()->DBG_Precoondition(arg1 & 0xF);
            break;
        }
        case DBG_DEV:
        {
            LOGV("[DBG] Develop internal test code!");
            switch(arg1 & 0xF)
            {
                case 0:
                    LOGV("[DBG] LH Test!!");
                    EHApp::GetInstance()->DBG_EH_Data();
                    break;
                case 1:
                    EvtNgtpMsgManager::GetInstance()->setLoggingNgtpDecoder(true);
                    break;
                case 2:
                    EvtNgtpMsgManager::GetInstance()->setLoggingNgtpDecoder(false);
                    break;
                default:
                    break;
            }
            break;
        }
        case DBG_CHARGE_SETTING_TEST:
        {
            LOGE("EVHandlerApp]DBG_CHARGE_SETTING_TEST_START");
            EvConfigData::GetInstance()->readEvcTestData();
            LOGE("EVHandlerApp]DBG_CHARGE_SETTING_TEST_END");
            break;
        }
        case DBG_CHARGE_MODE_CHOICE:
        {
            uint8_t value = arg1 & 0xF;
            LOGE("[EVHandlerApp]DBG_CHARGE_MODE_CHOICE : %d", value);
            EvChargeApp::GetInstance()->DBG_ChargeModeChoice(value);
            break;
        }
        case DBG_MAX_STATE_OF_CHARGE:
        {
            uint32_t permValue = arg1 & 0x00FF;
            uint32_t oneOffValue = arg2 & 0x00FF;
            LOGE("[EVHandlerApp]DBG_MAX_STATE_OF_CHARGE perm : %d oneOff : %d", permValue, oneOffValue);
            EvChargeApp::GetInstance()->DBG_MaxStateOfCharge(permValue, oneOffValue);
            break;
        }
        case DBG_DEPARTURE_TIMERS:
        {
            LOGE("[EVHandlerApp]DBG_DEPARTURE_TIMERS");
            break;
        }
        case DBG_TARIFFS:
        {
            LOGE("[EVHandlerApp]DBG_TARIFFS");
            break;
        }
        default:
        {
            LOGE("[Error] unknown Debug command. [%d]", arg1);
            break;
        }
    }
}

