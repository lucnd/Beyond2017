#include "SpecialModeReceiverManager.h"
#include "SpecialModeHandler.h"

#include <utils/Log.h>
#include "Log.h"
#include <utils/Handler.h>


SpecialModeReceiverManager::SpecialModeReceiverManager ( sp<sl::Handler> handler) {
    LOGI("## SpecialModeReceiverManager created!! ");

    if(handler != NULL) {
        m_Handler   = handler;
    }

    m_IsInitialize   = false;
}

SpecialModeReceiverManager::~SpecialModeReceiverManager() {
    releaseReceiver();
}

bool SpecialModeReceiverManager::initializeReceiver() {
    LOGI("## initializeReceiver() called!!");

    if(m_Handler == NULL) {
        LOGE("## m_Handler == NULL");
         return false;
    }
    if(m_IsInitialize == true)
        return true;

    m_IsInitialize =true;
    return true;
}

void SpecialModeReceiverManager::releaseReceiver(){
    m_IsInitialize = false;
}

void SpecialModeReceiverManager::setHandler(sp<sl::Handler> handler) {
    if(handler == NULL) {
        return;
    }
    m_Handler = handler;
}

#undef LOG_TAG
#define LOG_TAG "SpecialModeReceiverManager"
