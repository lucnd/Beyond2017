#ifndef _EV_LONG_HORIZON_HANDLER_H_
#define _EV_LONG_HORIZON_HANDLER_H_

#include "EHApp.h"

class EHAppHandler : public Handler
{
    public:
        EHAppHandler(sp<SLLooper>& looper, EHApp& app)
            :Handler(looper), mEHApp(app) {}
        virtual ~EHAppHandler() {}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EHApp& mEHApp;

        void decodeIMCpacket(const Buffer* buffer);
        void sendEhCan(const Buffer* buffer);
        void decodeMF(const Buffer* buffer);
        void decodeSF(const Buffer* buffer);

        bool IsValidPacket(const Buffer* buffer);
};

#define RECV_EH_PACKET 0
#define SEND_EH_CAN_TX 1

#endif // _EV_LONG_HORIZON_HANDLER_H_
