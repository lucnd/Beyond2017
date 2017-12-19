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

#define LOG_TAG "IEvAppBinderService"
#include "Log.h"

#include <binder/Parcel.h>
#include <binder/IServiceManager.h>
#include "IEvAppBinderService.h"

#define LOG_TAG "IEvAppBinderService"

enum {
    REQUEST_TEST_CMD = IBinder::FIRST_CALL_TRANSACTION,
    REQUEST_PRECONDISTATUS
};


class BpEvAppBinderService: public BpInterface<IEvAppBinderService>
{
public:
    BpEvAppBinderService(const sp<IBinder>& impl)
        : BpInterface<IEvAppBinderService>(impl)
    {
        LOGV("BpEvAppBinderService()");
    }

    // Test Function for SLDD Test
    virtual sp<Buffer> getEviFromEvc()
    {
        sp<Buffer> buf = new Buffer();
        LOGV("[BpEvAppBinderService] start");
        Parcel data;
        Parcel reply;
        svc::ElectricVehicleInformation evInfo;
        int32_t size = (int32_t)sizeof(evInfo);

        buf->setSize(size);
        data.writeInterfaceToken(IEvAppBinderService::getInterfaceDescriptor());

        remote()->transact(REQUEST_TEST_CMD, data, &reply);
        reply.read(buf->data(), buf->size());
        LOGV("[BpEvAppBinderService]bufsize[%d]", buf->size());
        return buf;
    }

    virtual sp<Buffer> getPreCondiStatusFromEcc()
    {
        sp<Buffer> buf = new Buffer();
        LOGV("[BpEvAppBinderService] start");
        Parcel data, reply;
        int32_t size = (int32_t)sizeof(svc::EVPreconditionStatus);

        buf->setSize(size);
        data.writeInterfaceToken(IEvAppBinderService::getInterfaceDescriptor());

        remote()->transact(REQUEST_PRECONDISTATUS, data, &reply);
        reply.read(buf->data(), buf->size());
        LOGV("[BpEvAppBinderService]bufsize[%d]", buf->size());
        return buf;
    }
};

IMPLEMENT_META_INTERFACE(EvAppBinderService, "application.IEvAppBinderService");
/**
## Implemented Code of Defined Macro
const android::String16 IEvAppBinderService::descriptor(application.IEvAppBinderService);

const android::String16& IEvAppBinderService::getInterfaceDescriptor() const
{
    return IEvAppBinderService::descriptor;
}

android::sp<IEvAppBinderService> IEvAppBinderService::asInterface(const android::sp<android::IBinder>& obj)
{
    android::sp<IEvAppBinderService> intr;
    if (obj != NULL) {
        intr = static_cast<IEvAppBinderService*>(
            obj->queryLocalInterface(IEvAppBinderService::descriptor).get());
        if (intr == NULL) {
            intr = new BpEvApp(obj);
        }
    }
    return intr;
}

IEvAppBinderService::IEvAppBinderService() { }
IEvAppBinderService::~IEvAppBinderService() { }
*/

// ----------------------------------------------------------------------

status_t BnEvAppBinderService::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    LOGV("onTransact()");

    switch (code) {
        case REQUEST_TEST_CMD:
        {
            LOGV("[BnEvAppBinderService]case REQUEST_TEST_CMD");
            CHECK_INTERFACE(IEvAppBinderService, data, reply);
            sp<Buffer> buf = getEviFromEvc();
            LOGV("[BnEvAppBinderService]set Buf ");
            reply->write(buf->data(), buf->size());

            LOGV("[BnEvAppBinderService]case REQUEST_TEST_CMD bufsize[%d]", buf->size());
            break;
        }

        case REQUEST_PRECONDISTATUS:
        {
            LOGV("[BnEvAppBinderService] REQUEST_PRECONDISTATUS");
            CHECK_INTERFACE(IEvAppBinderService, data, reply);
            sp<Buffer> buf = getPreCondiStatusFromEcc();
            reply->write(buf->data(), buf->size());
            LOGV("[BnEvAppBinderService]case REQUEST_PRECONDISTATUS bufsize[%d]", buf->size());
            break;
        }
        default:
        {
            LOGV("[BnEvAppBinderService]case default");
            return BBinder::onTransact(code, data, reply, flags);

        }
    }
    LOGV("[BnEvAppBinderService]End");
    return E_OK;
}
