#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"

#include <utils/Log.h>
#include "Log.h"
#include <utils/Handler.h>


SpecialModeReceiverManager::SpecialModeReceiverManager (sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler) {
    m_ServicesMgr = serviceMgr;
    m_Handler   = handler;

    // register configuration event of other app: bCall, eCall, SVT
    if(m_configManager == NULL) {
        m_configReceiver = new SpecialModeConfigurationReceiver(*this);
        m_configManager = m_ServicesMgr->getConfigurationManager();
        m_configManager->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_SVT, m_configReceiver);
        m_configManager->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_EC,  m_configReceiver);
        m_configManager->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_BC,  m_configReceiver);
    }
}

SpecialModeReceiverManager::~SpecialModeReceiverManager() {
}

void SpecialModeReceiverManager::SpecialModeConfigurationReceiver::onConfigDataChanged(sp<Buffer>& buf) {

    LOGV("%s() onConfigDataChanged(), name_buf:%s",__func__, buf->data());
    sp<sl::Message> messsage = m_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_CONFIG);
    if(buf->size() > 0)
    {
        messsage->buffer.setTo(buf->data(), buf->size());
    }
    messsage->sendToTarget();
}

