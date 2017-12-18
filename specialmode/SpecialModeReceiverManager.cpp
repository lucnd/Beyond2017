/**
* \file    SpecialModeReceiverManager.cpp
* \brief     Declaration of SpecialModeReceiverManager
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.13
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#include "SpecialModeReceiverManager.h"
#undef LOG_TAG
#define LOG_TAG "SpecialModeReceiverManager"
#include "SpecialModeHandler.h"
#include "Log.h"
/**
 * Constructor member function
 */
SpecialModeReceiverManager::SpecialModeReceiverManager(sp<SpecialModeServicesManager> servicesMgr, sp<Handler> handler){
    LOGI("## SpecialModeReceiverManager created!! ");
    if(servicesMgr != NULL) {
        m_ServicesMgr = servicesMgr;
    }
    if(handler != NULL) {
        m_Handler   = handler;
    }

    m_IsInitialize   = false;
}

SpecialModeReceiverManager::~SpecialModeReceiverManager(){
    releaseReceiver();
}

bool SpecialModeReceiverManager::initializeReceiver(){
    LOGI("## initializeReceiver() called!!");
    if(m_Handler == NULL) {
        LOGE("## m_Handler == NULL");
        return false;
    }

    if(m_ServicesMgr == NULL) {
        LOGE("## m_ServicesMgr == NULL");
        return false;
    }

    if(m_IsInitialize == true) {
        return true;
    }

    if(mWifiService == NULL) {
       mWifiService = interface_cast<IWifiManagerService>(defaultServiceManager()->getService(String16("service_layer.WifiManagerService")));
       mWifiReceiver = new SpecialModeWifiReceiver(*this);
       mWifiService->registerWifiEventReceiver(mWifiReceiver,1);
       LOGV("%s  mWifiService!!\n", __func__);
    }

    if(m_configManager == NULL) {
        m_configReceiver = new SpecialModeConfigurationReceiver(*this);
        m_configManager = interface_cast<IConfigurationManagerService>(defaultServiceManager()->getService(String16(ConfigurationManagerService::getServiceName())));
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_SVT, m_configReceiver);
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_EC, m_configReceiver);
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_BC, m_configReceiver);
    }

    m_DebugReceiver = new SpecialModeDebugReceiver(*this);
    m_ServicesMgr->getDebugManager()->attachReceiver(APP_ID_SPECIAL, m_DebugReceiver);
    m_SpecialModeDiagReceiver = new SpecialModeDiagReceiver(*this);
    m_ServicesMgr->getDiagManager()->attachReceiver(APP_ID_SPECIAL, m_SpecialModeDiagReceiver);

    sp<ISystemManagerService> systemService = interface_cast<ISystemManagerService>
                                                                        (defaultServiceManager()->getService(
                                                                            String16(SystemManagerService::getServiceName())));
    m_MdmVariant  = systemService->getMDMvariant();
    /* TELEMATICS_ONLY:0 , MY18_LOW:1, MY18_HIGH:2 */
    LOGI("%s m_MdmVariant[%d]", __func__, m_MdmVariant);
    if(m_MdmVariant != 1 && m_MdmVariant != 2) {
        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, m_MdmVariant);
    }

    m_TelephonyListener = new TelephonyListener(*this);
    int32_t  events = PhoneStateListener::LISTEN_PSIM_LOCK_STATE |
                                PhoneStateListener::LISTEN_PSIM_STATE |
                                PhoneStateListener::LISTEN_ACTIVE_SIM_STATE;

    TelephonyManager::listen(m_TelephonyListener, events, false);

    m_IsInitialize  = true;

    return true;
}

void SpecialModeReceiverManager::releaseReceiver(){
    LOGI("## releaseReceiver() called!!");
    if(m_ServicesMgr != NULL && m_IsInitialize == true) {
        if(m_SpecialModeDiagReceiver != NULL){
            m_ServicesMgr->getDiagManager()->unregisterReceiver(APP_ID_SPECIAL, m_SpecialModeDiagReceiver);
        }
//        if(m_DebugReceiver != NULL) {
//            m_ServicesMgr->getDebugManager()->unregisterReceiver(APP_ID_SPECIAL, m_DebugReceiver);
//        }
        m_IsInitialize  = false;
    }
}


void SpecialModeReceiverManager::setHandler(sp<Handler> handler){
    if(handler == NULL) {
        return;
    }
    m_Handler   = handler;
}

void SpecialModeReceiverManager::SpecialModeDebugReceiver::onReceive(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf){
    LOGI("DebugOnReceive : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%2x",id, cmd, cmd2, buf->size(), buf->data());
    if(mr_ReceiverMgr.m_MdmVariant != 1 && mr_ReceiverMgr.m_MdmVariant != 2) {
        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, mr_ReceiverMgr.m_MdmVariant);
        return;
    }

    sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_DEBUG_COMMAND, cmd, cmd2, id);
    if(buf->size() > 0) {
        messsage->buffer.setTo(buf->data(), buf->size());
    }
    messsage->sendToTarget();

    uint8_t responsePayload[DEBUG_RESPNE_SIZE];
    responsePayload[0] = 0x01;

    sp<Buffer> buff = new Buffer();
    buff->setTo(responsePayload,DEBUG_RESPNE_SIZE);
    mr_ReceiverMgr.m_ServicesMgr->getDebugManager()->responseData(id,DEBUG_CMD,cmd2,buff);
    LOGI("DebugOnRescpne : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%2x",id, cmd, cmd2, buff->size(), buff->data());
}

