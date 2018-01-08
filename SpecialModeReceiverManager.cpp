#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"

#include <utils/Log.h>
#include "Log.h"
#include <utils/Handler.h>


SpecialModeReceiverManager::SpecialModeReceiverManager (sp<SpecialModeServicesManager> servicesMgr, sp<sl::Handler> handler) {
    LOGI("## SpecialModeReceiverManager created!! ");
    if(servicesMgr != NULL) {
        m_ServicesMgr = servicesMgr;
    }
    if(handler != NULL) {
        m_Handler   = handler;
    }

    m_IsInitialize   = false;
}

SpecialModeReceiverManager::~SpecialModeReceiverManager() {
    releaseReceiver();
}

bool SpecialModeReceiverManager::initializeReceiver() {
    LOGI("## initializeReceiver() called!!");

    if(m_Handler == NULL)
        return false;
    if(m_ServicesMgr == NULL)
        return false;
    if(m_IsInitialize == true)
        return true;

//    if(mWifiService == NULL) {
//       mWifiService = interface_cast<IWifiManagerService>(defaultServiceManager()->getService(String16("service_layer.WifiManagerService")));
//       mWifiReceiver = new SpecialModeWifiReceiver(*this);
//       mWifiService->registerWifiEventReceiver(mWifiReceiver,1);
//       LOGV("%s  mWifiService!!\n", __func__);
//    }

    if(m_configManager == NULL) {
        m_configReceiver = new SpecialModeConfigurationReceiver(*this);
        m_configManager = interface_cast<IConfigurationManagerService>
                (defaultServiceManager()->getService(String16("service_layer.ConfigurationManagerService")));
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_SVT, m_configReceiver);
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_EC, m_configReceiver);
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_BC, m_configReceiver);
    }

//    m_DebugReceiver = new SpecialModeDebugReceiver(*this);
//    m_ServicesMgr->getDebugManager()->attachReceiver(APP_ID_SPECIAL, m_DebugReceiver);
//    m_SpecialModeDiagReceiver = new SpecialModeDiagReceiver(*this);
//    m_ServicesMgr->getDiagManager()->attachReceiver(APP_ID_SPECIAL, m_SpecialModeDiagReceiver);

    sp<ISystemManagerService> systemService = interface_cast<ISystemManagerService>
            (defaultServiceManager()->getService(String16("service_layer.SystemManagerService")));
    m_MdmVariant = systemService->getVariantVersion();
    if(m_MdmVariant != 1 && m_MdmVariant != 2) {
        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, m_MdmVariant);
    }

    m_IsInitialize =true;
    return true;
}

void SpecialModeReceiverManager::releaseReceiver(){
    m_IsInitialize = false;
}

void SpecialModeReceiverManager::setHandler(sp<sl::Handler> handler) {
    if(handler == NULL) {
        return;
    }
    m_Handler = handler;
}

void SpecialModeReceiverManager::SpecialModeDebugReceiver::onReceive(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer> &buf) {
    //    if(mr_ReceiverMgr.m_MdmVariant != 1 && mr_ReceiverMgr.m_MdmVariant != 2) {
    //        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, mr_ReceiverMgr.m_MdmVariant);
    //        return;
    //    }

    //    sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_DEBUG_COMMAND, cmd, cmd2, id);
    //    if(buf->size() > 0) {
    //        messsage->buffer.setTo(buf->data(), buf->size());
    //    }
    //    messsage->sendToTarget();

    //    uint8_t responsePayload[DEBUG_RESPNE_SIZE];
    //    responsePayload[0] = 0x01;

    //    sp<Buffer> buff = new Buffer();
    //    buff->setTo(responsePayload,DEBUG_RESPNE_SIZE);
    //    mr_ReceiverMgr.m_ServicesMgr->getDebugManager()->responseData(id,DEBUG_CMD,cmd2,buff);
    //    LOGI("DebugOnRescpne : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%2x",id, cmd, cmd2, buff->size(), buff->data());
}

