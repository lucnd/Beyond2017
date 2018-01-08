#ifndef SPECIALMODESERVICESMANAGER_H
#define SPECIALMODESERVICESMANAGER_H

#include <binder/BinderService.h>
#include "SpecialModeDataType.h"

#include "services/SystemManagerService/ISystemManagerService.h"
#include "services/SystemManagerService/ISystemManagerServiceType.h"

//#include "services/ApplicationManagerService/IApplicationManagerService.h"
#include "services/ApplicationManagerService/IApplicationManagerServiceType.h"

//#include "servicelayer/core/service_layer/include/services/ApplicationManagerService/IApplicationManagerService.h"
//#include "servicelayer/core/service_layer/include/services/ApplicationManagerService/IApplicationManagerServiceType.h"

//#include "services/NGTPManagerService/NGTPManagerService.h"
#include "services/NGTPManagerService/INGTPManagerService.h"

#include "services/WiFiManagerService/IWiFiManagerServiceType.h"
//#include "services/WiFiManagerService/WiFiManagerService.h"

#include "services/DebugManagerService/IDebugManagerServiceType.h"
//#include "services/DebugManagerService/DebugManagerService.h"

//#include "services/PowerManagerService/IPowerManagerServiceType.h"
//#include "services/PowerManagerService/PowerManager.h"

#include "services/DiagManagerService/IDiagManagerServiceType.h"
//#include "services/DiagManagerService/DiagManagerService.h"

#include "services/ConfigurationManagerService/IConfigurationManagerServiceType.h"
//#include "services/ConfigurationManagerService/ConfigurationManagerService.h"

class SpecialModeServicesManager : public RefBase
{
public:
    SpecialModeServicesManager();
    virtual ~SpecialModeServicesManager();

    sp<IDebugManagerService> getDebugManager();
    sp<IDiagManagerService> getDiagManager();
//    sp<IPowerManagerService> getPowerManager();
//    sp<IWiFiManagerService> getWifiManager();
    sp<IApplicationManagerService> getApplicationManager();
    sp<IConfigurationManagerService> getConfigurationManager();

    // add more
    sp<INGTPManagerService> getNgtpManagerService();
    sp<ISystemManagerService> getSystemManagerService();

private:
    sp<IDebugManagerService> m_DebugMgr;
    sp<IDiagManagerService> m_DiagMgr;
//    sp<IPowerManagerService> m_PowerMgr;
//    sp<IWiFiManagerService> m_WifiMgr;
    sp<IApplicationManagerService> m_AppMgr;
    sp<IConfigurationManagerService> m_ConfigurationMgr;

    sp<INGTPManagerService> m_NGTPMgr;
    sp<ISystemManagerService> m_SystemService;
};


#endif // SPECIALMODESERVICESMANAGER_H
