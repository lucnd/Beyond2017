#ifndef SPECIALMODEHANDLER_H
#define SPECIALMODEHANDLER_H

#include "SpecialModeDataType.h"
#include "ISpecialModeApplication.h"

enum HandlerMessageID {
    E_FROM_DIAGNOSTICS           = 3,
    E_FROM_SLDD                  = 4,
    E_FROM_WIFI_ON               = 13,
    E_FROM_WIFI_OFF              = 14,
    E_FROM_CONFIG                = 15
};

class SpecialModeHandler : public sl::Handler
{
private:
    ISpecialModeApplication& mr_Application;

public:
    SpecialModeHandler(sp<sl::SLLooper>& looper, ISpecialModeApplication& service)
        : Handler(looper),mr_Application(service){}
    virtual void handleMessage(const sp<sl::Message>& message);
    virtual ~SpecialModeHandler();
};

#endif // SPECIALMODEHANDLER_H
