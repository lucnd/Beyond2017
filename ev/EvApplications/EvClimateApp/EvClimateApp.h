#ifndef _EV_CLIMATE_CONTROL_APP_H_
#define _EV_CLIMATE_CONTROL_APP_H_

#include <utils/RefBase.h>
#include <utils/Mutex.h>
#include <array>

#include "EvtEVPreconditionStatus.h"
#include "EvtEVPreconditionSettings.h"
#include "EvtElectricVehicleInformation.h"
#include "EvtUtil/EvState.h"
#include "EvServiceManager.h"

using android::Mutex;

//ECC Feature Static Class.
class EvClimateApp : public EvtSingleton<EvClimateApp>, EvState
{
    public:
        EvClimateApp();
        virtual ~EvClimateApp(){};
        friend class EvClimateAppHandler;

        // ECC Resource.
        void onStart();
        void initialSetup(); // for FIRST Time Setting.

        uint8_t check_ClimateControlCondition();

        bool rcv_PreconditionStartReq(uint32_t _targetTemperatureCelsiusX2);                // Check NGTP Rx
        bool rcv_PreconditionStopReq();                 // Check NGTP Rx
        bool rcv_PreconditionSettingReq(int8_t _fuelFiredHeaterSetting, int8_t _prioritySetting, uint32_t _msgCreationTime);              // Check NGTP Rx

        bool rcv_CAN_PreClimateStatus();
        bool rcv_CAN_HVBattHeatingStatus(Buffer& buf);
        bool rcv_CAN_HVBattCoolingStatusExt(Buffer& buf);
        void rcv_CAN_FFHOperatingStatus(Buffer& buf);
        bool rcv_CAN_PreClimateRequests(Buffer& buf);
        void rcv_CAN_FFHPreClimSetting(Buffer& buf);
        void rcv_CAN_PreClimRangeComfort(Buffer& buf);

        void DBG_Precoondition(uint8_t value);

        void timeout_ECC_CAN_STATUS_IGNORE_TIMER();
        void timeout_ECC_DIRECT_PRECONDITION_TIMER();
        void timeout_ECC_HVAC_NO_RESP_TIMEOUT_TIMER();
        void timeout_ECC_NM_TIMEOUT_TIMER();
        void timeout_ECC_CAN_SETTING_IGNORE_TIMER();
        void timeout_ECC_PRESET_NM_TIMEOUT_TIMER();
        void timeout_ECC_SETTING_TRIGGER_TIMER();

        uint32_t getRemainTimerTime(uint8_t timerName);
        bool resetTimer(uint8_t timerName);

        enum ECCStatus{
            ECC_OFF = 0,
            ECC_IDLE,
            ECC_WAIT_CAN_TIMER,     // For Waiting for BUS Wake up.
            ECC_LISTEN_CAN_SIG,     // For Listening that HVAC/VSC is agree with TCU3 request.
            ECC_DIRECT_PRECONDI,    // For Preconditioning is progressing.
            ECC_WAIT_PRE_STOP,       // For Preconditioning STOP
            ECC_PRESET_WAIT_CAN,    // For Preconditioning SETTING for NGTP. -- Wait CAN
            ECC_PRESET_LISTEN_CAN,   // For Preconditioning SETTING for NGTP. -- Listening CAN.
            _END_ENUM_
        };

        enum ECCTimers{
            ECC_CAN_STATUS_IGNORE_TIMER = 0,
            ECC_HVAC_NO_RESP_TIMEOUT_TIMER,
            ECC_DIRECT_PRECONDITION_TIMER,
            ECC_CAN_SETTING_IGNORE_TIMER,
            ECC_SETTING_TRIGGER_TIMER,
            ECC_NM_TIMEOUT_TIMER,
            ECC_PRESET_NM_TIMEOUT_TIMER,
            ECC_END_TIMER
        };

    private:
        void send_NGTP_PreconditionStartResp(int preClimateStatus);
        void send_NGTP_PreconditionUpdateAlert();

        bool IsPowerModeHigerThanFour();
        bool IsPreconditioningAlreayInProgress();
        bool IsRESActivated();
        uint16_t IsAvailableRES();

        void setTimer(uint8_t timerName, int32_t expireTime, bool isMsTime = false);
        void cancelTimer(uint8_t timerName);
        void cancelAllTimer();

        uint8_t getErrorResultFromPreClimateStatus(int32_t data);
        uint8_t getAlertResultFromPreClimateStatus(int32_t data);

        int8_t PM_Status;
        bool bIsSentStartResp;
        std::array<Timer*, ECC_END_TIMER> ECC_Timers;

        error_t sendCanSignal(int sigID, const uint8_t data);
        // error_t sendCanSignal(int sigID, uint8_t data[]);

        virtual void toStringState(unsigned char state, unsigned char newState);

        sp<SLLooper> m_Looper;
        sp<Handler> mHandler;
        Mutex mLock;
        EvtEVPreconditionSettings* mPreconditionSettings;
        EvtVifManager* mEvVif;
        EvConfigData* mEvConfig;
        EvtNgtpMsgManager* mEvNgtp;
};

const static std::string getEccTimerName[] = {
    "ECC_CAN_STATUS_IGNORE_TIMER",
    "ECC_HVAC_NO_RESP_TIMEOUT_TIMER",
    "ECC_DIRECT_PRECONDITION_TIMER",
    "ECC_CAN_SETTING_IGNORE_TIMER",
    "ECC_SETTING_TRIGGER_TIMER",
    "ECC_NM_TIMEOUT_TIMER",
    "ECC_PRESET_NM_TIMEOUT_TIMER",
    "ECC_END_TIMER"
};

const static std::string EccState[] = {
    "ECC_OFF",
    "ECC_IDLE",
    "ECC_WAIT_CAN_TIMER",
    "ECC_LISTEN_CAN_SIG",
    "ECC_DIRECT_PRECONDI",
    "ECC_WAIT_PRE_STOP",
    "ECC_PRESET_WAIT_CAN",
    "ECC_PRESET_LISTEN_CAN"
};

#endif //_EV_CLIMATE_CONTROL_APP_H_