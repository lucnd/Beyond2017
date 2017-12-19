#ifndef _EV_DIAG_RECEIVER_H_
#define _EV_DIAG_RECEIVER_H_

#include "EvApp.h"

class DiagMgrReceiver : public BnDiagManagerReceiver
{
    public:
        DiagMgrReceiver(EvApp& app):mApp(app) {}
        ~DiagMgrReceiver() {}

        virtual void onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
        {
            //TODO something..
        }
        virtual void onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
        {
            //TODO someThing..
        }

    private:
        EvApp& mApp;
};

#endif // _EV_DIAG_RECEIVER_H_