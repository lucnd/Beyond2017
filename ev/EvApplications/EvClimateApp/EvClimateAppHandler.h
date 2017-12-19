#ifndef _EV_CLIMATE_CONTROL_HANDLER_H_
#define _EV_CLIMATE_CONTROL_HANDLER_H_

#include "EvClimateApp.h"
#define UNKNOWN -1

class EvClimateAppHandler : public Handler
{
    public:
        EvClimateAppHandler(sp<SLLooper>& looper, EvClimateApp& app)
            :Handler(looper), mEvClimateApp(app) {mAlertReason = UNKNOWN;}
        virtual ~EvClimateAppHandler() {}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EvClimateApp& mEvClimateApp;
        int16_t mAlertReason;

        void NGTP_PRECONDITON_UPDATE_ALERT(uint16_t alert_Reason);
        void delayed_NGTP_PRECONDITON_UPDATE_ALERT();
        void send_NGTP_PRECONDITON_UPDATE_ALERT(uint16_t alert_Reason);
        void trigger_delayed_NGTP_PRECONDITON_UPDATE_ALERT();
};

#endif // _EV_CLIMATE_CONTROL_HANDLER_H_