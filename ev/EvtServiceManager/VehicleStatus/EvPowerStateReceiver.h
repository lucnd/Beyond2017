#ifndef _EV_POWERSTATE_RECEIVER_H_
#define _EV_POWERSTATE_RECEIVER_H_

#include "EvApp.h"
class PowerStateReceiver : public BnPowerStateReceiver
{
    public:
        PowerStateReceiver(EvApp& app):mApp(app) {}
        ~PowerStateReceiver() {}

        virtual void onPowerStateChanged(int32_t newState, int32_t reason)
        {
            mApp.onPowerStateChanged(newState, reason);
        }
        virtual void onWakeup(int32_t currentState, int32_t reason)
        {
            mApp.onWakeup(currentState, reason);
        }

    private:
        EvApp& mApp;
};

#endif // _EV_POWERSTATE_RECEIVER_H_