/**
* \file    SpecialModeReceiverManager.h
* \brief     Declaration of SpecialModeReceiverManager
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

#if !defined(EA_16B00816_75DE_4cb6_8DD1_AC6A8295F479__INCLUDED_)
#define EA_16B00816_75DE_4cb6_8DD1_AC6A8295F479__INCLUDED_

#include "SpecialModeServicesManager.h"

class SpecialModeReceiverManager
{
    class SpecialModeDebugReceiver: public BnMgrReceiver
    {

    public:
        SpecialModeDebugReceiver(SpecialModeReceiverManager& rReceiverMgr): mr_ReceiverMgr(rReceiverMgr){};
        virtual ~SpecialModeDebugReceiver(){};
        virtual void onReceive(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);

    private:
        SpecialModeReceiverManager& mr_ReceiverMgr;

    };

    class SpecialModeDiagReceiver: public BnDiagManagerReceiver
    {

    public:
        SpecialModeDiagReceiver(SpecialModeReceiverManager& rReceiverMgr): mr_ReceiverMgr(rReceiverMgr){};
        virtual ~SpecialModeDiagReceiver(){};
        virtual void onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);
        virtual void onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);

    private:
        SpecialModeReceiverManager& mr_ReceiverMgr;

    };

    class TelephonyListener: public PhoneStateListener
    {
        public:
            TelephonyListener(SpecialModeReceiverManager& rReceiverMgr) : PhoneStateListener(false), mr_ReceiverMgr(rReceiverMgr) {};
        virtual ~TelephonyListener() {}
        virtual void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause)
        {
            mr_ReceiverMgr.onPSimStateChanged(simAct, event, slot, errorCause);
        }
        virtual void onActiveSimStateChanged(int32_t activeSim)
        {
            mr_ReceiverMgr.onActiveSimStateChanged(activeSim);
        }
        virtual void onPsimLockStateChanged(bool locked)
        {
            mr_ReceiverMgr.onPsimLockStateChanged(locked);
        }

        private:
            SpecialModeReceiverManager& mr_ReceiverMgr;
    };

    class SpecialModeWifiReceiver : public BnWifiManagerReceiver
    {
        public:
           SpecialModeWifiReceiver(SpecialModeReceiverManager& rReceiverMgr) : mr_ReceiverMgr(rReceiverMgr) { }
           virtual ~SpecialModeWifiReceiver(){}
           virtual error_t onWifiReceive(int32_t state, sp<Buffer>& payload);

        private:
            SpecialModeReceiverManager& mr_ReceiverMgr;
    };

    class SpecialModeConfigurationReceiver : public BnConfigurationManagerReceiver {
        public:
            SpecialModeConfigurationReceiver(SpecialModeReceiverManager& rReceiverMgr) : mr_ReceiverMgr(rReceiverMgr) { }
            virtual ~SpecialModeConfigurationReceiver() {}
            virtual void onConfigDataChanged(sp<Buffer>& buf);

        private:
            SpecialModeReceiverManager& mr_ReceiverMgr;
    };

public:
    SpecialModeReceiverManager(sp<SpecialModeServicesManager> servicesMgr, sp<Handler> handler);
    virtual ~SpecialModeReceiverManager();

    bool initializeReceiver();
    void releaseReceiver();
    void setHandler(sp<Handler> handler);

    void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause);
    void onActiveSimStateChanged(int32_t activeSim);
    void onPsimLockStateChanged(bool locked);

private:
    sp<IMgrReceiver> m_DebugReceiver;
    sp<Handler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    sp<IDiagManagerReceiver> m_SpecialModeDiagReceiver;
    sp<TelephonyListener> m_TelephonyListener;
    sp<IWifiManagerService> mWifiService;
    sp<IWifiManagerReceiver> mWifiReceiver;

     sp<IConfigurationManagerService> m_configManager;
    sp<IConfigurationManagerReceiver> m_configReceiver;
    bool m_IsInitialize;
    int32_t m_MdmVariant;
};
#endif // !defined(EA_16B00816_75DE_4cb6_8DD1_AC6A8295F479__INCLUDED_)
