#ifndef _EV_VIF_HANDLER_H_
#define _EV_VIF_HANDLER_H_

#include "EvtVifManager.h"
#include "EvCanFrame.h"
class EvVifHandler : public Handler
{
    public:
        EvVifHandler(sp<SLLooper>& looper, EvtVifManager &evVifMgr)
            :Handler(looper), mEvVifMgr(evVifMgr) {}
        virtual ~EvVifHandler() {}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EvtVifManager &mEvVifMgr;
        int32_t getEchoCanId(uint32_t sig_Id);
        void sendCanEchoData(uint32_t sig_Id);
        int32_t sendEchoFrameData(void* obj);
        void send_Echo_GWM_PMZ_AD_Hybrid_Frame(EvCanFrame* tFrame);
        void send_Echo_GWM_PMZ_R_Hybrid_Frame(EvCanFrame* tFrame);
        void send_Echo_GWM_PMZ_S_Hybrid_Frame(EvCanFrame* tFrame);
        void send_Echo_GWM_PMZ_T_Hybrid_Frame(EvCanFrame* tFrame);
        void send_Echo_GWM_PMZ_U_Hybrid_Frame(EvCanFrame* tFrame);

};

enum EV_VIF_ENUM {
    CAN_TX = 1,
    CAN_ECHO,
    CAN_FRAME_ECHO
};
#endif // _EV_VIF_HANDLER_H_
