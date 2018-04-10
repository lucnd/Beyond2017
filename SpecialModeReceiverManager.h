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
    class SpecialModeConfigurationReceiver : public BnConfigurationManagerReceiver {

    public:
        SpecialModeConfigurationReceiver(SpecialModeReceiverManager& receiverMgr);
        virtual ~SpecialModeConfigurationReceiver();
        virtual void onConfigDataChanged(sp<Buffer>& buf);

    private:
        SpecialModeReceiverManager& m_ReceiverMgr;
    };

public:
    SpecialModeReceiverManager( sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler);
    virtual ~SpecialModeReceiverManager();

private:
    sp<sl::Handler>                     m_Handler;
    sp<SpecialModeServicesManager>      m_ServicesMgr;
    sp<IConfigurationManagerService>    m_configManager;
    sp<IConfigurationManagerReceiver>   m_configReceiver;
};

#endif // SPECIALMODERECEIVERMANAGER_H
