/**
* \file    SpecialModeServicesManager.h
* \brief     Declaration of SpecialModeServicesManager
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.13
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#if !defined(EA_03719987_E304_424c_84ED_6E688B6D0791__INCLUDED_)
#define EA_03719987_E304_424c_84ED_6E688B6D0791__INCLUDED_

#include <binder/BinderService.h>
#include "SpecialModeData_type.h"

class SpecialModeServicesManager : public RefBase
{

public:
    SpecialModeServicesManager();
    virtual ~SpecialModeServicesManager();

    sp<IDebugManagerService> getDebugManager();
    sp<IDiagManagerService> getDiagManager();
    sp<IPowerManagerService> getPowerManager();
    sp<IWifiManagerService> getWifiManager();
    sp<IApplicationManagerService> getApplicationManager();
    sp<IConfigurationManagerService> getConfigurationManager();

private:
    sp<IDebugManagerService> m_DebugMgr;
    sp<IDiagManagerService> m_DiagMgr;
    sp<IPowerManagerService> m_PowerMgr;
    sp<IWifiManagerService> m_WifiMgr;
    sp<IApplicationManagerService> m_AppMgr;
    sp<IConfigurationManagerService> m_ConfigurationMgr;

};
#endif // !defined(EA_03719987_E304_424c_84ED_6E688B6D0791__INCLUDED_)
