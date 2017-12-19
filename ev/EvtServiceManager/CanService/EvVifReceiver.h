#ifndef _EV_VIF_RECEIVER_H_
#define _EV_VIF_RECEIVER_H_

#include "EvApp.h"

class VifReceiver : public BnvifManagerReceiver
{
    public:
        VifReceiver(EvApp& app):mApp(app) { targetModuleID = mApp.getAppID(); }
        ~VifReceiver() {}

        virtual void onReceive(uint16_t Sigid, sp<Buffer>& buf)
        {
            mApp.onNotifyFromVifMgr(Sigid, buf);
        }

        virtual void onReceiveFrame(uint16_t FrameId, sp<vifCANFrameSigData>& FrameSigData)
        {
            mApp.onNotifyFrameSignalFromVifMgr(FrameId , FrameSigData);
        }

    private:
        EvApp& mApp;
};

#endif // _EV_VIF_RECEIVER_H_