#ifndef _EV_POWERMODE_RECEIVER_H_
#define _EV_POWERMODE_RECEIVER_H_

#include "EvApp.h"

class PowerModeReceiver : public BnPowerModeReceiver
{
    public:
        PowerModeReceiver(EvApp& app):mApp(app) {}
        ~PowerModeReceiver() {}

        virtual void onPowerModeChanged(int8_t newMode)
        {
            mApp.onPowerModeChanged(newMode);
        }

    private:
        EvApp& mApp;
};

#endif // _EV_POWERMODE_RECEIVER_H_