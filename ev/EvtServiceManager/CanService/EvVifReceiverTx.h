#ifndef _EV_VIF_RECEIVER_TX_H_
#define _EV_VIF_RECEIVER_TX_H_

#include "EvtVifManager.h"

class VifReceiverTx : public BnvifManagerReceiverTX
{
    public:
        VifReceiverTx(EvtVifManager& evtVifSvc):mEvtVifSvc(evtVifSvc) {}
        ~VifReceiverTx() {}

        virtual void onReceive(uint32_t CallerModuleID, uint8_t sendingresult, sp<vifCANContainer>& vifcancontainer)
        {
            //Do Nothing!
        }

    private:
        EvtVifManager& mEvtVifSvc;
};

#endif // _EV_VIF_RECEIVER_TX_H_