#ifndef _SPECIALMODE_APPLICATION_H_
#define _SPECIALMODE_APPLICATION_H_

#include "ISpecialModeApplication.h"
#include "SpecialModeHandler.h"
#include "SpecialModeReceiverManager.h"
#include "SpecialModeServicesManager.h"

#include <utils/SLLooper.h>
#include <utils/Handler.h>

#include "SpecialModeBaseProcess.h"

#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>


class SpecialModeApplication : public ISpecialModeApplication,  public Application {

public:
    SpecialModeApplication();
    virtual ~SpecialModeApplication();
    virtual void do_SpecialModeHandler(uint32_t what, const sp<sl::Message>& message);
    virtual void onCreate();
    virtual void onDestroy();

    virtual void onPostReceived(const sp<Post>& post);
    virtual void onHMIReceived(const uint32_t type, const uint32_t action);


    void test();

    void release_SpecialModeProcess();
    virtual uint32_t get_SpecialModeType();
    virtual void set_SpecialModeType(uint32_t SpecialModeType);
    char* getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value, bool sync_now);
    void setPropertyInt(const char* name, const int32_t i_value, bool sync_now);

private:
    // variables
    sp<sl::SLLooper> m_Looper;
    sp<SpecialModeHandler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    SpecialModeReceiverManager* mp_ReceiverMgr;
    SpecialModeBaseProcess* mp_SpecialModeProcess;

    bool onSystemPostReceived(const sp<Post>& systemPost);
    bool m_AppAlive;
    bool m_ProvisioningFlag;
    static uint32_t m_SpecialModeType;

    //  function
    void handleEvent(uint32_t ev);
    void initialize_SpecialModeProcess();
    void handleEventProvisioning();
    void handleEventUnprovisioning();
    bool provision_SpecialMode();
    void setProvisioningFlag(bool flag);
    bool unprovision_SpecialMode();
};
#endif // _SPECIALMODE_APPLICATION_H_