void SpecialModeReceiverManager::SpecialModeDiagReceiver::onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer> &buf) {
//    LOGI("SpecialModeDiagReceiver : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%4x",id, cmd, cmd2, buf->size(), buf->data());
//    if(mr_ReceiverMgr.m_MdmVariant != 1 && mr_ReceiverMgr.m_MdmVariant != 2) {
//        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, mr_ReceiverMgr.m_MdmVariant);
//        return;
//    }

//    if(cmd != DIAG_CMD) {
//        LOGE("Diag Commnad error cmd=%x cmd2=%x",cmd,cmd2);
//        return;
//    }
//    else if(*(buf->data()) != DIAG_ROUTINEID1 || *(buf->data()+1) != DIAG_ROUTINEID2) {
//        LOGE("Diag Commnad error buf[0]=%x buff[1]=%x",*(buf->data()),*(buf->data()+1));
//        return;
//    } else {
//        //
//    }

//    sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_DIAGNOSTICS_DATA, cmd, cmd2, id);
//    if(buf->size() > 0) {
//        messsage->buffer.setTo(buf->data(), buf->size());
//    }
//    messsage->sendToTarget();

//    if(cmd2 ==DIAG_REQ_ACTIVATE) {
//        uint8_t responsePayload[DIAG_RESPONE_SIZE];
//        responsePayload[0] = *(buf->data());
//        responsePayload[1] = *(buf->data()+1);
//        responsePayload[2] = DIAG_RES_ACTIVATE;

//        sp<Buffer> buff = new Buffer();
//        buff->setTo(responsePayload,DIAG_RESPONE_SIZE);
//        mr_ReceiverMgr.m_ServicesMgr->getDiagManager()->responseDiagcmd(id,DIAG_RESPONE_CMD,cmd2,buff);
//        LOGI("SpecialModeDiagRespne : id:%d, cmd:%d, cmd2:%d, size:%d, data:0x%4x",id, cmd, cmd2, buf->size(), buf->data());
//    }
//    else
//        LOGI("SpecialModeDiagRespne : others commands => cmd2[%d]", cmd2);
}

void SpecialModeReceiverManager::SpecialModeDiagReceiver::onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer> &buf) {
//    if(mr_ReceiverMgr.m_MdmVariant != 1 && mr_ReceiverMgr.m_MdmVariant != 2) {
//        LOGE("%s ### The Demo Mode is not supported because of mdm vairant is [%d]. ###", __func__, mr_ReceiverMgr.m_MdmVariant);
//        return;
//    }

//    if(mr_ReceiverMgr.m_IsInitialize  == true ){
//        sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_DIAGNOSTICS_DATA, cmd, cmd2);
//        if(buf->size() > 0) {
//            messsage->buffer.setTo(buf->data(), buf->size());
//        }
//        messsage->sendToTarget();
//    }
//    else{
//        LOGE("SpecialMode app un-provisiong state - diag receiver");
//    }
}

//error_t SpecialModeReceiverManager::SpecialModeWifiReceiver::onWifiReceive(int32_t state, sp<Buffer> &payload) {
//    switch (state) {
//    case 1:     // wifi_event_connected
//    {
//        mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_WIFI_ON, state)
//                ->sendToTarget();
//    }
//        break;
//    case 2:     // wifi_event_disconnected    -> keep demo mode runing
//    {

//    }
//    case 9:     // wifi_event_wifi_off
//    {
//        mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_WIFI_OFF, state)
//              ->sendToTarget();
//    }

//    default:
//        break;
//    }

//    return E_OK;
//}

void SpecialModeReceiverManager::SpecialModeConfigurationReceiver::onConfigDataChanged(sp<Buffer> &buf) {
    sp<sl::Message> messages = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_CONFIG);
    if(buf->size() > 0) {
        messages->buffer.setTo(buf->data(), buf->size());
    }
    messages->sendToTarget();
}

void SpecialModeReceiverManager::onActiveSimStateChanged(int32_t activeSim) {

}

#undef LOG_TAG
#define LOG_TAG "SpecialModeReceiverManager"



