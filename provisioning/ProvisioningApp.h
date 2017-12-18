/**
* \file    ProvisioningApp.h
* \brief     Declaration of ProvisioningApp
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       wonsik93.park
* \date    2015.09.09
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#ifndef _PROVISIONING_SERVICE_APPLICATION_H_
#define _PROVISIONING_SERVICE_APPLICATION_H_

#include <application/Application.h>

#include "Handler.h"
#include "SLLooper.h"
#include "Timer.h"
#include "Buffer.h"

#include "HMIType.h"

#include "include/Typedef.h"
#include "include/ApplicationTable.h"

#include "services/NGTPManagerService/NGTPManagerService.h"
#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"
#include "services/NGTPManagerService/InternalNGTPData.h"
#include "services/PowerManagerService/PowerManagerService.h"
#include "services/PowerManagerService/IPowerManagerService.h"
#include "services/PowerManagerService/IPowerModeReceiver.h"
#include "services/HMIManagerService/HMIManagerService.h"
#include "services/HMIManagerService/IHMIManagerService.h"
#include "services/DiagManagerService/DiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerReceiver.h"
#include "services/ConfigurationManagerService/ConfigurationManagerService.h"
#include "services/ConfigurationManagerService/IConfigurationManagerService.h"
#include "services/ApplicationManagerService/ApplicationManagerService.h"
#include "services/ApplicationManagerService/IApplicationManagerService.h"
#include "services/vifManagerService/vifManagerService.h"
#include "services/vifManagerService/IvifManagerService.h"
#include "services/vifManagerService/IvifManagerReceiver.h"
#include "services/TelephonyManagerService/base/TelephonyManager.h"
#include "services/OperationModeManagerService/IOperationModeManagerService.h"
#include "services/CommunicationManagerService/CommunicationManagerService.h"
#include "services/CommunicationManagerService/CommunicationData.h"
#include "services/CommunicationManagerService/ICommunicationManagerService.h"
#include "services/DebugManagerService/DebugManagerService.h"
#include "services/DebugManagerService/IDebugManagerService.h"
#include "services/UpdateManagerService/UpdateManagerService.h"
#include "services/UpdateManagerService/IUpdateManagerService.h"
#include "services/NLIManagerService/INLIManagerService.h"
#include "services/NetworkManagerService/INetworkManagerService.h"
#include "services/SystemManagerService/ISystemManagerService.h"
#include "Provisioning_Types.h"
#include "ConfigRequestMsg.h"
#include "ConfigDataAckMsg.h"
#include "StatusUpdateAckMsg.h"
#include "FTUpdateMsg.h"
#include "NGTPDecodeMsg.h"
#include "KeepPower.h"
#include "OperationalMode.h"
#include "services/DiagManagerService/DiagCommand.h"

enum ProvisioningState {
    STATE_INIT,
    STATE_SEND_CONFIGDATA,
    STATE_RECEIVE_CONFIGDATA,
    STATE_SEND_CONFIGDATA_ACK,
    STATE_RECEIVE_STATUS_UPDATE,
    STATE_SEND_FTUPDATE
};

class ProvisioningApp : public Application
{
public:
    ProvisioningApp();
    virtual ~ProvisioningApp();
    virtual void onCreate();
    virtual void onDestroy();

    virtual uint16_t getAppID() { return APP_ID_PROVISIONING; }
    virtual void onHMIReceived(const uint32_t type, const uint32_t action);
    virtual void onPostReceived(const sp<Post>& post);
    void sendConfigRequestMsg1ToTSP();
    void sendConfigDataAckMsg1ToTSP(svc::Acknowledge ack);
    void sendStatusUpdateAckMsgToTSP(svc::Acknowledge ack);
    void sendFTUpdateMsgToTSP();
    void onNotifyFromTSP(InternalNGTPData *data, uint32_t size);
    void onDiagReceived(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);
    void decodingNGTPMessage(uint8_t *buffer, uint32_t size, uint32_t seconds, uint32_t millis, int32_t dsptAckRequired);
    void decodingFactoryTestMessage(uint8_t *buffer, uint8_t serviceType, uint32_t size, uint32_t eventCreationTime);
    void setCurrentState(ProvisioningState state) { mCurrentState = state; };
    ProvisioningState getState() { return mCurrentState; };
    //void sendDSPTackMsgToTSP();
    void makeEventCreationTime();
    void setCreationTime(uint32_t sec, uint32_t milli);
    uint32_t getCreationTimeSecond();
    uint32_t getCreationTimeMilli();
    void setVersionParameters();
    void bCall_backlight_on();
    void bCall_active_light_solid();
    void bCall_active_light_off();
    void startTimer(uint8_t timer_id);
    void stopTimer(uint8_t timer_id);
    void processDiagCommand(uint8_t cmd1, uint8_t cmd2, uint32_t id, sp<Buffer>& buf);
    void TSPMsgHandler(InternalNGTPData *ngtp);
    void responseDiagCommand(uint32_t id, uint8_t ack);
    void setDiagResponse(uint8_t id, int8_t resp);
    int8_t getDiagResponse(uint8_t id) { return mDiagResponse.valueFor(id); };
    void setDefaultVIN();
    error_t saveGenParamToConfigMgr();
    void notifyProvisioningComplete();
    sp<IvifManagerService> connectToVifMgr();
    void onVifReceive(uint16_t Sigid, sp<Buffer>& buf);
    void VIFMsgHandler(uint8_t cmd, sp<Buffer> buf);
    uint8_t* getVersionInfoWithSpaces(const char *name);
    uint8_t* getTelephonyInfoWithSpaces(uint8_t index);
    int32_t getBCallButtonLedTimeout();
    void setTuStsParameters();
    void setUpTuStsData();
    void checkRetryTimeout();
    bool isDuplicateMsg(uint8_t *lastData, uint8_t *receivedData, uint32_t size);
    void deleteRetryMessage();
    void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause);
    void onActiveSimStateChanged(int32_t activeSim);
    void sendFTUpdateMsgByActiveSim();
    void processActiveSimChanged(int32_t activeSim);
    uint8_t getCurrentOperationalMode();
    bool getbCallTriggered();
    void setbCallTriggered(bool triggered);

    void sendDiagResponse();
    bool TSimLockAcquire();
    bool TSimLockRelease();
    void enableSpyMode(bool enable);
    void send_bCallButton_ack();
    bool isCallActive();
    void HMIMsgHandler(uint8_t arg1, uint8_t arg2);
    uint8_t* getMCUInfoWithSpaces();
    void processProvisioningMessage();
    void acquireKeepPower();
    void releaseKeepPower();
    uint32_t convert_Time2Ngtptime(uint32_t sec);
    void changeActSim_forConfig();
    void trigger_configUpdate();
    void sendTSIMlockStatus(bool lock);
    void trigger_configUpdate_byTSP();
private:
    class MyHandler : public Handler
    {
        public:
            static const uint32_t RECV_MSG_FROM_MCU = 0;
            static const uint32_t RECV_MSG_FROM_TSP = 1;
            static const uint32_t RECV_MSG_FROM_HMI = 2;
            static const uint32_t RECV_MSG_FROM_DIAG = 3;
            static const uint32_t RECV_MSG_FROM_VIF = 4;
            static const uint32_t RECV_MSG_PSIM_STATE_CHANGED = 5;
            static const uint32_t RECV_MSG_ACTIVESIM_STATE_CHANGED = 6;

        public:
            MyHandler(sp<SLLooper>& looper, ProvisioningApp& outer)
                :Handler(looper), mApplication(outer) {}
            virtual ~MyHandler() {}

            virtual void handleMessage(const sp<sl::Message>& msg);

        private:
            ProvisioningApp& mApplication;
    };
    class NgtpReceiver : public BnNGTPReceiver
    {
        public:
            NgtpReceiver(ProvisioningApp& app):mApplication(app) { targetModuleID = mApplication.getAppID(); }
            virtual ~NgtpReceiver() {}

            virtual void onReceive(InternalNGTPData *data, int datalength)
            {
                LOGV("onReceive called");
                mApplication.onNotifyFromTSP(data, datalength);
            }

        private:
            ProvisioningApp& mApplication;
    };
    class PowerModeReceiver : public BnPowerModeReceiver
    {
        public:
            PowerModeReceiver(ProvisioningApp& app):mApplication(app) {}
            virtual ~PowerModeReceiver() {}
        virtual void onPowerModeChanged(const int8_t newMode)
        {
            LOGV("PowerMode onReceive called");
        }

        private:
            ProvisioningApp& mApplication;
    };
    class MyTimer : public TimerTimeoutHandler
    {
        public:
            static const uint32_t TIMER_BCALLBUTTON = 1;
            static const uint32_t TIMER_FTUPDATE = 2;
            static const uint32_t TIMER_CONFIG = 3;
            static const uint32_t TIMER_TUSTS = 4;
            static const uint32_t TIMER_RETRY = 5;
            static const uint32_t TIMER_DUPLICATE = 6;

        public:
            MyTimer(ProvisioningApp& s):mApplication(s) {}
            virtual ~MyTimer() {}

            void handlerFunction( int );

        private:
            ProvisioningApp& mApplication;
    };
    class DiagManagerReceiver : public BnDiagManagerReceiver
    {
        public:
            DiagManagerReceiver(ProvisioningApp& app):mApplication(app) {}
            virtual ~DiagManagerReceiver() {}

            virtual void onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
            {
                LOGV("DiagManager onReceive called, cmd = 0x%x, cmd2 = 0x%x, ", cmd, cmd2);
                mApplication.onDiagReceived(id, type, cmd, cmd2, buf);
            }
            virtual void onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
            {

            }
        private:
            ProvisioningApp& mApplication;
    };
    class VifReceiver : public BnvifManagerReceiver
    {
    public:
        VifReceiver(ProvisioningApp& app)
            :mParent(app) {};
        virtual ~VifReceiver() {}

        virtual void onReceive(uint16_t Sigid, sp<Buffer>& buf)
        {
            //LOGV("onReceive Sigid:0x%02X", Sigid);
            mParent.onVifReceive(Sigid, buf);
        }

        virtual void onReceiveFrame(uint16_t FrameId, sp<vifCANFrameSigData>& FrameSigData) {}

    private:
        ProvisioningApp &mParent;

    };
    class TelephonyListener : public PhoneStateListener
    {
    public:
        TelephonyListener(ProvisioningApp& app) : PhoneStateListener(false), mApplication(app) {}
        virtual ~TelephonyListener() {}
        virtual void onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause)
        {
            mApplication.onPSimStateChanged(simAct, event, slot, errorCause);
        }
        virtual void onActiveSimStateChanged(int32_t activeSim)
        {
            mApplication.onActiveSimStateChanged(activeSim);
        }
    private:
        ProvisioningApp& mApplication;
    };
private:
    sp<SLLooper> mLooper;
    sp<MyHandler> mHandler;

    sp<INGTPManagerService> m_ngtpManager;
    sp<INGTPReceiver> m_ngtpReceiver;
    sp<IPowerManagerService> m_powerManager;
    sp<IPowerModeReceiver> m_powerModeReceiver;
    sp<IHMIManagerService> mHMIMgr;
    sp<IDiagManagerService> m_diagManager;
    sp<IDiagManagerReceiver> m_diagManagerReceiver;
    sp<IConfigurationManagerService> m_configManager;
    sp<IApplicationManagerService> m_AppMgr;
    sp<IOperationModeManagerService> mOperationalMode;
    ProvisioningState mCurrentState;
    uint32_t mCreationTime_seconds;
    uint32_t mCreationTime_millis;
    ConfigRequestMsg *mConfigRequest;
    ConfigDataAckMsg *mConfigDataAck;
    StatusUpdateAckMsg *mStatusUpdateAck;
    FTUpdateMsg *mFTUpdate;
    NGTPDecodeMsg *mNGTPDecode;
    tuSts_parameters mTuStsParameters;
    vin_parameters mVinParameters;
    MyTimer *m_myTimer;
    Timer *m_bCallButtonTimer;
    Timer *m_FTUpdateTimer;
    Timer *m_ConfigTimer;
    Timer *m_TuStsTimer;
    Timer *m_RetryTimer;
    Timer *m_DuplicateTimer;
    KeyedVector<uint8_t, int8_t> mDiagResponse;
    sp<IvifManagerService> mvifManager;
    sp<IvifManagerReceiver> mvifReceiver;
    uint8_t vin_data[18];
    uint8_t vin_flag[3];
    uint8_t mLastServiceData[1024];
    sp<TelephonyListener> mTelephonyListener;
    KeepPower mKeepPower;
    uint8_t mKeepPowerCnt;
    bool mbCallTrigger;
    uint16_t diagRoutineID;
    sp<TelephonyManager::TsimLock> mTsimLock;
    bool m_bCallActive;
};
#endif /**_PROVISIONING_SERVICE_APPLICATION_H_*/
