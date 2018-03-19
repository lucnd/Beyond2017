#ifndef ISPECIALMODEAPPLICATION_H
#define ISPECIALMODEAPPLICATION_H

#include <utils/SLLooper.h>
#include <utils/Handler.h>
#include <binder/IServiceManager.h>

#include <corebase/application/Application.h>
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/ApplicationManagerService/ISystemPostReceiver.h>
#include <services/ApplicationManagerService/IApplicationManagerService.h>

class ISpecialModeApplication
{
public:
    ISpecialModeApplication() {}
    virtual ~ISpecialModeApplication() {}
    virtual char* getPropertyWrap(const char* name) = 0;

    virtual void setPropertyChar(const char* name, const char* value, bool sync_now = false) = 0;
    virtual void setPropertyInt(const char* name, const int32_t i_value, bool sync_now = true) = 0;
    virtual void doSpecialModeHandler(uint32_t what, const sp<sl::Message>& message) = 0;
};

#endif // ISPECIALMODEAPPLICATION_H
