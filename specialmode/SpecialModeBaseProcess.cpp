/**
* \file    SpecialModeBaseProcess.cpp
* \brief     Declaration of SpecialModeBaseProcess
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

#include <sys/stat.h>
#include "SpecialModeBaseProcess.h"
#include "SpecialModeHandler.h"

#undef LOG_TAG
#define LOG_TAG "SpecialModeBaseProccess"

#define MAX_WAIT_COUNT  10

/**
 * construct
 */
SpecialModeBaseProcess::SpecialModeBaseProcess()
    : m_Handler(NULL),m_ServicesMgr(NULL),mp_Application(NULL),mp_DemoModeTimer(NULL){
    LOGV("## SpecialModeBaseProcess constructor called.");
}

SpecialModeBaseProcess::~SpecialModeBaseProcess(){
    if(mp_DemoModeTimer != NULL) {
        delete mp_DemoModeTimer;
    }
    LOGV("## ~SpceialModeBaseProcess() destructor called.");
}

void SpecialModeBaseProcess::initialize(sp<SpecialModeServicesManager> servicesMgr, ISpecialModeAppInterface* pApp, sp<Handler> handler){
    LOGV("## SpecialModeBaseProcess::initialize() called.");
    m_ServicesMgr   = servicesMgr;
    mp_Application  = pApp;
    m_Handler       = handler;

    m_TsimLock      = new TelephonyManager::TsimLock();
    if(m_psimLock == NULL) {
        m_psimLock = new TelephonyManager::PsimLock(TelephonyManager::REMOTE_SIM_PRIORITY_HIGH);
    }
    initializeProcess();
}


bool SpecialModeBaseProcess::acquireTSimLock(){
    bool res = false;
    res = m_TsimLock->acquire();

    LOGD("###acquireTSimLock() return value:%d", res);
    return res;
}

bool SpecialModeBaseProcess::releaseTSimLock(){
    bool res = false;

    res = m_TsimLock->release();

    LOGD("###releaseTSimLock() return value:%d", res);
    return res;
}

void SpecialModeBaseProcess::activateTSIM(){
    int32_t i = 0;
    if(TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM){
        m_request_acquire_tSim = true;
        TelephonyManager::setActiveSim(TelephonyManager::REMOTE_SIM_TSIM
                                , TelephonyManager::REMOTE_SIM_PRIORITY_HIGH
                                , TelephonyManager::REMOTE_SIM_CAUSE_ECALL);

        do {
            ::usleep(100*1000);
            i++;
        }while(TelephonyManager::getActiveSim() == TelephonyManager::REMOTE_SIM_PSIM
                && i <= MAX_WAIT_COUNT);
    }
    else{
        acquireTSimLock();
    }
    LOGV("activateTSIM() wait count:%d", i);
}

bool SpecialModeBaseProcess::acquirePSimLock() {
    bool res = false;
    res = m_psimLock->acquire();

    LOGD("%s: acquirePSimLock() return value:%d", __func__, res);
    return res;
}

bool SpecialModeBaseProcess::releasePSimLock() {
    bool res = false;
    res = m_psimLock->release();

    LOGD("%s: releasePSimLock() return value:%d", __func__, res);
    return res;
}

char* SpecialModeBaseProcess::getPropertyWrap(const char* name)
{
    if(name != NULL) {
        LOGD("%s:  name[%s]", __func__, name);
        return mp_Application->getPropertyWrap(name);
    }
    return NULL;
}

void SpecialModeBaseProcess::setPropertyChar(const char* name, const char* value)
{
    if(name != NULL) {
        LOGD("%s:  name[%s], value[%s]", __func__, name, value);
        mp_Application->setPropertyChar(name, value);
    }
}

void SpecialModeBaseProcess::setPropertyInt(const char* name, const int32_t i_value)
{
    if(name != NULL) {
        LOGD("%s:  name[%s], i_value[%d]", __func__, name, i_value);
        mp_Application->setPropertyInt(name, i_value);
    }
}
