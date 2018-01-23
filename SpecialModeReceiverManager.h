#ifndef SPECIALMODERECEIVERMANAGER_H
#define SPECIALMODERECEIVERMANAGER_H

#include "SpecialModeServicesManager.h"

#include <utils/Handler.h>
#include <utils/Log.h>
#include <ServiceList.h>
#include <services/TelephonyManagerService/base/ITelephonyService.h>
#include <services/TelephonyManagerService/base/IPhoneStateListener.h>
#include <services/TelephonyManagerService/base/TelephonyManager.h>

class SpecialModeReceiverManager
{

public:
    SpecialModeReceiverManager( sp<sl::Handler> handler);
    virtual ~SpecialModeReceiverManager();

    bool initializeReceiver();
    void releaseReceiver();
    void setHandler(sp<sl::Handler> handler);
    // prepare for phase 2
//    void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause);
//    void onPsimLockStateChanged(bool locked);
//    void onActiveSimStateChanged(int32_t activeSim);


private:
    bool m_IsInitialize;
    int32_t m_MdmVariant;
    sp<sl::Handler> m_Handler;
    // TODO prepare for phase 2
//    sp<IMgrReceiver> m_DebugReceiver;
//    sp<SpecialModeServicesManager> m_ServicesMgr;
//    sp<IDiagManagerReceiver> m_SpecialModeDiagReceiver;
//    sp<TelephonyListener> m_TelephonyListener;
//    sp<IWiFiManagerService> mWifiService;
//    sp<IWiFiManagerReceiver> mWifiReceiver;
//    sp<IConfigurationManagerService> m_configManager;
//    sp<IConfigurationManagerReceiver> m_configReceiver;

};

#endif // SPECIALMODERECEIVERMANAGER_H
