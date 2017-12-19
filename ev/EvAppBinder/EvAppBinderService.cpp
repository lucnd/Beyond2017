/**
* \file    ISecurityManagerService.cpp
* \brief     Implementation of ISecurityManagerService class
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       paulwj.choi
* \date       2017.04.04
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#define LOG_TAG "EvAppBinderService"
#include "Log.h"

#include <binder/Parcel.h>
#include <binder/IServiceManager.h>
#include <utils/String16.h>

#include "EvAppBinderService.h"
#include "EvtNgtpMsgManager.h"

EvAppBinderService::EvAppBinderService()
{
    LOGV("EvAppBinderService()");
}

void EvAppBinderService::onCreate()
{
    LOGE("EvAppBinderService::onCreates");

    instantiate();
}

void EvAppBinderService::instantiate()
{
    LOGV("EvAppBinderService::instantiate()");
    defaultServiceManager()->addService(String16(EvAppBinderService::getStubName()), new EvBinderStub(*this));
}

// Service Stub Function
sp<Buffer> EvAppBinderService::EvBinderStub::getEviFromEvc()
{
    LOGV("EvBinderStub::getEviFromEvc");
    return mEvAppBinderService.getEviFromEvc();
}

// ServiceManager Funnction
sp<Buffer> EvAppBinderService::getEviFromEvc()
{
    LOGV("EvAppBinderService::getEviFromEvc");

    svc::ElectricVehicleInformation evInfo;
    int32_t eviSize;
    sp<Buffer> buf = new Buffer();

    EvtNgtpMsgManager::GetInstance()->setCurElectricVehicleInfo(evInfo);
    eviSize = (int32_t)sizeof(evInfo);
    buf->setTo((char*) &evInfo, eviSize);

    return buf;
}

// ECC
sp<Buffer> EvAppBinderService::EvBinderStub::getPreCondiStatusFromEcc()
{
    LOGV("EvBinderStub::getPreCondiStatusFromEcc");
    return mEvAppBinderService.getPreCondiStatusFromEcc();
}

// ECC
sp<Buffer> EvAppBinderService::getPreCondiStatusFromEcc()
{
    LOGV("EvAppBinderService::getPreCondiStatusFromEcc");

    svc::EVPreconditionStatus tPreconditionStatus;
    EvtNgtpMsgManager::GetInstance()->setCurEvPreconditionStatus(tPreconditionStatus);

    sp<Buffer> buf = new Buffer();
    buf->setTo((uint8_t*) &tPreconditionStatus, sizeof(tPreconditionStatus));
    return buf;
}
