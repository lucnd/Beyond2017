/**
* \file    SpecialModeApplication.cpp
* \brief     Declaration of SpecialModeApplication
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.13
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#define LOG_TAG "SpecialModeApplication"

#include <sys/stat.h>

#include "Log.h"

#include "SpecialModeApplication.h"
#include "SpecialModeEvent.h"
#include "SpecialModeType.h"
#include "process/demomode/DemoModeProcess.h"
#include "process/incontrollight/InControlLightProcess.h"


/*local version for SpecialMode*/
static const char* const LOCAL_VER = "20170426.01R";

android::sp<Application> gApp;

uint32_t SpecialModeApplication::m_SpecialModeType= 0;

SpecialModeApplication::~SpecialModeApplication(){

}

/**
 * Constructor
 */
SpecialModeApplication::SpecialModeApplication()
    : mp_SpecialModeProcess(NULL),mp_ReceiverMgr(NULL),m_AppAlive(false){
    LOGI("%s  JLR Create : LOCAL_VERSION [ %s ]", __func__, LOCAL_VER);
}

void SpecialModeApplication::onCreate(){
    LOGI("SpecialMode for JLR, and into in inActive State");
    m_Looper = SLLooper::myLooper();
    if(m_Handler == NULL){
        m_Handler = new SpecialModeHandler(m_Looper, *this);
    }

    if(m_ServicesMgr == NULL) {
        m_ServicesMgr   = new SpecialModeServicesManager();
    }

    if(mp_ReceiverMgr == NULL){
        mp_ReceiverMgr = new SpecialModeReceiverManager(m_ServicesMgr,m_Handler);
        mp_ReceiverMgr->initializeReceiver();
    }
    m_AppAlive  = true;
    handleEvent(SpecialMode_PROVISIONING_E);
}


void SpecialModeApplication::onDestroy(){
    LOGI("SpecialMode OnDestroy");

    handleEvent(SpecialMode_UNPROVISIONING_E);
    if(mp_SpecialModeProcess != NULL){
        delete mp_SpecialModeProcess;
        mp_SpecialModeProcess = NULL;
    }

    if(mp_ReceiverMgr != NULL){
        mp_ReceiverMgr->releaseReceiver();
        delete mp_ReceiverMgr;
        mp_ReceiverMgr = NULL;
    }

    printf("~SpecialModeApplication JLR");

    ReadyToDestroy();
}

void SpecialModeApplication::onPostReceived(const sp<Post>& post){
    LOGI("SpecialModeApplication::onPostReceived : what[0x%x] arg1[%d] arg2[%d]", post->what, post->arg1, post->arg2);
    sp<Buffer> buf_tmp = new Buffer();
    sp<sl::Message> message = m_Handler->obtainMessage(post->what, post->arg1, post->arg2);
    message->sendToTarget();
}

void SpecialModeApplication::Initialize_SpecialModeProcess(){
    LOGI("initialize_SpceialModeProcess() SpecialMode Type : %d", m_SpecialModeType);

    if(m_SpecialModeType == IN_CONTROL_LIGHT){
        mp_SpecialModeProcess = (SpecialModeBaseProcess*) new InControlLightProcess();                
    }
    else{
        mp_SpecialModeProcess = (SpecialModeBaseProcess*) new DemoModeProcess();
    }
    mp_SpecialModeProcess->initialize(m_ServicesMgr, this, m_Handler);
}

void SpecialModeApplication::release_SpecialModeProcess(){
    if(mp_SpecialModeProcess== NULL) {
        return;
    }
    LOGI("release_SpecialModeProcess() called.");

    delete mp_SpecialModeProcess;
    mp_SpecialModeProcess = NULL;
}

void SpecialModeApplication::do_SpecialModeHandler(uint32_t what, const sp<sl::Message>& message){
    //LOGV("#########do_SpecialModeHandler : what[%d],msgwhat[%d],msgarg1[%d]", what, message->what, message->arg1);
    if(m_ProvisioningFlag == true) {
        mp_SpecialModeProcess->do_SpecialModeHandler(what, message);
    }
}

void SpecialModeApplication::handleEvent(uint32_t ev) {
    //LOGV(" SpecialModeApplication::handleEvent(uint32_t ev)");
    switch(ev) {
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
    LOGV("SpecialModeApplication PROVISIONING");

    if(provision_SpecialMode() == true){
        setProvisioningFlag(true);
        Initialize_SpecialModeProcess();
    }
    else{
        cout << "SpecialMode_provisioning Failed" << endl;
    }
}


void SpecialModeApplication::handleEventUnprovisioning(){
    LOGV("SpecialModeApplication::handleEventUnprovisioning() ++");
    if(unprovision_SpecialMode() == true){
        mp_SpecialModeProcess->handleEvent(SpecialMode_UNPROVISIONING_E);
        release_SpecialModeProcess();
        setProvisioningFlag(false);
    }
    else{
        cout << "SpecialMode_unprovisioning Failed" << endl;
    }
}


bool SpecialModeApplication::provision_SpecialMode(){
    /*TODO : read configuration  */
    //LOGI("provision_SpecialMode(): start");
    uint32_t configVal = 0;
    configVal = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, SPECIALMODE_INCONTROL);

    if((configVal >> INCONTROL_TYPE_WEIGHT) & 0x01){
        set_SpecialModeType(IN_CONTROL_LIGHT);
    }else{
        set_SpecialModeType(DEMOMODE);
    }
    LOGI("Get SpecialMode_type : %d ", m_SpecialModeType);
    return true;
}


bool SpecialModeApplication::unprovision_SpecialMode(){
    cout << "SpecialMode_unprovisioning" << endl;
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

extern "C" class Application* createApplication() {
    printf("create SpecialModeApplication");

    //android::sp<Application> app = new Hello;
    gApp = new SpecialModeApplication;
    return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
    delete (SpecialModeApplication*)application;
}

