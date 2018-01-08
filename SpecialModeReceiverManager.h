#ifndef SPECIALMODERECEIVERMANAGER_H
#define SPECIALMODERECEIVERMANAGER_H

#include <utils/Handler.h>
#include <utils/Log.h>
#include <ServiceList.h>
#include <SpecialModeServicesManager.h>
#include <services/TelephonyManagerService/base/ITelephonyService.h>
#include <services/TelephonyManagerService/base/IPhoneStateListener.h>
#include <services/TelephonyManagerService/base/TelephonyManager.h>

class SpecialModeReceiverManager
{
public:
    class SpecialModeDebugReceiver : BnMgrReceiver
    {
    public:
        SpecialModeDebugReceiver(SpecialModeReceiverManager& rReceiverMgr): mr_ReceiverMgr(rReceiverMgr){}
        virtual ~SpecialModeDebugReceiver(){}
        virtual void onReceive(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);

    private:
        SpecialModeReceiverManager& mr_ReceiverMgr;

    };

    class SpecialModeDiagReceiver: public BnDiagManagerReceiver
    {

    public:
        SpecialModeDiagReceiver(SpecialModeReceiverManager& rReceiverMgr): mr_ReceiverMgr(rReceiverMgr){}
        virtual ~SpecialModeDiagReceiver(){}
        virtual void onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);
        virtual void onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);

    private:
        SpecialModeReceiverManager& mr_ReceiverMgr;


    };

//    class TelephonyListener: public PhoneStateListener
//    {
//        public:
//            TelephonyListener(SpecialModeReceiverManager& rReceiverMgr) : PhoneStateListener(false), mr_ReceiverMgr(rReceiverMgr) {}
//        virtual ~TelephonyListener() {}
//        virtual void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause)
//        {
//            mr_ReceiverMgr.onPSimStateChanged(simAct, event, slot, errorCause);
//        }
//        virtual void onPsimLockStateChanged(bool locked)
//        {
//            mr_ReceiverMgr.onPsimLockStateChanged(locked);
//        }
//        virtual void onActiveSimStateChanged(int32_t activeSim)
//        {
//            mr_ReceiverMgr.onActiveSimStateChanged(activeSim);
//        }


//        private:
//            SpecialModeReceiverManager& mr_ReceiverMgr;
//    };

    class SpecialModeConfigurationReceiver : public BnConfigurationManagerReceiver {

        public:
            SpecialModeConfigurationReceiver(SpecialModeReceiverManager& rReceiverMgr) : mr_ReceiverMgr(rReceiverMgr) {}
            virtual ~SpecialModeConfigurationReceiver() {}
            virtual void onConfigDataChanged(sp<Buffer>& buf);

        private:
            SpecialModeReceiverManager& mr_ReceiverMgr;
    };

//    class SpecialModeWifiReceiver : public BnWiFiManagerReceiver
//    {
//        public:
//           SpecialModeWifiReceiver(SpecialModeReceiverManager& rReceiverMgr) : mr_ReceiverMgr(rReceiverMgr) { }
//           virtual ~SpecialModeWifiReceiver(){}
//           virtual error_t onWifiReceive(int32_t state, sp<Buffer>& payload);

//        private:
//            SpecialModeReceiverManager& mr_ReceiverMgr;
//    };

public:
    SpecialModeReceiverManager(sp<SpecialModeServicesManager> servicesMgr, sp<sl::Handler> handler);
    virtual ~SpecialModeReceiverManager();
    bool initializeReceiver();
    void releaseReceiver();
    void setHandler(sp<sl::Handler> handler);
//    void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause);
//    void onPsimLockStateChanged(bool locked);
    void onActiveSimStateChanged(int32_t activeSim);


private:
    bool m_IsInitialize;
    int32_t m_MdmVariant;
    sp<IMgrReceiver> m_DebugReceiver;
    sp<sl::Handler> m_Handler;
    sp<SpecialModeServicesManager> m_ServicesMgr;
    sp<IDiagManagerReceiver> m_SpecialModeDiagReceiver;
//    sp<TelephonyListener> m_TelephonyListener;
//    sp<IWiFiManagerService> mWifiService;
//    sp<IWiFiManagerReceiver> mWifiReceiver;
    sp<IConfigurationManagerService> m_configManager;
    sp<IConfigurationManagerReceiver> m_configReceiver;
};

#endif // SPECIALMODERECEIVERMANAGER_H