void SpecialModeReceiverManager::SpecialModeDiagReceiver::onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf){
    LOGI("SpecialModeDiagReceiver : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%4x",id, cmd, cmd2, buf->size(), buf->data());
    if(mr_ReceiverMgr.m_MdmVariant != 1 && mr_ReceiverMgr.m_MdmVariant != 2) {
        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, mr_ReceiverMgr.m_MdmVariant);
        return;
    }

    if(cmd != DIAG_CMD) {
        LOGE("Diag Commnad error cmd=%x cmd2=%x",cmd,cmd2);
        return;
    }
    else if(*(buf->data()) != DIAG_ROUTINEID1 || *(buf->data()+1) != DIAG_ROUTINEID2) {
        LOGE("Diag Commnad error buf[0]=%x buff[1]=%x",*(buf->data()),*(buf->data()+1));
        return;
    } else {
        //
    }

    sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_DIAGNOSTICS_DATA, cmd, cmd2, id);
    if(buf->size() > 0) {
        messsage->buffer.setTo(buf->data(), buf->size());
    }
    messsage->sendToTarget();

    if(cmd2 ==DIAG_REQ_ACTIVATE) {
        uint8_t responsePayload[DIAG_RESPONE_SIZE];
        responsePayload[0] = *(buf->data());
        responsePayload[1] = *(buf->data()+1);
        responsePayload[2] = DIAG_RES_ACTIVATE;

        sp<Buffer> buff = new Buffer();
        buff->setTo(responsePayload,DIAG_RESPONE_SIZE);
        mr_ReceiverMgr.m_ServicesMgr->getDiagManager()->responseDiagcmd(id,DIAG_RESPONE_CMD,cmd2,buff);
        LOGI("SpecialModeDiagRespne : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%4x",id, cmd, cmd2, buf->size(), buf->data());
    }
    else
        LOGI("SpecialModeDiagRespne : others commands => cmd2[%d]", cmd2);

}

void SpecialModeReceiverManager::SpecialModeDiagReceiver::onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf){
    LOGI("%s : %d , %d, m_IsInitialize[%d]", __func__, cmd, cmd2, mr_ReceiverMgr.m_IsInitialize);

    if(mr_ReceiverMgr.m_MdmVariant != 1 && mr_ReceiverMgr.m_MdmVariant != 2) {
        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, mr_ReceiverMgr.m_MdmVariant);
        return;
    }

    if(mr_ReceiverMgr.m_IsInitialize  == true ){
        sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_DIAGNOSTICS_DATA, cmd, cmd2);
        if(buf->size() > 0) {
            messsage->buffer.setTo(buf->data(), buf->size());
        }
        messsage->sendToTarget();
    }
    else{
        LOGE("SpecialMode app un-provisiong state - diag receiver");
    }
}

void SpecialModeReceiverManager::onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause)
{
    //LOGV("%s  ( %d,%d,%d,%d )", __func__, simAct, event, slot, errorCause);
    m_Handler->obtainMessage(RECV_MSG_FROM_TELEPHONY_PSIM_STATE_CHANGED, simAct, event, slot)
            ->sendToTarget();
}

void SpecialModeReceiverManager::onActiveSimStateChanged(int32_t activeSim)
{
    LOGI("%s  activeSim( %d )", __func__, activeSim);
    m_Handler->obtainMessage(RECV_MSG_FROM_TELEPHONY_ACTIVESIM_STATE_CHANGED, activeSim)
            ->sendToTarget();
}

void SpecialModeReceiverManager::onPsimLockStateChanged(bool locked)
{
    LOGI("%s  locked( %d )", __func__, locked);
    m_Handler->obtainMessage(RECV_MSG_FROM_TELEPHONY_PSIM_LOCK_STATE_CHANGED, locked)
            ->sendToTarget();
}

error_t SpecialModeReceiverManager::SpecialModeWifiReceiver::onWifiReceive(int32_t state, sp<Buffer>& payload)
{
      LOGV("onWifiReceive!! state = %d\n", state);
      LOGV("onWifiReceive!! payload = %s\n", payload->data());

      switch(state) {
          case 1:  //WIFI_EVENT_CONNECTED
              {
                mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_WIFI_ON, state)
                        ->sendToTarget();
              }
             break;
          case 2:  //WIFI_EVENT_DISCONNECTED
          case 9:  //WIFI_EVENT_WIFI_OFF
             {
                 mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_WIFI_OFF, state)
                       ->sendToTarget();
             }
            break;
          default:
               break;
      }
      return E_OK;
}

void SpecialModeReceiverManager::SpecialModeConfigurationReceiver::onConfigDataChanged(sp<Buffer>& name) {
    LOGV("%s() onConfigDataChanged(), name_buf:%s",__func__, name->data());

    sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_CONFIG);
    if(name->size() > 0) {
        messsage->buffer.setTo(name->data(), name->size());
    }
    messsage->sendToTarget();
}
