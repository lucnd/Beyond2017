#include "Log.h"
#include "SpecialModeServicesManager.h"


SpecialModeServicesManager::SpecialModeServicesManager()
    : m_DebugMgr(NULL), m_DiagMgr(NULL), m_AppMgr(NULL), m_ConfigurationMgr(NULL), m_SystemService(NULL), m_NGTPMgr(NULL){

    m_DebugMgr = interface_cast<IDebugManagerService>(defaultServiceManager()
                                                      ->getService(String16("service_layer.DebugManagerService")));

    m_DiagMgr = interface_cast<IDiagManagerService>(defaultServiceManager()
                                                    ->getService(String16("service_layer.DiagManagerService")));

    m_ConfigurationMgr = interface_cast<IConfigurationManagerService>(defaultServiceManager()
                                                                      ->getService(String16("service_layer.ConfigurationManagerService")));

    m_AppMgr  = interface_cast<IApplicationManagerService>(defaultServiceManager()
                                                           ->getService(String16(APPLICATION_SRV_NAME)));

    m_NGTPMgr = interface_cast<INGTPManagerService>(defaultServiceManager()
                                                    ->getService(String16("service_layer.NGTPManagerService")));

    m_AppMgr = interface_cast<IApplicationManagerService>(defaultServiceManager()
                                                          ->getService(String16("service_layer.ApplicationManagerService")));

    m_SystemService = interface_cast<ISystemManagerService>(defaultServiceManager()
                                                            ->getService(String16("service_layer.SystemManagerService")));

//    m_PowerMgr = interface_cast<IPowerManagerService>(defaultServiceManager()
//                    ->getService(String16(PowerManagerService::getServiceName())));

//        m_WifiMgr =  interface_cast<IWiFiManagerService>(defaultServiceManager()
//                                                         ->getService(String16("service_layer.WiFiManagerService")));


}
