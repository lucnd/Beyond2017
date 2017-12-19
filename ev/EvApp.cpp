///////////////////////////////////////////////////////////
//  EvApp.cpp
//  Implementation of the Class EvApp
//  Created on:
//  Original author: Hojunes.Lee
///////////////////////////////////////////////////////////
#include <time.h>
#include <binder/IServiceManager.h>
#include "services/ApplicationManagerService/IApplicationManagerService.h"

// EV Applications
#include "EHApp.h"
#include "EvChargeApp.h"
#include "EvClimateApp.h"
#include "EvRangeApp.h"
#include "EvHBDApp.h"

// EvApp Class
#include "EvApp.h"
#include "EvAppHandler.h"
#include "EvReceiver/EvReceiver.h"
#include "EvCanList.h"
#include "EvConfigDataList.h"
#include "EvtUtil/EvtUtil.h"
#include "EvCanFrame.h"

#define LOG_TAG "[EVT]EvApp"
#include "Log.h"

static android::sp<Application> gApp;

EvApp::EvApp()
{
    state = EVAPP::OFF;          // set EvApp state.
    LOGE("onCreate EvApp : built[%s]", __TIMESTAMP__);
    LOGV("20171027 CAN Inf path version.");

    m_Looper = SLLooper::myLooper();
    mHandler = new EvAppHandler(m_Looper, *this);
}

void EvApp::onCreate()
{
    //// Receiver Binding. ////
    m_powerModeReceiver = new PowerModeReceiver(*this);
    m_powerStateReceiver = new PowerStateReceiver(*this);
    m_vifMgrReceiver = new VifReceiver(*this);
    m_ngtpMgrReceiver = new NgtpReceiver(*this);
    m_DebugReceiver = new DebugReceiver(*this);
    m_configManagerReceiver = new ConfigurationManagerReceiver(*this);

    //// Receiver Connect. ////
    regVifService();
    regPwrService();
    regNgtpService();
    regDebugService();
    regConfigurationService();

    //// Evt Service Binding. ////
    //EvtVifManager::GetInstance()->onStart();
    EvtVifManager::GetInstance()->setVIFService(m_vifMgrService, getAppID());
    //EvtNgtpMsgManager::GetInstance()->onStart();
    EvtNgtpMsgManager::GetInstance()->setNGTPService(m_ngtpMgrService);
    EvConfigData::GetInstance()->onStart();
    EvConfigData::GetInstance()->setConfigurationService(m_configManagerService);

    // EvAppBinder Instantiate
    LOGV("Call EvAppBinderService instance222");
    m_EvAppBinder = new EvAppBinderService();
    m_EvAppBinder->instantiate();

    //// Booting Condition Check! ////
    bool isEV = EvConfigData::GetInstance()->setEvBootConfig(EVT_configDataMonitorList, COUNT_OF(EVT_configDataMonitorList));
    if(!isEV)
    {
        onDestroy();
        return;
    }
    setState(EVAPP::IDLE);

    if(checkMdmVariant() == false)
    {
        LOGE("This board is not TeleOnly or High variant.");
        onDestroy();
        return;
    }

    //// Application Binding. ////
    bindingApplication();

    //// Data Class Binding & initialization. ////
    EvtElectricVehicleInformation::GetInstance()->resetAll();
    EvtChargeSettings::GetInstance()->resetAll();
    EvtEVPreconditionStatus::GetInstance()->resetAll();

    regSVTmonitor();
    LOGI("Booting Complete.");
}

void EvApp::bindingApplication()
{
    if(EvConfigData::GetInstance()->configData["AppModeECC"] == ENABLED)
    {
        LOGI("Ev Climate Control App has been provisioned.");
        EvClimateApp::GetInstance()->onStart();
    }

    if(EvConfigData::GetInstance()->configData["AppModeEVC"] == ENABLED)
    {
        LOGI("Ev Charging App has been provisioned.");
        EvChargeApp::GetInstance()->onStart();
    }

    if(EvConfigData::GetInstance()->configData["AppModeEH"] == ENABLED)
    {
        LOGI("Ev Long Horizon App has been provisioned.");
        EHApp::GetInstance()->onStart();
    }

    if(EvConfigData::GetInstance()->configData["AppModeEVR"] == ENABLED)
    {
        LOGI("Ev Range App has been provisioned.");
        EvRangeApp::GetInstance()->onStart();
    }

    if(EvConfigData::GetInstance()->configData["AppModeHBD"] == ENABLED)
    {
        LOGI("Ev HBD App has been provisioned.");
        EvHBDApp::GetInstance()->onStart();
    }
}

