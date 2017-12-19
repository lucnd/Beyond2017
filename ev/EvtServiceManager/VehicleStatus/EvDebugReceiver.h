#ifndef _EV_DEBUG_RECEIVER_H_
#define _EV_DEBUG_RECEIVER_H_

#include "EvApp.h"

class DebugReceiver : public BnMgrReceiver
{
    public:
        DebugReceiver(EvApp& app):mApp(app) { targetModuleID = mApp.getAppID(); }
        ~DebugReceiver() {}

        virtual void onReceive(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
        {
            mApp.onNotifyFromDebugMgr(id, cmd, cmd2, buf);
        }

    private:
        EvApp& mApp;
};

#endif // _EV_DEBUG_RECEIVER_H_