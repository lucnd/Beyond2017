#ifndef _EV_NGTP_RECEIVER_H_
#define _EV_NGTP_RECEIVER_H_

#include "EvApp.h"

class NgtpReceiver : public BnNGTPReceiver
{
    public:
        NgtpReceiver(EvApp& app):mApp(app) { targetModuleID = mApp.getAppID(); }
        ~NgtpReceiver() {}

        virtual void onReceive(InternalNGTPData *data, int datalength)
        {
            mApp.onNotifyFromNgtpMgr(data, datalength);
        }

    private:
        EvApp& mApp;
};

#endif // _EV_NGTP_RECEIVER_H_