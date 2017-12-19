#ifndef _EV_CHARGE_APP_H_
#define _EV_CHARGE_APP_H_

// standard lib
#include <array>
#include <list>
#include <map>
#include "Handler.h"
#include "Message.h"
#include "SLLooper.h"
#include "Timer.h"

// Data class
#include "EvtSingleton.h"
#include "EvtChargeSettings.h"
#include "EvCanFrame.h"

//Utility
#include "EvtUtil/EvState.h"
#define PCR041ENABLE 1

using namespace sl;

//EVC Feature Static Class.
class EvChargeApp : public EvtSingleton<EvChargeApp>, EvState
{
    public:
        EvChargeApp();
        ~EvChargeApp(){};

        void onStart();
        bool check_ChargDataWakeUpAllow();

        bool send_NMMessageTCU();
        bool send_ChargeNowReqExtUpdte();

        void rcv_ChargingStatusDisp();
        void rcv_WlChargingStatusDisp();
        void rcv_HVBatteryUsableSOCDisp(Buffer& buf);
        void rcv_ChargePwrInterruptDisp(Buffer& buf);
        void rcv_CAN_Frame(EvCanFrame *pEvCanFrame);
        bool triggerUpdateAlertCondition(int32_t sigId);

        bool DBG_ChargeNowSetting(uint8_t value);
        bool DBG_ChargeModeChoice(uint8_t value);
        bool DBG_MaxStateOfCharge(uint32_t permValue, uint32_t oneOffValue);
        void send_ChargingStatusUpdate();
        void send_ChargingSettingsUpdate();

        void rcv_NgtpMessage(svc::ChargeSettings* msg, uint32_t _msgCreationTime);
        void DBG_NgtpMessage(svc::ChargeSettings* msg);

        void timeout_EVC_GWM_BUSOPEN_TIMER();
        void timeout_EVC_IMC_IGNR_TIMER();
        void timeout_EVC_IMC_WKUP_TIMER();
        void timeout_EVC_EVI_PUSH_TIMER();

        void initialSetupStart(); // for FIRST Time Setting. SRD_ReqEVC0100_v1
        void initialSetupEnd();

        // void pushSingleData(sp<vifCANContainer>& sigdata, int sigID, uint8_t data);
        error_t sendCanSignal(int sigID, const uint8_t data);
        error_t sendCanArraySignal(int sigID, const uint8_t data[]);
        int32_t getCfgData(const char* name);
        int32_t getDefaultCfgValue(const char* name);

        uint32_t getValueWithLimit(uint32_t value, uint32_t minLimit, uint32_t maxLimit, uint32_t defaultValue);
        std::array<int32_t, 5> arrayNgtpResult;

        enum{
            EVC_GWM_BUSOPEN_TIMER = 0,
            EVC_IMC_IGNR_TIMER,
            EVC_IMC_WKUP_TIMER,
            EVC_EVI_PUSH_TIMER,
            EVC_END_TIMER
        };

        enum{
            COMPARE_TO_IMC = 0,
            COMPARE_TO_TSP
        };

        void resetTimer(uint8_t timerName);
        bool isTimeToIgnoreChange();

        void triggerECWstate();
        void setEVCState(unsigned char newState);
        void initializeCompFlags();
        void restartTimer(uint8_t timerName, uint16_t expireTime, bool isMsTime);

    private:
        bool bEVC_EVI_TIMER_ALIVE;
        uint32_t latestErrorCode;

        sp<SLLooper>    m_Looper;
        sp<Handler>     mHandler;

        void setTimer(uint8_t timerName, uint16_t expireTime, bool isMsTime = false);
        uint32_t getRemainTimerTime(uint8_t timerName);
        void cancelTimer(uint8_t timerName);
        void cancelAllTimer();

        virtual void toStringState(unsigned char state, unsigned char newState);

        int32_t check_ChargeSettingDuplication(svc::ChargeSettings* ngtp_ChargeSettingMsg);
        int32_t check_ChargeSettingRequest();
        error_t check_ChargeNowSetting_Service(svc::ChargeSettings* ngtp_ChargeSettingMsg);
        error_t check_MaxStateOfCharge_Service(uint8_t compareMode, svc::ChargeSettings* ngtp_ChargeSettingMsg);
        error_t check_ChargingModeChoice_Service(svc::ChargeSettings* ngtp_ChargeSettingMsg);
        error_t check_DepartureTimers_Service(uint8_t compareMode, svc::ChargeSettings* ngtp_ChargeSettingMsg);
        error_t check_Tariffs_Service(svc::ChargeSettings* ngtp_ChargeSettingMsg);
        bool compareDepartureTimer(uint8_t compareMode, eDepartureTimer* ngtpDepartureTimer);
        bool compareTimerType(eDepartureTimerDefinition* ngtpDeptTimerDef, _DepartureTimerDefinition& canDeptTimerDef);
        bool compareDepartureTime(eDepartureTimerDefinition* ngtpDeptTimerDef, _DepartureTimerDefinition& canDeptTimerDef);
        bool compareTimerTarget(eDepartureTimerDefinition* ngtpDeptTimerDef, _DepartureTimerDefinition& canDeptTimerDef);
        bool compareDayOfWeek(char& ngtpDay, bool canBool);

        error_t check_ChargeNowSetting_Dup(svc::ChargeSettings* ngtp_ChargeSettingMsg);
        error_t check_MaxStateOfCharge_Dup(svc::ChargeSettings* ngtp_ChargeSettingMsg);
        error_t check_ChargingModeChoice_Dup(svc::ChargeSettings* ngtp_ChargeSettingMsg);

        void start_EVC_EVI_PUSH_TIMER();
        void sendChargingStatusByCondition();

        void send_ChargeSettingSResponse(int32_t _result);
        bool sendDbgMaxSOC(uint8_t mode, uint32_t value);
        bool isConflictWithOnBoardChange();
        void trigger_setEcwState(uint8_t cmd);

        Timer* EVC_Timers[EVC_END_TIMER];

        svc::ChargeSettings current_ChargeSettings;
        std::list<svc::ChargeSettings*> list_Ngtp_ChargeSettings;

        //MAP <-> Function pointer
        typedef void (*FrameFunc)(EvCanFrame *pEvCanFrame);
        typedef std::map<uint16_t, FrameFunc > MAP_FUNC_LIST;
        MAP_FUNC_LIST m_mapFrameFunc;

        bool bCompChargeNow;
        bool bCompChargingMode;
        bool bCompMaxSOC;
        bool bCompTariff;
        bool bCompDeptTimer;

    public:
        enum EVCStatus{
            EVC_OFF = 0,
            EVC_IDLE,
            EVC_WAIT_WAKEUP_STATE,
            EVC_WAIT_IMC_IGNR_STATE,
            EVC_WAIT_CAN_SIGNAL_STATE,
            EVC_FIRST_TIME_STATE,
            EVC_END_ENUM
        };

        enum ChrgNowState{
            STOP = 0,
            START,
            UNKNOWN
        };

        enum EVC_Hander_Identifier{
            REVC_END=0
        };
};

#endif // _EV_CHARGE_APP_H_
