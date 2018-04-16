#define LOG_TAG "SpecialModeApplication"
#include <sys/stat.h>
#include "SpecialModeBaseProcess.h"
#include "SpecialModeHandler.h"

SpecialModeBaseProcess::SpecialModeBaseProcess() : m_Handler(NULL),mp_Application(NULL),mp_DemoModeTimer(NULL) {
}

SpecialModeBaseProcess::~SpecialModeBaseProcess(){
    if(mp_DemoModeTimer != NULL) {
        delete mp_DemoModeTimer;
    }
}

void SpecialModeBaseProcess::initialize(sp<SpecialModeServicesManager> servicesMgr,ISpecialModeApplication* pApp){
    LOGV("## SpecialModeBaseProcess::initialize()");
    m_ServicesMgr   = servicesMgr;
    mp_Application  = pApp;
    initializeProcess();
}

std::string SpecialModeBaseProcess::getPropertyWrap(const char* name){
    if(name != NULL) {
        return mp_Application->getPropertyWrap(name);
    }
    return NULL;
}

void SpecialModeBaseProcess::setPropertyChar(const char* name, const char* value) {
    if(name != NULL) {
        mp_Application->setPropertyChar(name, value);
    }
}

void SpecialModeBaseProcess::setPropertyInt(const char* name, const int32_t i_value) {
    if(name != NULL) {
        mp_Application->setPropertyInt(name, i_value);
    }
}
