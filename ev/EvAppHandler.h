#ifndef _EV_APP_HANDLER_H_
#define _EV_APP_HANDLER_H_

#include "EvApp.h"

class EvAppHandler : public Handler
{
    public:
        EvAppHandler(sp<SLLooper>& looper, EvApp& app)
            :Handler(looper), mApplication(app) {}
        virtual ~EvAppHandler() {}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EvApp& mApplication;

         //Custom Code.
        void check_ProvisioningState(std::string lookup_data);
        void decode_CAN_Signal(uint16_t sigID, Buffer& buf);
        void decode_CAN_Frame(void* obj);
        bool isVehicleNormalMode(int powerMode);
        void receive_PM(uint16_t pm);
        void regSVTmonitor();
        void receivedNGTP(Buffer& buf);
};

#endif // _EV_APP_HANDLER_H_