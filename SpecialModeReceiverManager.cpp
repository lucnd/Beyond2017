#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"
#include "SpecialModeConfigurationReceiver.h"

#include <utils/Log.h>
#include "Log.h"
#include <utils/Handler.h>


SpecialModeReceiverManager::SpecialModeReceiverManager(sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler) :
    m_ServicesMgr(serviceMgr),
    m_configReceiver(new SpecialModeConfigurationReceiver(handler))
{
    auto configMan = serviceMgr->getConfigurationManager();
    configMan->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_SVT, m_configReceiver);
    configMan->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_EC,  m_configReceiver);
    configMan->registerReceiver(SPECIALMODE_CONFIG_FILE, DEMOMODE_APP_MODE_BC,  m_configReceiver);
    LOGI("## SpecialModeReceiverManager created!! ");
}

SpecialModeReceiverManager::~SpecialModeReceiverManager() {}
