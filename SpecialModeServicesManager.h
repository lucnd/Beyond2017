#ifndef SPECIALMODESERVICESMANAGER_H
#define SPECIALMODESERVICESMANAGER_H


#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>
// #include <services/SystemManagerService/ISystemManagerService.h>

// #include "services/SystemManagerService/ISystemManagerService.h"
// #include "services/SystemManagerService/ISystemManagerServiceType.h"

//#include "services/ApplicationManagerService/IApplicationManagerService.h"
// #include "services/ApplicationManagerService/IApplicationManagerServiceType.h"

// #include <services/ApplicationManagerService/IApplicationManagerService.h>

//#include "services/NGTPManagerService/NGTPManagerService.h"
// #include "services/NGTPManagerService/INGTPManagerService.h"

// #include "services/WiFiManagerService/IWiFiManagerServiceType.h"
//#include "services/WiFiManagerService/WiFiManagerService.h"

// #include "services/DebugManagerService/IDebugManagerServiceType.h"
//#include "services/DebugManagerService/DebugManagerService.h"

// #include "services/DiagManagerService/IDiagManagerServiceType.h"
//#include "services/DiagManagerService/DiagManagerService.h"

// #include "services/ConfigurationManagerService/IConfigurationManagerServiceType.h"
//#include "services/ConfigurationManagerService/ConfigurationManagerService.h"

class SpecialModeServicesManager : public RefBase
{
public:
    SpecialModeServicesManager();
    virtual ~SpecialModeServicesManager();

    sp<IApplicationManagerService> getApplicationManager();
//     sp<IDebugManagerService> getDebugManager();
//     sp<IDiagManagerService> getDiagManager();
//    sp<IPowerManagerService> getPowerManager();
//    sp<IWiFiManagerService> getWifiManager();
//     sp<IConfigurationManagerService> getConfigurationManager();
//     sp<INGTPManagerService> getNgtpManagerService();
//     sp<ISystemManagerService> getSystemManagerService();

private:
    sp<IApplicationManagerService> m_AppMgr;
//     sp<IDebugManagerService> m_DebugMgr;
//     sp<IDiagManagerService> m_DiagMgr;
//    sp<IPowerManagerService> m_PowerMgr;
//    sp<IWiFiManagerService> m_WifiMgr;
//     sp<ISystemManagerService> m_SystemService;
//     sp<IConfigurationManagerService> m_ConfigurationMgr;
//     sp<INGTPManagerService> m_NGTPMgr;
};


#endif // SPECIALMODESERVICESMANAGER_H
