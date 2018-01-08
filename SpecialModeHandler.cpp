#include "SpecialModeHandler.h"

SpecialModeHandler::~SpecialModeHandler(){
}

void SpecialModeHandler::handleMessage(const sp<sl::Message>& message) {
    const int32_t what = message->what;
    mApplication.do_SpecialModeHandler(what, message);
}
