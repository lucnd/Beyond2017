#ifndef _EV_HBD_HANDLER_H_
#define _EV_HBD_HANDLER_H_

#include "EvHBDApp.h"

class EvHBDAppHandler : public Handler
{
    public:
        EvHBDAppHandler( sp<SLLooper>& looper, EvHBDApp& app)
            : Handler(looper), mHBDApp(app) {}
        virtual ~EvHBDAppHandler(){}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EvHBDApp& mHBDApp;
};

#endif // _EV_HBD_HANDLER_H_
