#define LOG_TAG "SpecialModeApplication"
#include <Log.h>
#include <stdio.h>
#include "SpecialModeApplication.h"
#include "DemoModeProcess.h"

#include "HMIType.h"

static android::sp<Application> gApp;


SpecialModeApplication::SpecialModeApplication() : mp_SpecialModeProcess(NULL),mp_ReceiverMgr(NULL),m_AppAlive(false) {

}

SpecialModeApplication::~SpecialModeApplication() {

}

void SpecialModeApplication::onCreate() {

    LOGV("## SpecialMode for JLR TCU4: onCreate() and into in inActive State");

    m_Looper = sl::SLLooper::myLooper();
    if(m_Handler == NULL){
        m_Handler = new SpecialModeHandler(m_Looper, *this);
    }
    LOGI("Pass create handler");

    getServices();

    // TODO need fix: new SpecialModeServicesManager() make crash App
    // if(m_ServicesMgr == NULL) {
    //     m_ServicesMgr   = new SpecialModeServicesManager();
    // }
    // LOGI("pass create service manager");

    if(mp_ReceiverMgr == NULL){
        mp_ReceiverMgr = new SpecialModeReceiverManager(m_Handler);
        mp_ReceiverMgr->initializeReceiver();
    }
    LOGI("Pass create receiver manager");

    m_AppAlive  = true;
    initializeSpecialModeProcess();
}

void SpecialModeApplication::onDestroy() {
    LOGV("onDestroy()");

    // TODO prepare for phase 2
    //  if(mp_SpecialModeProcess != NULL){
    //     delete mp_SpecialModeProcess;
    //     mp_SpecialModeProcess = NULL;
    // }

    // if(mp_ReceiverMgr != NULL){
    //     mp_ReceiverMgr->releaseReceiver();
    //     delete mp_ReceiverMgr;
    //     mp_ReceiverMgr = NULL;
    // }

    ReadyToDestroy();
}

void SpecialModeApplication::getServices() {
    m_AppMgr = interface_cast<IApplicationManagerService>(defaultServiceManager()
                                                          ->getService(String16("service_layer.ApplicationManagerService")));
    m_SystemMgr = interface_cast<ISystemManagerService>(defaultServiceManager()
                                                          ->getService(String16("service_layer.SystemManagerService")));
    m_NGTPMgr = interface_cast<INGTPManagerService>(defaultServiceManager()
                                                          ->getService(String16("service_layer.NGTPManagerService")));
    m_ConfigurationMgr = interface_cast<IConfigurationManagerService>(defaultServiceManager()
                                                          ->getService(String16("service_layer.ConfigurationManagerService")));


    LOGI("## getServices(): m_AppMgr[%s]\n, m_SystemMgr[%s]\n, m_NGTP[%s]\n, m_Configuration[%s]\n",
         m_AppMgr, m_SystemMgr, m_NGTPMgr, m_ConfigurationMgr);
}

void SpecialModeApplication::onPostReceived(const sp<Post>& post) {

    LOGI("SpecialModeApplication::onPostReceived : what[0x%x] arg1[%d] arg2[%d]", post->what, post->arg1, post->arg2);
    sp<Buffer> buf_tmp = new Buffer();
    sp<sl::Message> message = m_Handler->obtainMessage(post->what, post->arg1, post->arg2);
    message->sendToTarget();
}

void SpecialModeApplication::doSpecialModeHandler(uint32_t what, const sp<sl::Message>& message) {
    LOGV("#########doSpecialModeHandler : what[%d],msgwhat[%d],msgarg1[%d]", what, message->what, message->arg1);
    mp_SpecialModeProcess->doSpecialModeHandler(what, message);    // when specialmode process ready
}

void SpecialModeApplication::initializeSpecialModeProcess(){
    LOGI("initialize_SpceialModeProcess() SpecialMode Type");
    mp_SpecialModeProcess = (SpecialModeBaseProcess*) new DemoModeProcess();
    mp_SpecialModeProcess->initialize(this,m_Handler);

    // TODO implement when config file release by JLR vendor
    // if(m_SpecialModeType == IN_CONTROL_LIGHT){
    //     mp_SpecialModeProcess = (SpecialModeBaseProcess*) new InControlLightProcess();
    // }
    // else{
    //     mp_SpecialModeProcess = (SpecialModeBaseProcess*) new DemoModeProcess();
    // }
    //TODO ignore service because issues with them.
    // mp_SpecialModeProcess->initialize(m_ServicesMgr, this, m_Handler);

}

void SpecialModeApplication::releaseSpecialModeProcess(){
    LOGI("release_SpecialModeProcess() called.");
     if(mp_SpecialModeProcess== NULL) {
         return;
     }
     delete mp_SpecialModeProcess;
     mp_SpecialModeProcess = NULL;
}

char* SpecialModeApplication::getPropertyWrap(const char* name){
    if(name == NULL) {
        LOGE("%s  name is NULL !", __func__);
        return NULL;
    }
    return getProperty(name);
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
