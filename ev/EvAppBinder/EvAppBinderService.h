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

#ifndef EV_APP_BINDER_SERVICE_H
#define EV_APP_BINDER_SERVICE_H

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/StrongPointer.h>

#include "Typedef.h"
#include "Buffer.h"

#include "IEvAppBinderService.h"

using namespace android;

class EvAppBinderService
{
public:
    EvAppBinderService();
    ~EvAppBinderService();
    void onCreate();
    void instantiate();

    static const char* getStubName() { return "application.IEvAppBinderService";};

public:
    sp<Buffer> getEviFromEvc();
    sp<Buffer> getPreCondiStatusFromEcc();

    class EvBinderStub : public BnEvAppBinderService
    {
    public:
        EvBinderStub(EvAppBinderService& m) : mEvAppBinderService(m) {}
        ~EvBinderStub(){};

        virtual sp<Buffer> getEviFromEvc();
        virtual sp<Buffer> getPreCondiStatusFromEcc();

    private:
        EvAppBinderService& mEvAppBinderService;
    };

};
#endif /**EV_APP_BINDER_SERVICE_H*/
