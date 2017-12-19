#ifndef _EV_RANGE_HANDLER_H_
#define _EV_RANGE_HANDLER_H_

#include "EvRangeApp.h"

class EvRangeAppHandler : public Handler
{
    public:
        EvRangeAppHandler( sp<SLLooper>& looper, EvRangeApp& app)
            : Handler(looper), mEvRangeApp(app) {}
        virtual ~EvRangeAppHandler(){}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EvRangeApp& mEvRangeApp;
};

#endif // _EV_RANGE_HANDLER_H_
