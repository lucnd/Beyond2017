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


SpecialModeApplication::SpecialModeApplication() : mp_ReceiverMgr(nullptr), mp_SpecialModeProcess(nullptr) { }

SpecialModeApplication::~SpecialModeApplication() { }

void SpecialModeApplication::onCreate() {
    LOGV("## onCreate()");
    m_Looper = sl::SLLooper::myLooper();

    if(m_Handler == NULL){
        m_Handler = new SpecialModeHandler(m_Looper, *this);
    }

    if(m_ServicesMgr == NULL) {
        m_ServicesMgr   = new SpecialModeServicesManager();
    }

    if(mp_ReceiverMgr == NULL){
        mp_ReceiverMgr = new SpecialModeReceiverManager(m_ServicesMgr, m_Handler);
    }

    mp_SpecialModeProcess  = new DemoModeProcess();
}

void SpecialModeApplication::onDestroy() {
    LOGV("##onDestroy()");
    delete mp_SpecialModeProcess;
    mp_SpecialModeProcess = NULL;
    delete mp_ReceiverMgr;
    mp_ReceiverMgr =  NULL;

    ReadyToDestroy();
}

uint32_t SpecialModeApplication::getSpecialModeType() {
    return m_SpecialModeType;
}

void SpecialModeApplication::setSpecialModeType(uint32_t specialModeType) {
    m_SpecialModeType = specialModeType;
}

void SpecialModeApplication::onPostReceived(const sp<Post>& post) {
    sp<sl::Message> message = m_Handler->obtainMessage(post->what,
                                                       post->arg1,
                                                       post->arg2);
    message->sendToTarget();
}

void SpecialModeApplication::doSpecialModeHandler(const sp<sl::Message>& message) {
    mp_SpecialModeProcess->doSpecialModeHandler(message);
}

std::string SpecialModeApplication::getPropertyWrap(const char* name){
    if(name == NULL) {
        return NULL;
    }
    return getProperty(name);
}

void SpecialModeApplication::setPropertyChar(const char* name, const char* value, bool sync_now){
    if(name != NULL) {
        setProperty(name, value, sync_now);
    }
}

void SpecialModeApplication::setPropertyInt(const char* name, const int32_t i_value, bool sync_now){
    if(name != NULL) {
        setProperty(name, i_value, sync_now);
    }
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
