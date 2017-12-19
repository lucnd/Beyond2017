#ifndef _EV_I_RECEIVER_H_
#define _EV_I_RECEIVER_H_

#include "services/DebugManagerService/DebugManagerService.h"
#include "services/DebugManagerService/IDebugManagerService.h"
#include "services/DebugManagerService/IMgrReceiver.h"

#include "services/vifManagerService/IvifManagerService.h"
#include "services/vifManagerService/IvifManagerReceiver.h"

#include "services/NGTPManagerService/NGTPManagerService.h"
#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"
#include "services/NGTPManagerService/InternalNGTPData.h"

#include "PowerManagerService/PowerManagerService.h"
#include "services/PowerManagerService/IPowerStateReceiver.h"
#include "services/PowerManagerService/IPowerManagerService.h"

#include "services/ConfigurationManagerService/ConfigurationManagerService.h"
#include "services/ConfigurationManagerService/IConfigurationManagerService.h"

#include "services/DiagManagerService/IDiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerReceiver.h"

class EvIReceiver
{
protected:
    // Service <-> Receiver
    sp<IvifManagerService> m_vifMgrService;
    sp<IvifManagerReceiver> m_vifMgrReceiver;

    sp<INGTPManagerService> m_ngtpMgrService;
    sp<INGTPReceiver> m_ngtpMgrReceiver;

    sp<IPowerManagerService> m_powerMgrService;
    sp<IPowerModeReceiver> m_powerModeReceiver;
    sp<IPowerStateReceiver> m_powerStateReceiver;

    sp<IDebugManagerService> m_debugMgrService;
    sp<IMgrReceiver> m_DebugReceiver;

    sp<IConfigurationManagerService> m_configManagerService;
    sp<IConfigurationManagerReceiver> m_configManagerReceiver;
};

#endif // _EV_I_RECEIVER_H_