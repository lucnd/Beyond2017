/**
* \file    SpecialModeHandler.cpp
* \brief     Declaration of SpecialModeHandler
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

#include "SpecialModeHandler.h"

SpecialModeHandler::~SpecialModeHandler(){
}

void SpecialModeHandler::handleMessage(const sp<sl::Message>& message){
    const int32_t what = message->what;
    //LOGI("SpecialModeHandler:: handleMessage what:%d msgarg1:%d",what,message->arg1);
    mr_Application.do_SpecialModeHandler(what, message);
}