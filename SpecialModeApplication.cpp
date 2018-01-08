#define LOG_TAG "SpecialModeApplication"


#include "SpecialModeApplication.h"
#include "SpecialModeEvent.h"
#include "DemoModeProcess.h"
#include "InControlLightProcess.h"
#include "SpecialModeType.h"
#include "HMIType.h"

#include <Log.h>
#include <stdio.h>

static android::sp<Application> gApp;
uint32_t SpecialModeApplication::m_SpecialModeType = 0;

/* local version of Special Mode*/
//static const char* const LOCAL_VER = "20170426.01R";


SpecialModeApplication::~SpecialModeApplication() {
}

SpecialModeApplication::SpecialModeApplication() : mp_SpecialModeProcess(NULL), mp_ReceiverMgr(NULL), m_AppAlive(false) {

}

void SpecialModeApplication::onCreate() {
    LOGV("SpecialModeApplication::onCreate()");

    m_Looper = sl::SLLooper::myLooper();

    if(m_Handler == NULL){
        m_Handler = new SpecialModeHandler(m_Looper, *this);
    }

    if(m_ServicesMgr == NULL) {
        m_ServicesMgr = new SpecialModeServicesManager();
    }
    if(mp_ReceiverMgr == NULL) {
        mp_ReceiverMgr = new SpecialModeReceiverManager(m_ServicesMgr, m_Handler);
    }

    m_AppAlive = true;
    handleEvent(SpecialMode_PROVISIONING_E);     // need more time
}

void SpecialModeApplication::onDestroy() {
    LOGV("SpecialModeApplication::onDestroy()");

    handleEvent(SpecialMode_UNPROVISIONING_E);

    if(mp_SpecialModeProcess != NULL) {
        delete mp_SpecialModeProcess;
        mp_SpecialModeProcess = NULL;
    }
    if(mp_ReceiverMgr != NULL) {
        delete mp_ReceiverMgr;
        mp_ReceiverMgr = NULL;
    }

    printf("~SpecialModeApplication JLR");

    ReadyToDestroy();
}

void SpecialModeApplication::initialize_SpecialModeProcess() {

    LOGI("initialize_SpceialModeProcess() SpecialMode Type : %d", m_SpecialModeType);

}

void SpecialModeApplication::release_SpecialModeProcess() {
    LOGI("release_SpecialModeProcess() called.");
}

void SpecialModeApplication::do_SpecialModeHandler(uint32_t what, const sp<sl::Message>& message) {
    LOGV("#########do_SpecialModeHandler : what[%d],msgwhat[%d],msgarg1[%d]", what, message->what, message->arg1);
}

void SpecialModeApplication::handleEvent(uint32_t ev) {
     //LOGV(" SpecialModeApplication::handleEvent(uint32_t ev)");
    switch (ev) {
    case SpecialMode_PROVISIONING_E:
        handleEventProvisioning();
        break;
    case SpecialMode_UNPROVISIONING_E:
        handleEventUnprovisioning();
        break;

    default:
        break;
    }
}

void SpecialModeApplication::handleEventProvisioning(){
    if(provision_SpecialMode() == true){
        setProvisioningFlag(true);
        initialize_SpecialModeProcess();
    }
    else{
        printf("SpecialMode_provisioning Failed");
    }
}

void SpecialModeApplication::handleEventUnprovisioning() {
    if(unprovision_SpecialMode() == true) {
        mp_SpecialModeProcess->handleEvent(SpecialMode_UNPROVISIONING_E);
        release_SpecialModeProcess();
        setProvisioningFlag(false);
    }
    else {
        printf("SpecialMode_unprovision Failed");
    }
}

bool SpecialModeApplication::provision_SpecialMode(){
    /*TODO : read configuration  */
    //LOGI("provision_SpecialMode(): start");
    uint32_t configVal = 0;
    configVal = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, SPECIALMODE_INCONTROL);

    if((configVal >> INCONTROL_TYPE_WEIGHT) & 0x01){
        set_SpecialModeType(IN_CONTROL_LIGHT);
    }
    else{
        set_SpecialModeType(DEMOMODE);
    }

    LOGI("Get SpecialMode_type : %d ", m_SpecialModeType);
    return true;
}

bool SpecialModeApplication::unprovision_SpecialMode(){
    printf("Unprovision SpecialMode");
    return true;
}

uint32_t SpecialModeApplication::get_SpecialModeType(){
    return m_SpecialModeType;
}

void SpecialModeApplication::set_SpecialModeType(uint32_t SpecialModeType){
    m_SpecialModeType = SpecialModeType;
}

void SpecialModeApplication::setProvisioningFlag(bool flag){
    m_ProvisioningFlag  = flag;
}

char* SpecialModeApplication::getPropertyWrap(const char* name)
{
    if(name == NULL) {
        LOGE("%s  name is NULL !", __func__);
        return NULL;
    }
    return getProperty(name);
}

void SpecialModeApplication::setPropertyChar(const char* name, const char* value, bool sync_now)
{
    if(name != NULL) {
        LOGV("Berfore: %s name[%s], value[%s]", __func__, name, value);
        setProperty(name, value, sync_now);
    }
    LOGV("After: %s", __func__);
}

void SpecialModeApplication::setPropertyInt(const char* name, const int32_t i_value, bool sync_now)
{
    if(name != NULL) {
        LOGV("Berfore: %s name[%s], i_value[%d]", __func__, name, i_value);
        setProperty(name, i_value, sync_now);
    }
    LOGV("After: %s", __func__);
}

void SpecialModeApplication::onPostReceived(const sp<Post>& post) {
    LOGV("onPostReceived(), what = %d, arg1 = %d, arg2 = %d",
        post->what, post->arg1, post->arg2);
}

void SpecialModeApplication::onHMIReceived(const uint32_t type, const uint32_t action) {
    LOGV("onHMIReceived(), type = %d, action = %d", type, action);
}

void SpecialModeApplication::test() {
	printf("test");
}

bool SpecialModeApplication::onSystemPostReceived(const sp<Post>& systemPost) {
    LOGV("onSystemPostReceived(), what = %d", systemPost->what);
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
