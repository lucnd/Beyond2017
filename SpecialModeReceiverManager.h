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
    /******************** Declare Internal Class *****************************/
    /*Receiver event form Configuration Manager Service*/
    class SpecialModeConfigurationReceiver : public BnConfigurationManagerReceiver {
        public:
            SpecialModeConfigurationReceiver(SpecialModeReceiverManager& rReceiverMgr) : mr_ReceiverMgr(rReceiverMgr) { }
            virtual ~SpecialModeConfigurationReceiver() {}
            virtual void onConfigDataChanged(sp<Buffer>& buf);

        private:
            SpecialModeReceiverManager& mr_ReceiverMgr;
    };

    /*Receiver event from WiFiManager Service*/
    class SpecialModeWiFiReceiver : public BnWiFiManagerService
    {
        public:
            SpecialModeWiFiReceiver(SpecialModeReceiverManager& rReceiverMgr) : mr_ReceiverMgr(rReceiverMgr) { }
            virtual ~SpecialModeWiFiReceiver(){}
            virtual errno_t onWiFiReceiver(int32_t state, sp<Buffer>& playload);
        private:
            SpecialModeServicesManager& mr_ReceiverMgr;
    };
    /******************** End declare internal class ********************************/

public:
    SpecialModeReceiverManager( sp<SpecialModeServicesManager> serviceMgr, sp<sl::Handler> handler);
    virtual ~SpecialModeReceiverManager();
    bool initializeReceiver();
    void releaseReceiver();
    void setHandler(sp<sl::Handler> handler);

private:
    bool                                m_IsInitialize;
    int32_t                             m_MdmVariant;
    sp<sl::Handler>                     m_Handler;
    sp<SpecialModeServicesManager>      m_ServicesMgr;
    sp<IConfigurationManagerService>    m_configManager;
    sp<IConfigurationManagerReceiver>   m_configReceiver;
//    sp<IWiFiManagerService>             m_WiFiService;
//    sp<IWiFiManagerReceiver>            m_WiFiReceiver;
};

#endif // SPECIALMODERECEIVERMANAGER_H