void EvApp::onDestroy()
{
    if(getState() == EVAPP::OFF)
    {
        return;
    }

    releaseVifService();
    releasePwrService();
    releaseNgtpService();
    releaseConfigurationService();
    //releaseDebugService(); //'IDebugManagerService' does not support release API.

    LOGE("Bye JLR");
    setState(EVAPP::OFF);
}

//Public Function
void EvApp::onPostReceived(const sp<Post>& post)
{
    if(getState() == EVAPP::OFF)
    {
        return;
    }

    if(post->what == MODULE_APPLICATION_MGR)
    {
        // triggerDebugCMD(post->arg1);
        sp<sl::Message> message = mHandler->obtainMessage(REVC_SLDD, post->arg1, post->arg2);
        message->sendToTarget();
    }
    else if(post->what == (appid_t)APP_ID_SVT)
    {
        EvVehicleStatus::GetInstance()->setVehicleSVTstatus(post->arg1);
    }
    else
    {
        // TODO: Add other application signal receiver.
    }
}

// PowerStateReceiver
void EvApp::onPowerStateChanged(int32_t newState, int32_t reason)
{
    uint16_t curPowerState = EvVehicleStatus::GetInstance()->getCurPowerState();
    LOGI("EvApp::onPowerStateChanged: [%s](%d) ==> [%s](%d) : reason(%s)",
                                                        power_state_str[curPowerState],
                                                        curPowerState,
                                                        power_state_str[newState],
                                                        newState,
                                                        power_reason_str[reason]);

    EvVehicleStatus::GetInstance()->setCurPowerState(newState);
}
void EvApp::onWakeup(int32_t currentState, int32_t reason)
{
    uint16_t curPowerState = EvVehicleStatus::GetInstance()->getCurPowerState();
    LOGI("EvApp::onWakeup: [%s](%d): %s", power_state_str[curPowerState], curPowerState, power_reason_str[reason]);

    EvVehicleStatus::GetInstance()->setCurPowerState(currentState);
}

//PowerModeReceiver
void EvApp::onPowerModeChanged(int8_t newMode)
{
    LOGV("Evt onPowerModeChanged() newMode:%d", newMode);
    EvConfigData::GetInstance()->configData["POWER_MODE"] = newMode;

    mHandler->obtainMessage(RECV_POWER_MODE, newMode)->sendToTarget();
}

void EvApp::onNotifyFromVifMgr(uint16_t Sigid, sp<Buffer>& buf, bool isInitial)
{
    if(getState() == EVAPP::OFF && !isInitial){
        LOGE("[Error] EvApp is not started yet!, Ignored VifReceiver::onReceive SigId[%d]",Sigid);
        return ;
    }

    if(!(buf->size() > 0 && buf->data() != NULL))
    {
        //EMPTY SIGNAL.
        return ;
    }
    sp<sl::Message> message = mHandler->obtainMessage(RECV_VIFCAN, Sigid);
    message->buffer.setTo(buf->data(), buf->size());
    message->sendToTarget();
}

