#ifndef _TEMPLATE_APPLICATION_H_
#define _TEMPLATE_APPLICATION_H_

#include <utils/SLLooper.h>
#include <utils/Handler.h>
#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>

class TemplateApplication : public Application {
private:
    /**
    * Write your own handler class inside your application class
    */
    class TemplateHandler : public sl::Handler {
        public:
            static const uint32_t MSG_GET_MDM_VARIANT = 0;
            static const uint32_t MSG_SEND_POST = 1;
            static const uint32_t MSG_SEND_HMI = 2;

            TemplateHandler(TemplateApplication &app) : mApp(app) {}
            virtual ~TemplateHandler() {}
            virtual void handleMessage(const sp<sl::Message>& msg);
        private:
            TemplateApplication &mApp;
    };

    /**
     * SystemPostReceiver to receive system post event
     *
     */
    bool onSystemPostReceived(const sp<Post>& systemPost);

    class SystemPostReceiver : public BnSystemPostReceiver
    {
    public:
        SystemPostReceiver(TemplateApplication &app) : mApp(app) {}
        virtual bool onSystemPostReceived(const sp<Post>& systemPost)
        {
            return mApp.onSystemPostReceived(systemPost);
        }
    private:
            TemplateApplication &mApp;
    };

    sp<ISystemPostReceiver> mSystemReceiver;


    /**
    * Declare managers to use
    */
	sp<ISystemManagerService> mSystemManager;
    sp<IApplicationManagerService> mAppManager;

    /**
    * Declare handler to use
    */
    sp<TemplateHandler> mHandler;

public:
    TemplateApplication();
    virtual ~TemplateApplication();

    /**
    * Application has two lifecycle method, onCreate() and onDestroy()
    */
    virtual void onCreate();
    virtual void onDestroy();

    /**
    * Callback method
    */
    virtual void onPostReceived(const sp<Post>& post);
    virtual void onHMIReceived(const uint32_t type, const uint32_t action);

    /**
    * your own method
    */
    void test();
};
#endif // _TEMPLATE_APPLICATION_H_
