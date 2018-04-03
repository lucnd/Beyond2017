#ifndef _SPECIALMODE_APPLICATION_H_
#define _SPECIALMODE_APPLICATION_H_

#include "ISpecialModeApplication.h"
#include "SpecialModeServicesManager.h"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeBaseProcess.h"
#include "SpecialModeHandler.h"
#include <string>
#include <utils/SLLooper.h>
#include <utils/Handler.h>
#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>


class SpecialModeApplication : public ISpecialModeApplication, public Application {

public:
    SpecialModeApplication();
    virtual ~SpecialModeApplication();
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onPostReceived(const sp<Post>& post);
    virtual void doSpecialModeHandler(const sp<sl::Message>& message);
    void releaseSpecialModeProcess();
    void setPropertyChar(const char* name, const char* value, bool sync_now);
    void setPropertyInt(const char* name, const int32_t i_value, bool sync_now);
    std::string getPropertyWrap(const char* name) override;

    void setSpecialModeType(uint32_t specialModeType);
    virtual uint32_t getSpecialModeType();

#ifdef G_TEST
public:
    static uint32_t                     m_SpecialModeType;
    sp<sl::SLLooper>                    m_Looper;
    SpecialModeReceiverManager*         mp_ReceiverMgr;
    SpecialModeBaseProcess*             mp_SpecialModeProcess;
    bool                                m_AppAlive;
    sp<IApplicationManagerService>      m_AppMgr;
    sp<ISystemManagerService>           m_SystemMgr;
    sp<INGTPManagerService>             m_NGTPMgr;
    sp<IConfigurationManagerService>    m_ConfigurationMgr;
    sp<telephony::ITelephonyService>    m_TelephonyMgr;
    sp<SpecialModeServicesManager>      m_ServicesMgr;

    void initializeSpecialModeProcess();

#else
private:
    static uint32_t                     m_SpecialModeType;
    sp<sl::SLLooper>                    m_Looper;
    sp<SpecialModeHandler>              m_Handler;
    SpecialModeReceiverManager*         mp_ReceiverMgr;
    SpecialModeBaseProcess*             mp_SpecialModeProcess;
    bool                                m_AppAlive;
    sp<IApplicationManagerService>      m_AppMgr;
    sp<ISystemManagerService>           m_SystemMgr;
    sp<INGTPManagerService>             m_NGTPMgr;
    sp<IConfigurationManagerService>    m_ConfigurationMgr;
    sp<telephony::ITelephonyService>    m_TelephonyMgr;
    sp<SpecialModeServicesManager>      m_ServicesMgr;

    void initializeSpecialModeProcess();

#endif

};
#endif // _SPECIALMODE_APPLICATION_H_
