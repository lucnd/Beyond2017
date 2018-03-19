#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"

#include <utils/Log.h>
#include "Log.h"
#include <utils/Handler.h>


SpecialModeReceiverManager::SpecialModeReceiverManager (sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler) {
    LOGI("## SpecialModeReceiverManager created!! ");

    if (serviceMgr != NULL) {
        m_ServicesMgr = serviceMgr;
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

    if(m_ServicesMgr == NULL) {
        LOGE("## m_Service == NULL");
         return false;
    }

    if(m_Handler == NULL) {
        LOGE("## m_Handler == NULL");
         return false;
    }

    if(m_IsInitialize == true) {

        return true;
    }
    // register configuration event
    if(m_configManager == NULL) {
        m_configReceiver = new SpecialModeConfigurationReceiver(*this);
        m_configManager = m_ServicesMgr->getConfigurationManager();
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_SVT, m_configReceiver);
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_EC,  m_configReceiver);
        m_configManager->registerReceiver(SPCIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_BC,  m_configReceiver);
    }

    // register wifi state event
    if(m_WiFiService == NULL) {
        // TODO
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

void SpecialModeReceiverManager::SpecialModeConfigurationReceiver::onConfigDataChanged(sp<Buffer>& name) {
    LOGV("%s() onConfigDataChanged(), name_buf:%s",__func__, name->data());

    sp<sl::Message> messsage = mr_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_CONFIG);
    if(name->size() > 0) {
        messsage->buffer.setTo(name->data(), name->size());
    }
    messsage->sendToTarget();
}

#undef LOG_TAG
#define LOG_TAG "SpecialModeReceiverManager"
