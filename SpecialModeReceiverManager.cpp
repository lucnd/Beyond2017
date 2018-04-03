#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"

#include <utils/Log.h>
#include "Log.h"
#include <utils/Handler.h>



#ifdef G_TEST
SpecialModeReceiverManager::SpecialModeReceiverManager() {
#else
SpecialModeReceiverManager::SpecialModeReceiverManager (sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler) {
    LOGI("## SpecialModeReceiverManager created!! ");
    if (serviceMgr != NULL) {
        m_ServicesMgr = serviceMgr;
    }

    if(handler != NULL) {
        m_Handler   = handler;
    }
#endif
    m_IsInitialize   = false;
}


SpecialModeReceiverManager::~SpecialModeReceiverManager() {
    releaseReceiver();
}


SpecialModeReceiverManager::SpecialModeConfigurationReceiver::SpecialModeConfigurationReceiver(SpecialModeReceiverManager& receiverMgr) : m_ReceiverMgr{receiverMgr}
{

}

SpecialModeReceiverManager::SpecialModeConfigurationReceiver::~SpecialModeConfigurationReceiver()
{

}


void SpecialModeReceiverManager::initializeReceiver() {


    // register configuration event
    if(m_configManager == NULL) {
        m_configReceiver = new SpecialModeConfigurationReceiver(*this);
        m_configManager = m_ServicesMgr->getConfigurationManager();
        m_configManager->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_SVT, m_configReceiver);
        m_configManager->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_EC,  m_configReceiver);
        m_configManager->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_BC,  m_configReceiver);
    }

    m_IsInitialize =true;
}

void SpecialModeReceiverManager::releaseReceiver(){
    m_IsInitialize = false;

}

// TODO: change param name => buff
void SpecialModeReceiverManager::SpecialModeConfigurationReceiver::onConfigDataChanged(sp<Buffer>& name) {

#ifdef G_TEST
#else
    LOGV("%s() onConfigDataChanged(), name_buf:%s",__func__, name->data());
#endif
    sp<sl::Message> messsage = m_ReceiverMgr.m_Handler->obtainMessage(RECV_MSG_FROM_CONFIG);
    if(name->size() > 0) 
    {
        messsage->buffer.setTo(name->data(), name->size());
    }
    messsage->sendToTarget();
}

