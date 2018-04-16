#ifndef SPECIALMODERECEIVERMANAGER_H
#define SPECIALMODERECEIVERMANAGER_H

#include "SpecialModeServicesManager.h"

#include <utils/Handler.h>
#include <utils/Log.h>
#include <ServiceList.h>
#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <services/ConfigurationManagerService/IConfigurationManagerService.h>

class SpecialModeReceiverManager
{
public:
    SpecialModeReceiverManager(sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler);
    virtual ~SpecialModeReceiverManager();

private:
    sp<SpecialModeServicesManager>      m_ServicesMgr;
    sp<IConfigurationManagerReceiver>   m_configReceiver;
};

#endif // SPECIALMODERECEIVERMANAGER_H
