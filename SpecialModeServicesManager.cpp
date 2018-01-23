#include "Log.h"
#include "SpecialModeServicesManager.h"


SpecialModeServicesManager::SpecialModeServicesManager()
 {
    LOGI("## SpecialModeServicesManager created!! ");
    m_AppMgr  = interface_cast<IApplicationManagerService>(defaultServiceManager()
                                                           ->getService(String16("service_layer.ApplicationManagerService")));
// TODO prepare for phase 2

//     m_SystemService = interface_cast<ISystemManagerService>(defaultServiceManager()
//                                                             ->getService(String16("service_layer.SystemManagerService")));

//     m_NGTPMgr = interface_cast<INGTPManagerService>(defaultServiceManager()
//                                                     ->getService(String16("service_layer.NGTPManagerService")));

//     m_ConfigurationMgr = interface_cast<IConfigurationManagerService>(defaultServiceManager()
//                                                                       ->getService(String16("service_layer.ConfigurationManagerService")));

//     m_DebugMgr = interface_cast<IDebugManagerService>(defaultServiceManager()
//                                                      ->getService(String16("service_layer.DebugManagerService")));

//     m_DiagMgr = interface_cast<IDiagManagerService>(defaultServiceManager()
//                                                    ->getService(String16("service_layer.DiagManagerService")));

//     m_PowerMgr = interface_cast<IPowerManagerService>(defaultServiceManager()
//                    ->getService(String16(PowerManagerService::getServiceName())));

//     m_WifiMgr =  interface_cast<IWiFiManagerService>(defaultServiceManager()
//                                                         ->getService(String16("service_layer.WiFiManagerService")));
}

SpecialModeServicesManager::~SpecialModeServicesManager(){

}

sp<IApplicationManagerService> SpecialModeServicesManager::getApplicationManager(){
    LOGV("%s Called...", __func__);
    return m_AppMgr;
}

