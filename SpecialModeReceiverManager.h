#ifndef SPECIALMODERECEIVERMANAGER_H
#define SPECIALMODERECEIVERMANAGER_H

#include "SpecialModeServicesManager.h"

#include <utils/Handler.h>
#include <utils/Log.h>
#include <ServiceList.h>
#include <services/TelephonyManagerService/base/ITelephonyService.h>
#include <services/TelephonyManagerService/base/IPhoneStateListener.h>
#include <services/TelephonyManagerService/base/TelephonyManager.h>
#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <services/ConfigurationManagerService/IConfigurationManagerService.h>

class SpecialModeReceiverManager
{

    /*Receiver event form Configuration Manager Service*/

#ifdef G_TEST
    public:
    class SpecialModeConfigurationReceiver : public BnConfigurationManagerReceiver {
            public:
            SpecialModeConfigurationReceiver(SpecialModeReceiverManager& receiverMgr);
            virtual ~SpecialModeConfigurationReceiver();
            virtual void onConfigDataChanged(sp<Buffer>& buf);

        private:
            SpecialModeReceiverManager& m_ReceiverMgr;
    };
#else
    class SpecialModeConfigurationReceiver : public BnConfigurationManagerReceiver {

        public:
            SpecialModeConfigurationReceiver(SpecialModeReceiverManager& receiverMgr);
            virtual ~SpecialModeConfigurationReceiver();
            virtual void onConfigDataChanged(sp<Buffer>& buf);

        private:
            SpecialModeReceiverManager& m_ReceiverMgr;
    };
#endif

public:
#ifdef G_TEST
    SpecialModeReceiverManager();
#else
    SpecialModeReceiverManager( sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler);
#endif
    virtual ~SpecialModeReceiverManager();

    void initializeReceiver();
    void releaseReceiver();

private:
    bool                                m_IsInitialize;
    sp<sl::Handler>                     m_Handler;
#ifdef G_TEST
    public:
    sp<SpecialModeServicesManager>      m_ServicesMgr;
    sp<IConfigurationManagerService>    m_configManager;
    sp<IConfigurationManagerReceiver>   m_configReceiver;
#else
    sp<SpecialModeServicesManager>      m_ServicesMgr;
    sp<IConfigurationManagerService>    m_configManager;
    sp<IConfigurationManagerReceiver>   m_configReceiver;
#endif

};

#endif // SPECIALMODERECEIVERMANAGER_H
