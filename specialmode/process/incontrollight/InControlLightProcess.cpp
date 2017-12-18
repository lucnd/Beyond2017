/**
* \file    InControlLightProcess.cpp
* \brief     Declaration of InControlLightProcess
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

#include "InControlLightProcess.h"


InControlLightProcess::InControlLightProcess(){
    LOGV("## InControlLightProcess create.");
}


InControlLightProcess::~InControlLightProcess(){
    LOGV("~InControlLightProcess()++");
}

void InControlLightProcess::do_SpecialModeHandler(int32_t what, const sp<sl::Message>& message){

}
void InControlLightProcess::handleTimerEvent(int timerId){

}



void InControlLightProcess::handleEvent(uint32_t ev){

}


void InControlLightProcess::initializeProcess(){
    LOGV("## InControlLightProcess::initializeProcess()");
}