void EvApp::onNotifyFrameSignalFromVifMgr(uint16_t _FrameID, sp<vifCANFrameSigData>& _FrameSigData, bool isInitial)
{
    LOGE("Receive [%d]Frame", _FrameID);
    if (getState() != EVAPP::IDLE && !isInitial)
    {
        LOGE("[Error] EvApp is not started yet!, Ignored VifReceiver::onNotifyFrameSignalFromVifMgr FrameID[%d]", _FrameID);
        return ;
    }

    if (!(_FrameSigData->mNumOfSigInFrame>0 && _FrameSigData->mFrameSigData!=NULL))
    {
        //EMPTY SIGNAL+
        return ;
    }

    EvCanFrame* tEvCanFrame = new EvCanFrame(_FrameSigData); // CAUTION : Should be deleted at EvAppHandler side.

    sp<sl::Message> message = mHandler->obtainMessage(RECV_VIFCAN_FRAME, (void*)tEvCanFrame);
    message->sendToTarget();

    // for(int index=0; index < _FrameSigData->mNumOfSigInFrame; index++)
    // {
    //     LOG_EV("FrameID:[%d] Index:[%d] Signal:[%d] Data:[%d]",_FrameID, index, _FrameSigData->mFrameSigData[index].sigid, _FrameSigData->mFrameSigData[index].values[0]);
    // }

    // for(auto tCanInfo : tEvCanFrame.mCanList)
    // {
    //     LOGE("<<debug>>Signal:[%d] Data:[%d]",tCanInfo.mCanId, tCanInfo.mCanData[0]);
    // }
}

//NGTPMgrReceiver
void EvApp::onNotifyFromNgtpMgr(InternalNGTPData *pNgtp, int datalength){

    if(getState() == EVAPP::OFF){
        LOGE("[Error] EvApp is not started yet!, Ignored NgtpReceiver::onReceive");
        return ;
    }

    if(pNgtp->encodedSize <= 0 || pNgtp == NULL)
    {
        // Ingore. Invalid message.
        return ;
    }

    LOGV("serviceType = 0x%x, time = 0x%x, encodedsize = %d, messagetype = %d, HEAP_ADDRESS:%u",
                pNgtp->serviceType, pNgtp->creationTime_seconds, pNgtp->encodedSize, pNgtp->messageType, pNgtp);

    sp<sl::Message> message = mHandler->obtainMessage(RECV_NGTP);
    message->buffer.setTo((uint8_t*)pNgtp, datalength); // copy to message buffer.
    message->sendToTarget();
}

