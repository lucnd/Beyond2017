#define LOG_TAG "SpecialModeApplication"
#include "SpecialModeHandler.h"

SpecialModeHandler::~SpecialModeHandler(){
}

void SpecialModeHandler::handleMessage(const sp<sl::Message>& message){
    const int32_t what = message->what;
    mr_Application.doSpecialModeHandler(what, message);

}
