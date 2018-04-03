#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeHandler.h"

SpecialModeHandler::~SpecialModeHandler(){
}

#ifndef G_TEST
void SpecialModeHandler::handleMessage(const sp<sl::Message>& message){
    mr_Application.doSpecialModeHandler(message);
}
#endif
