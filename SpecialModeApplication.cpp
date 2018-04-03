#define LOG_TAG "SpecialModeApplication"
#include <Log.h>
#include <stdio.h>
#include "SpecialModeApplication.h"
#include "DemoModeProcess.h"
#include <string>
#include "HMIType.h"
#include <inttypes.h>

static android::sp<Application>                         gApp;
uint32_t SpecialModeApplication::m_SpecialModeType      = 0;


SpecialModeApplication::SpecialModeApplication() : mp_SpecialModeProcess(nullptr),
    mp_ReceiverMgr(nullptr), m_AppAlive(nullptr) { }

SpecialModeApplication::~SpecialModeApplication() { }

void SpecialModeApplication::onCreate() {
    LOGV("## onCreate()");
#ifdef G_TEST
#else
    m_Looper = sl::SLLooper::myLooper();
    if(m_Handler == NULL){
        m_Handler = new SpecialModeHandler(m_Looper, *this);
    }
#endif
    if(m_ServicesMgr == NULL) {
        m_ServicesMgr   = new SpecialModeServicesManager();
    }

#ifdef G_TEST
    mp_ReceiverMgr = new SpecialModeReceiverManager();
    mp_ReceiverMgr->initializeReceiver();
#else
    if(mp_ReceiverMgr == NULL){
#ifdef G_TEST
        mp_ReceiverMgr = new SpecialModeReceiverManager();
#else
        mp_ReceiverMgr = new SpecialModeReceiverManager(m_ServicesMgr, m_Handler);
#endif
        mp_ReceiverMgr->initializeReceiver();
    }
#endif
    m_AppAlive  = true;

    initializeSpecialModeProcess();
}

void SpecialModeApplication::onDestroy() {
    LOGV("## onDestroy()++");
    releaseSpecialModeProcess();
    ReadyToDestroy();
}

uint32_t SpecialModeApplication::getSpecialModeType() {
    return m_SpecialModeType;
}

void SpecialModeApplication::setSpecialModeType(uint32_t specialModeType) {
    m_SpecialModeType = specialModeType;
}

void SpecialModeApplication::onPostReceived(const sp<Post>& post) {
#ifdef G_TEST
#else
    LOGI("SpecialModeApplication::onPostReceived : what[0x%x] arg1[%d] arg2[%d]", post->what, post->arg1, post->arg2);
    sp<Buffer> buf_tmp = new Buffer();
    sp<sl::Message> message = m_Handler->obtainMessage(post->what, post->arg1, post->arg2);
    message->sendToTarget();
#endif
}

void SpecialModeApplication::doSpecialModeHandler(const sp<sl::Message>& message) {
#ifdef G_TEST
#else
    mp_SpecialModeProcess->doSpecialModeHandler(message);
#endif
}

void SpecialModeApplication::initializeSpecialModeProcess(){
    mp_SpecialModeProcess = (SpecialModeBaseProcess*) new DemoModeProcess();
    mp_SpecialModeProcess->initialize(m_ServicesMgr, this);
}

void SpecialModeApplication::releaseSpecialModeProcess(){
    delete mp_SpecialModeProcess;
    mp_SpecialModeProcess = NULL;

    if(mp_ReceiverMgr != NULL){
        mp_ReceiverMgr->releaseReceiver();
        delete mp_ReceiverMgr;
        mp_ReceiverMgr = NULL;
    }
}

std::string SpecialModeApplication::getPropertyWrap(const char* name){
    // if(name == NULL) {
    //     LOGE("%s  name is NULL !", __func__);
    //     return NULL;
    // }
    // return getProperty(name);
    return std::string("-1");
}

void SpecialModeApplication::setPropertyChar(const char* name, const char* value, bool sync_now){
    if(name != NULL) {
        LOGV("Before: %s name[%s], value[%s]", __func__, name, value);
        setProperty(name, value, sync_now);
    }
    LOGV("After: %s", __func__);
}

void SpecialModeApplication::setPropertyInt(const char* name, const int32_t i_value, bool sync_now){
    if(name != NULL) {
        LOGV("Before: %s name[%s], i_value[%d]", __func__, name, i_value);
        setProperty(name, i_value, sync_now);
    }
    LOGV("After: %s", __func__);
}

#ifdef __cplusplus
extern "C" class Application* createApplication() {
    printf("create SpecialModeApplication");
    gApp = new SpecialModeApplication;
    return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
    delete (SpecialModeApplication*)application;
}
#endif