void EvApp::onNotifyConfigDataChanged(sp<Buffer>& buf){
    sp<sl::Message> message = mHandler->obtainMessage(REVC_CONFIG);
    if(buf->size() > 0)
    {
        message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

void EvApp::onNotifyFromDebugMgr(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
{
    LOGE("onNotifyFromDebugMgr id:0x%x cmd:0x%x cmd2:0x%x", id, cmd, cmd2);
    sp<sl::Message> message = mHandler->obtainMessage(RECV_DEBUG, id, cmd, cmd2);
    if(buf->size() > 0)
    {
        message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

//Private Internal Function.
extern "C" class Application* createApplication() {
    gApp = new EvApp;
    return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
    delete (EvApp*)application;
}

void EvApp::regSVTmonitor()
{
    mHandler->sendMessageDelayed(/*Message*/mHandler->obtainMessage(/*What*/REG_SVT_MONITOR), /*DelayTime*/500);
}

void EvApp::regVifService()
{
    m_vifMgrService = SET_SERVICE(vifManagerService);
    error_t _result = E_OK;

    //register CAN Signal Monitoring.
    for(uint16_t SigId : EVT_CAN_Monitoring_List){
        sp<vifCANSigRequest> _CurSigReq = new vifCANSigRequest(SigId);
        _result = m_vifMgrService->registerReceiver(SigId, true, m_vifMgrReceiver, vifmgrCANSyncNow_e::Sync_Now, _CurSigReq);
        if(_result != E_OK)
        {
            LOGE("[ERROR] Fail to register VIFMgr Receiver. ErrorCode[%d] Signal[%d]", _result, SigId);
        }
        sp<Buffer> _CurSig = _CurSigReq->getCANSigData();
        onNotifyFromVifMgr(SigId, _CurSig, true);
    }

    //register CAN Frame Monitoring.
    for(uint16_t FrameId : EVT_Frame_Monitoring_List)
    {
        sp<vifCANFrameSigData> _CurFrameReq = new vifCANFrameSigData(FrameId);
        _result = m_vifMgrService->registerReceiverFrame( FrameId, m_vifMgrReceiver, vifmgrCANSyncNow_e::Sync_Now, _CurFrameReq);
        if(_result != E_OK)
        {
            LOGE("[ERROR] Fail to register VIFMgr Receiver. ErrorCode[%d] FrameId[%d]", _result, FrameId);
        }
    }
}
void EvApp::releaseVifService()
{
    //unregister CAN Signal Monitorig.
    for(uint16_t Sigid : EVT_CAN_Monitoring_List){
        m_vifMgrService->unregisterReceiver(Sigid, m_vifMgrReceiver);
    }

    //unregister CAN Frame Monitoring.
    for(uint16_t FrameId : EVT_Frame_Monitoring_List){
        m_vifMgrService->unregisterReceiverFrame(FrameId, m_vifMgrReceiver);
    }
}

error_t EvApp::regConfigurationService()
{
    m_configManagerService = SET_SERVICE(ConfigurationManagerService);

    error_t _result;
    for(auto name : EVT_configDataMonitorList)
    {
        _result = m_configManagerService->registerReceiver(CONFIG_FILE, name.c_str(), m_configManagerReceiver);
        if(_result != E_OK)
        {
            LOGE("[ERROR] Fail to register VIFMgr Receiver. ErrorCode[%d] LookUpName[%s]", _result, name.c_str());
        }
    }
    return E_OK;
}
error_t EvApp::releaseConfigurationService()
{
    for(auto name : EVT_configDataMonitorList)
    {
        m_configManagerService->unregisterReceiver(CONFIG_FILE, name.c_str(), m_configManagerReceiver);
    }
    return E_OK;
}

bool EvApp::checkMdmVariant()
{
    int32_t mdmvariant = EvVehicleStatus::GetInstance()->getMdmVariant();
    LOG_EV("Current TCU3 Board variant is [%d]",mdmvariant);
    if(mdmvariant != VARI_MY18_TELE_ONLY && mdmvariant != VARI_MY18_HIGH)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void EvApp::regPwrService()
{
    m_powerMgrService = SET_SERVICE(PowerManagerService);
    m_powerMgrService->registerPowerModeReceiver(m_powerModeReceiver, true);
    m_powerMgrService->registerPowerStateReceiver(m_powerStateReceiver, 0, true);
}
void EvApp::releasePwrService()
{
    m_powerMgrService->unregisterPowerModeReceiver(m_powerModeReceiver);
    m_powerMgrService->unregisterPowerStateReceiver(m_powerStateReceiver);
}

void EvApp::regNgtpService()
{
    m_ngtpMgrService = SET_SERVICE(NGTPManagerService);
    m_ngtpMgrService->registerReceiver(m_ngtpMgrReceiver, EvtNgtpMsgManager::EVT_CHARGE_PROFILE/*Service Type*/);
    m_ngtpMgrService->registerReceiver(m_ngtpMgrReceiver, EvtNgtpMsgManager::EVT_PRECONDITIONING/*Service Type*/);
}
void EvApp::releaseNgtpService()
{
    m_ngtpMgrService->unregisterReceiver(m_ngtpMgrReceiver, EvtNgtpMsgManager::EVT_CHARGE_PROFILE/*Service Type*/);
    m_ngtpMgrService->unregisterReceiver(m_ngtpMgrReceiver, EvtNgtpMsgManager::EVT_PRECONDITIONING/*Service Type*/);
}

void EvApp::regDebugService()
{
    m_debugMgrService = SET_SERVICE(DebugManagerService);
    m_debugMgrService->attachReceiver((appid_t)APP_ID_REMOTESERVICE, m_DebugReceiver);
}

void EvApp::toStringState(unsigned char state, unsigned char newState)
{
    std::string EvAppState[EVAPP::_END_ENUM_];
    EvAppState[EVAPP::OFF] = "OFF";
    EvAppState[EVAPP::IDLE] = "IDLE";

    if(state < 0 && state >= EVAPP::_END_ENUM_)
    {
        return ;
    }

    if(newState < 0 && newState >= EVAPP::_END_ENUM_)
    {
        return ;
    }

    LOGE("EvApp state is changed : [%s] => [%s]", EvAppState[state].c_str(), EvAppState[newState].c_str());
}