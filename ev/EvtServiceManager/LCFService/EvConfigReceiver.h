#ifndef _EV_CONFIG_RECEIVER_H_
#define _EV_CONFIG_RECEIVER_H_

#include "EvApp.h"

class ConfigurationManagerReceiver : public BnConfigurationManagerReceiver
{
    public:
        ConfigurationManagerReceiver(EvApp& app):mApp(app) {}
        virtual ~ConfigurationManagerReceiver() {}

        virtual void onConfigDataChanged(sp<Buffer>& name)
        {
            mApp.onNotifyConfigDataChanged(name);
        }
    private:
        EvApp& mApp;
};

#endif // _EV_CONFIG_RECEIVER_H_