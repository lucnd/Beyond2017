#ifndef _SPECIALMODE_APPLICATION_H_
#define _SPECIALMODE_APPLICATION_H_

#include "ISpecialModeApplication.h"
#include "SpecialModeServicesManager.h"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeBaseProcess.h"
#include "SpecialModeHandler.h"



#include <utils/SLLooper.h>
#include <utils/Handler.h>
#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>

#include <services/NGTPManagerService/INGTPManagerService.h>
#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <services/ConfigurationManagerService/IConfigurationManagerServiceType.h>
#include <services/ConfigurationManagerService/IConfigurationManagerService.h>
#include <services/WiFiManagerService/IWiFiManagerService.h>
#include <services/TelephonyManagerService/base/TelephonyManager.h>

// for power manager
#include <services/PowerManagerService/IPowerManagerService.h>
#include <services/PowerManagerService/IPowerManagerServiceType.h>
#include <PowerState.h>

class SpecialModeApplication : public ISpecialModeApplication, public Application {

public:
    SpecialModeApplication();
    virtual ~SpecialModeApplication();
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onPostReceived(const sp<Post>& post);
    virtual void doSpecialModeHandler(uint32_t what, const sp<sl::Message>& message);
    void releaseSpecialModeProcess();
    void setPropertyChar(const char* name, const char* value, bool sync_now);
    void setPropertyInt(const char* name, const int32_t i_value, bool sync_now);
    char* getPropertyWrap(const char* name);


private:
    sp<sl::SLLooper> m_Looper;
    sp<SpecialModeHandler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    SpecialModeReceiverManager* mp_ReceiverMgr;
    SpecialModeBaseProcess* mp_SpecialModeProcess;

    bool m_AppAlive;
    bool m_ProvisioningFlag;

    void initializeSpecialModeProcess();

    sp<IApplicationManagerService>      m_AppMgr;
    sp<ISystemManagerService>           m_SystemMgr;
    sp<INGTPManagerService>             m_NGTPMgr;
    sp<IWiFiManagerService>             m_WifiMgr;
    sp<IConfigurationManagerService>    m_ConfigurationMgr;
    sp<telephony::ITelephonyService>    m_TelephonyMgr;
    sp<IPowerManagerService>            m_PowerMgr;

    void getServices();

};
#endif // _SPECIALMODE_APPLICATION_H_
