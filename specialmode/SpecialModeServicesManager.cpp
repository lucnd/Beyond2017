/**
* \file    SpecialModeServicesManager.cpp
* \brief     Declaration of SpecialModeServicesManager
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

#include "SpecialModeServicesManager.h"
#undef LOG_TAG
#define LOG_TAG "SpecialModeServicesManager"
#include "Log.h"
#include "SpecialModeHandler.h"

SpecialModeServicesManager::SpecialModeServicesManager()
    : m_DebugMgr(NULL),m_DiagMgr(NULL),m_PowerMgr(NULL),m_WifiMgr(NULL),m_AppMgr(NULL), m_ConfigurationMgr(NULL){
    LOGI("## SpecialModeServicesManager created!! ");

    m_WifiMgr =  interface_cast<IWifiManagerService>(defaultServiceManager()
                ->getService(String16("service_layer.WifiManagerService")));
    m_DebugMgr = interface_cast<IDebugManagerService>(defaultServiceManager()
                ->getService(String16(DebugManagerService::getServiceName())));
    m_DiagMgr = interface_cast<IDiagManagerService>(defaultServiceManager()
                ->getService(String16(DiagManagerService::getServiceName())));
    m_PowerMgr = interface_cast<IPowerManagerService>(defaultServiceManager()
                    ->getService(String16(PowerManagerService::getServiceName())));
    m_AppMgr = interface_cast<IApplicationManagerService>(defaultServiceManager()
                ->getService(String16("service_layer.ApplicationManagerService")));
    m_ConfigurationMgr = interface_cast<IConfigurationManagerService>(defaultServiceManager()
                ->getService(String16("service_layer.ConfigurationManagerService")));
}

SpecialModeServicesManager::~SpecialModeServicesManager(){

}

sp<IDebugManagerService> SpecialModeServicesManager::getDebugManager(){
    LOGV("%s called...", __func__);
    return m_DebugMgr;
}

sp<IDiagManagerService> SpecialModeServicesManager::getDiagManager(){
    LOGV("%s called...", __func__);
    return m_DiagMgr;
}

sp<IPowerManagerService> SpecialModeServicesManager::getPowerManager(){
    LOGV("%s called...", __func__);
    return m_PowerMgr;
}

sp<IWifiManagerService> SpecialModeServicesManager::getWifiManager(){
    LOGV("%s called...", __func__);
    return m_WifiMgr;
}

sp<IApplicationManagerService> SpecialModeServicesManager::getApplicationManager(){
    LOGV("%s called...", __func__);
    return m_AppMgr;
}

sp<IConfigurationManagerService> SpecialModeServicesManager::getConfigurationManager(){
    LOGV("%s called...", __func__);
    return m_ConfigurationMgr;
}