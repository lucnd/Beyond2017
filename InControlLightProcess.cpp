#define LOG_TAG "SpecialModeApplication"
#include "InControlLightProcess.h"

InControlLightProcess::InControlLightProcess(){
    LOGV("## InControlLightProcess create");
}

InControlLightProcess::~InControlLightProcess(){
    LOGV("~InControlLightProcess()++");
}

void InControlLightProcess::handleEvent(uint32_t ev){

}

void InControlLightProcess::handleTimerEvent(int timerId){

}

void InControlLightProcess::doSpecialModeHandler(int32_t what, const sp<sl::Message> &message){

}

void InControlLightProcess::initializeProcess(){
    LOGV("## InControlLightProcess::initializeProcess()");
}
