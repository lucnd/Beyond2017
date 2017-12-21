#define LOG_TAG "TemplateApplication"
#include <Log.h>

#include <stdio.h>

#include "TemplateApplication.h"
#include "HMIType.h"

static android::sp<Application> gApp;

TemplateApplication::TemplateApplication() {
}

TemplateApplication::~TemplateApplication() {
}

void TemplateApplication::onCreate() {
	LOGV("onCreate()");
    mSystemManager = interface_cast<ISystemManagerService>
        (defaultServiceManager()->getService(String16("service_layer.SystemManagerService")));
    mAppManager = interface_cast<IApplicationManagerService>
        (defaultServiceManager()->getService(String16("service_layer.ApplicationManagerService")));
    mHandler = new TemplateHandler(*this);
    mHandler->obtainMessage(TemplateHandler::MSG_SEND_POST)->sendToTarget();

    // System Post receiver
    mSystemReceiver = new SystemPostReceiver(*this);
    mAppManager->registerSystemPostReceiver(mSystemReceiver, SYS_POST_ALL);
    mAppManager->registerSystemPostReceiver(mSystemReceiver, SYS_POST_BOOT_COMPLETED | SYS_POST_BOOT_COMPLETED_PRE | SYS_POST_PROVISIONING_COMPLETED);
    LOGV("1. prop.status =%s", getProperty("prop.status"));
    setProperty("prop.status", 0, true);
    LOGV("2. prop.status =%s", getProperty("prop.status"));
//    mAppManager->inquireApplicationStatus(1);
    LOGV("boot complete? %d", mAppManager->getBootCompleted());
}

void TemplateApplication::onDestroy() {
	LOGV("onDestroy()");
    // unregister receiver
    mAppManager->unregisterSystemPostReceiver(mSystemReceiver);
}

void TemplateApplication::onPostReceived(const sp<Post>& post) {
    LOGV("onPostReceived(), what = %d, arg1 = %d, arg2 = %d",
        post->what, post->arg1, post->arg2);
}

void TemplateApplication::onHMIReceived(const uint32_t type, const uint32_t action) {
    LOGV("onHMIReceived(), type = %d, action = %d", type, action);
}

void TemplateApplication::test() {
	printf("test");
}

bool TemplateApplication::onSystemPostReceived(const sp<Post>& systemPost) {
    LOGV("onSystemPostReceived(), what = %d", systemPost->what);
}

/**
* Write your handler code below!
*/
void TemplateApplication::TemplateHandler::handleMessage(const sp<sl::Message>& msg) {
    LOGV("handleMessage: %d", msg->what);
    switch(msg->what) {
/*
        case MSG_GET_MDM_VARIANT: {
            int32_t mdm = mApp.mSystemManager->getSOCvariant();
            LOGV("mdm: %d", mdm);
            obtainMessage(MSG_SEND_HMI)->sendToTarget();
            break;
        }

        case MSG_SEND_HMI: {
            mApp.mAppManager->broadcastHMI(HMI_TYPE_BUTTON_ECALL, HMI_BUTTON_ACTION_PRESSED);
//            obtainMessage(MSG_SEND_POST)->sendToTarget();
            break;
        }
*/
        case MSG_SEND_POST: {
            sp<Post> post = Post::obtain(0);
            post->action = "post.action.SEND_CALL";
            post->what = 0;
            post->arg1 = 1;
            post->arg2 = 2;
            mApp.mAppManager->broadcastPost(post); // send post messaage to specific app by app name
            break;
        }
        default:
            break;
    }
}

#ifdef __cplusplus
extern "C" class Application* createApplication() {
	printf("create TemplateApplication");
	gApp = new TemplateApplication;
	return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
	delete (TemplateApplication*)application;
}
#endif
