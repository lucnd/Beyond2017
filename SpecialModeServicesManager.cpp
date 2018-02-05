#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeServicesManager.h"
#include <Log.h>

SpecialModeServicesManager::SpecialModeServicesManager() : m_ConfigurationMgr(NULL), m_ApplicationMgr(NULL), m_SystemMgr(NULL), m_NGTPMgr(NULL), m_PowerMgr(NULL)
{
    LOGI("## SpecialModeServicesManager created!!");

    m_ConfigurationMgr  = interface_cast<IConfigurationManagerService>(defaultServiceManager()
                                                                        ->getService(String16(CONFIGURATION_SRV_NAME)));
    m_ApplicationMgr    = interface_cast<IApplicationManagerService>(defaultServiceManager()
                                                                        ->getService(String16(APPLICATION_SRV_NAME)));
    m_SystemMgr         = interface_cast<ISystemManagerService>(defaultServiceManager()
                                                                        ->getService(String16(SYSTEM_SRV_NAME)));
    m_NGTPMgr           = interface_cast<INGTPManagerService>(defaultServiceManager()
                                                                        ->getService(String16("NGTP_SRV_NAME")));
    m_PowerMgr          = interface_cast<IPowerManagerService>(defaultServiceManager()
                                                                        ->getService(String16(POWER_SRV_NAME)));
}

SpecialModeServicesManager::~SpecialModeServicesManager(){

}

sp<IConfigurationManagerService> SpecialModeServicesManager::getConfigurationManager(){
    return m_ConfigurationMgr;
}

sp<IApplicationManagerService> SpecialModeServicesManager::getApplicationManager(){
    return m_ApplicationMgr;
}

sp<ISystemManagerService> SpecialModeServicesManager::getSystemManager(){
    return m_SystemMgr;
}

sp<INGTPManagerService> SpecialModeServicesManager::getNGTPManager(){
    return m_NGTPMgr;
}

sp<IPowerManagerService> SpecialModeServicesManager::getPowerManager(){
    return m_PowerMgr;
}
