#define LOG_TAG "SpecialModeApplication"
#include <Log.h>
#include <stdio.h>
#include "SpecialModeApplication.h"
#include "DemoModeProcess.h"
#include "InControlLightProcess.h"
#include "SpecialModeType.h"

#include "HMIType.h"

#include <inttypes.h>



static android::sp<Application> gApp;

uint32_t SpecialModeApplication::m_SpecialModeType = 0;


SpecialModeApplication::SpecialModeApplication() : mp_SpecialModeProcess(NULL),mp_ReceiverMgr(NULL),m_AppAlive(false) {

}

SpecialModeApplication::~SpecialModeApplication() {

}

void SpecialModeApplication::onCreate() {

    LOGV("## SpecialMode for JLR TCU4: onCreate() and into in inActive State: test PowerManagerService");

    m_Looper = sl::SLLooper::myLooper();

    if(m_Handler == NULL){
        m_Handler = new SpecialModeHandler(m_Looper, *this);
    }

     if(m_ServicesMgr == NULL) {
        m_ServicesMgr   = new SpecialModeServicesManager();
     }

    if(mp_ReceiverMgr == NULL){
        mp_ReceiverMgr = new SpecialModeReceiverManager(m_ServicesMgr, m_Handler);
        mp_ReceiverMgr->initializeReceiver();
    }

    m_AppAlive  = true;

//    initializeSpecialModeProcess();    // if no need provisioning
    doAfterProvision();

}

void SpecialModeApplication::onDestroy() {
    LOGV("onDestroy()");

    doAfterUnprovision();

    if(mp_SpecialModeProcess != NULL){
        delete mp_SpecialModeProcess;
        mp_SpecialModeProcess = NULL;
    }
    if(mp_ReceiverMgr != NULL){
        mp_ReceiverMgr->releaseReceiver();
        delete mp_ReceiverMgr;
        mp_ReceiverMgr = NULL;
    }

    ReadyToDestroy();
}

bool SpecialModeApplication::provisionSpecialMode() {
    LOGI("## Starting provision SpecialMode");
    uint32_t configVal = 0;
    configVal = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE,2, SPECIALMODE_INCONTROL);
    LOGI("configVal = %08x", configVal);
    if((configVal >> INCONTROL_TYPE_WEIGHT) & 0x01) {
        setSpecialModeType(E_IN_CONTROL_LIGHT);
    }else {
        setSpecialModeType(E_DEMOMODE);
    }
    LOGI("## Result after provision SpecialMode: %d ", m_SpecialModeType);
    return true;
}

bool SpecialModeApplication::unprovisionSpecialMode() {
    LOGI("## unprovision SpecialMode ");
    return true;
}

void SpecialModeApplication::doAfterProvision() {
    if(provisionSpecialMode() == true){
        setProvisioningFlag(true);
        initializeSpecialModeProcess();
    }else{
        LOGE("%s : false", __func__);
    }
}

void SpecialModeApplication::doAfterUnprovision() {
    if(unprovisionSpecialMode() == true){
//        mp_SpecialModeProcess->handleEvent(E_UNPROVISIONING);
        releaseSpecialModeProcess();
        setProvisioningFlag(false);
    }else{
        LOGE("%s : false", __func__);
    }
}

uint32_t SpecialModeApplication::getSpecialModeType() {
    return m_SpecialModeType;
}

void SpecialModeApplication::setSpecialModeType(uint32_t specialModeType) {
    m_SpecialModeType = specialModeType;
}

void SpecialModeApplication::setProvisioningFlag(bool flag) {
    m_ProvisioningFlag = flag;
}

void SpecialModeApplication::onPostReceived(const sp<Post>& post) {

    LOGI("SpecialModeApplication::onPostReceived : what[0x%x] arg1[%d] arg2[%d]", post->what, post->arg1, post->arg2);
    sp<Buffer> buf_tmp = new Buffer();
    sp<sl::Message> message = m_Handler->obtainMessage(post->what, post->arg1, post->arg2);
    message->sendToTarget();
}

void SpecialModeApplication::doSpecialModeHandler(uint32_t what, const sp<sl::Message>& message) {
    LOGV("#########doSpecialModeHandler : what[%d],msgwhat[%d],msgarg1[%d]", what, message->what, message->arg1);
    mp_SpecialModeProcess->doSpecialModeHandler(what, message);
}

void SpecialModeApplication::initializeSpecialModeProcess(){
    LOGI("## initializeSpceialModeProcess() start");
     if(m_SpecialModeType == E_IN_CONTROL_LIGHT){
         mp_SpecialModeProcess = (SpecialModeBaseProcess*) new InControlLightProcess();
     }
     else{
         mp_SpecialModeProcess = (SpecialModeBaseProcess*) new DemoModeProcess();
     }
     mp_SpecialModeProcess->initialize(m_ServicesMgr, this, m_Handler);
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
