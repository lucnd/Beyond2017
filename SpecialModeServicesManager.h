#ifndef SPECIALMODESERVICESMANAGER_H
#define SPECIALMODESERVICESMANAGER_H

#include <binder/BinderService.h>


#include <services/ConfigurationManagerService/IConfigurationManagerService.h>
#include <services/ConfigurationManagerService/IConfigurationManagerServiceType.h>

#include <services/ApplicationManagerService/IApplicationManagerService.h>
#include <services/ApplicationManagerService/IApplicationManagerServiceType.h>

#include <services/NGTPManagerService/INGTPManagerService.h>
#include <services/NGTPManagerService/INGTPManagerServiceType.h>

#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/SystemManagerService/ISystemManagerServiceType.h>

#include "services/PowerManagerService/IPowerManagerServiceType.h"
#include "services/PowerManagerService/IPowerManagerService.h"

class SpecialModeServicesManager : public RefBase
{
public:
    SpecialModeServicesManager();
    virtual ~SpecialModeServicesManager();

    sp<IConfigurationManagerService>    getConfigurationManager();
    sp<IApplicationManagerService>      getApplicationManager();
    sp<ISystemManagerService>           getSystemManager();
    sp<INGTPManagerService>             getNGTPManager();
    sp<IPowerManagerService>            getPowerManager();

private:
    sp<IConfigurationManagerService>    m_ConfigurationMgr;
    sp<IApplicationManagerService>      m_ApplicationMgr;
    sp<ISystemManagerService>           m_SystemMgr;
    sp<INGTPManagerService>             m_NGTPMgr;
    sp<IPowerManagerService>            m_PowerMgr;
};


#endif // SPECIALMODESERVICESMANAGER_H
