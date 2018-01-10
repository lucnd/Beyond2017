#ifndef _SPECIALMODE_APPLICATION_H_
#define _SPECIALMODE_APPLICATION_H_

#include "ISpecialModeApplication.h"

#include <utils/SLLooper.h>
#include <utils/Handler.h>
#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>

class SpecialModeApplication : public ISpecialModeApplication, public Application {
private:
    /**
    * Write your own handler class inside your application class
    */
    // class TemplateHandler : public sl::Handler {
    //     public:
    //         static const uint32_t MSG_GET_MDM_VARIANT = 0;
    //         static const uint32_t MSG_SEND_POST = 1;
    //         static const uint32_t MSG_SEND_HMI = 2;

    //         TemplateHandler(SpecialModeApplication &app) : mApp(app) {}
    //         virtual ~TemplateHandler() {}
    //         virtual void handleMessage(const sp<sl::Message>& msg);
    //     private:
    //         SpecialModeApplication &mApp;
    // };

    /**
     * SystemPostReceiver to receive system post event
     *
     */
    // bool onSystemPostReceived(const sp<Post>& systemPost);

    // class SystemPostReceiver : public BnSystemPostReceiver
    // {
    // public:
    //     SystemPostReceiver(SpecialModeApplication &app) : mApp(app) {}
    //     virtual bool onSystemPostReceived(const sp<Post>& systemPost)
    //     {
    //         return mApp.onSystemPostReceived(systemPost);
    //     }
    // private:
    //         SpecialModeApplication &mApp;
    // };

    // sp<ISystemPostReceiver> mSystemReceiver;


    /**
    * Declare managers to use
    */
	// sp<ISystemManagerService> mSystemManager;
    // sp<IApplicationManagerService> mAppManager;

    /**
    * Declare handler to use
    */
    // sp<TemplateHandler> mHandler;

public:
    SpecialModeApplication();
    virtual ~SpecialModeApplication();

    /**
    * Application has two lifecycle method, onCreate() and onDestroy()
    */
    virtual void onCreate();
    virtual void onDestroy();

    /**
    * Callback method
    */
    virtual void onPostReceived(const sp<Post>& post);
    // virtual void onHMIReceived(const uint32_t type, const uint32_t action);

    /**
    * your own method
    */
    void test();
    virtual void do_SpecialModeHandler(uint32_t what, const sp<sl::Message>& message);
    char* getPropertyWrap(const char* name);
    void setPropertyChar(const char* name, const char* value, bool sync_now);
    void setPropertyInt(const char* name, const int32_t i_value, bool sync_now);


};
#endif // _SPECIALMODE_APPLICATION_H_
