/**
* \file    iSecurityManagerService.h
* \brief     Declaration of iSecurityManagerService
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author     paulwj.choi
* \date       2017.04.04
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/


#ifndef I_EV_APP_BINDER_SERVICE_H
#define I_EV_APP_BINDER_SERVICE_H

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/StrongPointer.h>

#include "Typedef.h"
#include "Buffer.h"

#include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"

using namespace android;

class IEvAppBinderService : public IInterface
{
public:
    DECLARE_META_INTERFACE(EvAppBinderService);
    /*
    static const android::String16 descriptor;
    static android::sp<IEvAppBinderService> asInterface(const android::sp<android::IBinder>& obj);
    virtual const android::String16& getInterfaceDescriptor() const;
    IEvAppBinderService();
    virtual ~IEvAppBinderService();
    */

    virtual sp<Buffer> getEviFromEvc() = 0;
    virtual sp<Buffer> getPreCondiStatusFromEcc() = 0;
    static const char* getServiceName()
    {
        return "application.EvAppBinderService";
    }
};

class BnEvAppBinderService : public BnInterface<IEvAppBinderService>
{

public:
    virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags = 0);
};
#endif /**I_EV_APP_BINDER_SERVICE_H*/
