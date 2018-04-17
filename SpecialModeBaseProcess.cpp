#define LOG_TAG "SpecialModeApplication"
#include <sys/stat.h>
#include "SpecialModeBaseProcess.h"
#include "SpecialModeHandler.h"

SpecialModeBaseProcess::SpecialModeBaseProcess() : m_Handler(NULL),mp_Application(NULL),mp_DemoModeTimer(NULL) {
}

SpecialModeBaseProcess::~SpecialModeBaseProcess(){
    delete mp_DemoModeTimer;
}

void SpecialModeBaseProcess::initialize(sp<SpecialModeServicesManager> servicesMgr,ISpecialModeApplication* pApp){
    m_ServicesMgr   = servicesMgr;
    mp_Application  = pApp;
    initializeProcess();
}

std::string SpecialModeBaseProcess::getPropertyWrap(const char* name){
    if(name != NULL) {
#ifndef G_TEST
        return mp_Application->getPropertyWrap(name);
#endif
        return "1";
    }
    return "-1";
}

void SpecialModeBaseProcess::setPropertyChar(const char* name, const char* value) {
    LOGV("SetpropertyChar: name[%s], value[%s]", name, value);
    if(name != NULL) {
#ifndef G_TEST
        mp_Application->setPropertyChar(name, value);
#endif
    }
}

void SpecialModeBaseProcess::setPropertyInt(const char* name, const int32_t i_value) {
    LOGV("SetPropertyInt: name[%s], value[%d]",name, i_value);
    if(name != NULL) {
#ifndef G_TEST
        mp_Application->setPropertyInt(name, i_value);
#endif
    }
}
