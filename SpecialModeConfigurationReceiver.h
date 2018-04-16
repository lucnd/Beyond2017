#ifndef SPECIALMODECONFIGURATIONRECEIVER_H
#define SPECIALMODECONFIGURATIONRECEIVER_H

#include <services/ConfigurationManagerService/IConfigurationManagerReceiver.h>
#include <utils/Handler.h>

class SpecialModeConfigurationReceiver: public BnConfigurationManagerReceiver {
public:
    explicit SpecialModeConfigurationReceiver(sp<sl::Handler> handler) : m_Handler(handler) {}
    virtual ~SpecialModeConfigurationReceiver() {}
    virtual void onConfigDataChanged(sp<Buffer>&) override;

private:
    sp<sl::Handler> m_Handler;
};

#endif // SPECIALMODECONFIGURATIONRECEIVER_H
