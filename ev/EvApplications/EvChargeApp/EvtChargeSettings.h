#ifndef _EV_CHARGE_SETTINGS_H_
#define _EV_CHARGE_SETTINGS_H_
#include <vector>
#include "EvtSingleton.h"
#include "EvtElectricVehicleInformation.h"
// #include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"

//Typedef of NGTP Classes
typedef svc::ChargeSettings eChargeSettings;
typedef svc::ChargeSettings::chargeNowSetting eChargeNowSetting;
typedef svc::ChargeSettings::maxStateOfCharge eMaxStateOfCharge;
typedef svc::ChargeSettings::maxStateOfCharge::permanentMaxSoc ePermanentMaxSoc;
typedef svc::ChargeSettings::maxStateOfCharge::oneOffMaxSoc eOneOffMaxSoc;
typedef svc::ChargeSettings::chargingModeChoice eChargingModeChoice;
typedef svc::ChargeSettings::departureTimers eDepartureTimers;
typedef svc::ChargeSettings::departureTimers::timers eDepartureTimerList;
typedef svc::ChargeSettings::departureTimers::timers::component eDepartureTimer;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition eDepartureTimerDefinition;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerType eDepartureTimerType;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::departureTime eDepartureTime;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerTarget eTimerTarget;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerTarget::singleDay eSingleDay;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerTarget::repeatSchedule eDepartureRepeatSchedule;
typedef svc::ChargeSettings::tariffs eTariffs;
typedef svc::ChargeSettings::tariffs::tariffs eTariffList;
typedef svc::ChargeSettings::tariffs::tariffs::component eTariff;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition eTariffDefinition;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::repeatSchedule eTariffRepeatSchedule;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneA eTariifZoneA;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneA::endTime eTariifEndTimeA;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneA::bandType eTariffBandTypeA;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneB eTariifZoneB;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneB::endTime eTariifEndTimeB;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneB::bandType eTariffBandTypeB;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneC eTariifZoneC;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneC::endTime eTariifEndTimeC;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneC::bandType eTariffBandTypeC;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneD eTariifZoneD;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneD::endTime eTariifEndTimeD;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneD::bandType eTariffBandTypeD;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneE eTariifZoneE;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneE::endTime eTariifEndTimeE;
typedef svc::ChargeSettings::tariffs::tariffs::component::tariffDefinition::tariffZoneE::bandType eTariffBandTypeE;

enum{
        ZoneA= 0,
        ZoneB,
        ZoneC,
        ZoneD,
        ZoneE
};


typedef struct
{
    bool state;
    // OSS_INT32 at svc::MaxStateOfCharge
    int32_t permanentMaxSoc_field;
    // 0- NoChange 1- Clear 2 - MaxSoc
    int8_t permanentMaxSoc_Status;

    // OSS_INT32 at svc::MaxStateOfCharge
    int32_t oneOffMaxSoc_field;
    // 0- NoChange 1- Clear 2 - MaxSoc
    int8_t oneOffMaxSoc_Status;
} _MaxStateOfCharge;

typedef struct
{
    uint32_t hour_field;
    uint32_t minute_field;
} _TimeHM;

typedef struct
{
    bool monday_field;
    bool tuesday_field;
    bool wednesday_field;
    bool thursday_field;
    bool friday_field;
    bool saturday_field;
    bool sunday_field;
} _DayOfWeekSchedule;

typedef struct
{
    uint32_t day;
    uint32_t month;
    uint32_t year;
    int32_t can_Day;
    int32_t can_Month;
    int32_t can_Year;
} _TimeDate;

typedef struct
{
    uint8_t timeType_field;
    _TimeDate singleDay_field;
    _DayOfWeekSchedule repeatSchedule_field;
} _TimerTarget;

typedef struct
{
    uint8_t timerType_field;
    _TimeHM departureTime_field;
    _TimerTarget timerTarget_field;
} _DepartureTimerDefinition;

typedef struct
{
    uint32_t timerIndex_field;
    _DepartureTimerDefinition departureTimerDefinition_field;
    bool _bDepartureTimerDefinition_is_present;
    bool _bDepartureTimer_is_updated;
} _DepartureTimer;

typedef struct
{
    _TimeHM endTime_field;
    uint8_t bandType_field;
} _TariffZone;

typedef struct
{
    bool isEnabled_field;
    _DayOfWeekSchedule repeatSchedule_field;
    _TariffZone tariffZoneA_field;
    _TariffZone tariffZoneB_field;
    _TariffZone tariffZoneC_field;
    _TariffZone tariffZoneD_field;
    _TariffZone tariffZoneE_field;
    bool _bTariffZoneA_is_present;
    bool _bTariffZoneB_is_present;
    bool _bTariffZoneC_is_present;
    bool _bTariffZoneD_is_present;
    bool _bTariffZoneE_is_present;
} _TariffDefinition;

typedef struct
{
    bool _bTariff_is_updated;
    uint32_t tariffIndex_field;
    _TariffDefinition tariffDefinition_field;
    bool _bTariffDefinition_is_present;
} _Tariff;

class EvtTariffEndTimes
{
    public:
        EvtTariffEndTimes();
        ~EvtTariffEndTimes(){};

        uint32_t getTariffEndTimeA() const;
        void    setTariffEndTimeA(uint32_t _tariffEndA);

        uint32_t getTariffEndTimeB() const;
        void    setTariffEndTimeB(uint32_t _tariffEndB);

        uint32_t getTariffEndTimeC() const;
        void    setTariffEndTimeC(uint32_t _tariffEndC);

        uint32_t getTariffEndTimeD() const;
        void    setTariffEndTimeD(uint32_t _tariffEndD);

    private:
        uint32_t tariffEndTimeA;
        uint32_t tariffEndTimeB;
        uint32_t tariffEndTimeC;
        uint32_t tariffEndTimeD;
};

class EvtChargeSettings : public EvtSingleton<EvtChargeSettings>
{
    public:
        EvtChargeSettings();
        ~EvtChargeSettings(){};
        void resetAll();
        void printData();

        uint32_t getCreationTimeStamp() const;
        void    setCreationTimeStamp(uint32_t _creationTimeStamp);

        uint32_t getLatestTimeStamp() const;
        void    setLatestTimeStamp(uint32_t _latestTimeStamp);

        uint32_t getDeadBandTimeStamp() const;
        void    setDeadBandTimeStamp(uint32_t _deadBandTimeStamp);

        eChargeNowSetting getChargeNowSettingField() const;
        void setChargeNowSettingField(eChargeNowSetting _chargeNowSettingField);

        void setMaxStateOfChargeField(int32_t _permanentMaxSoc, int32_t _oneOffMaxSoc);

        void setPermanentMaxSocField(int32_t _permanentMaxSoc);
        int32_t getPermanentMaxSocField() const;

        void setPermanentMaxSocStatus(int8_t _permanentMaxSocStat);
        int8_t getPermanentMaxSocStatus() const;

        void setOneOffMaxSocField(int32_t _oneOffMaxSoc);
        int32_t getOneOffMaxSocField() const;

        void setOneOffMaxSocStatus(int8_t _oneOffMaxSocStat);
        int8_t getOneOffMaxSocStatus() const;

        eChargingModeChoice getChargingModeChoiceField() const;
        void    setChargingModeChoiceField(eChargingModeChoice _chargingModeChoiceField);

        void setChargeSettingsFromNGTP(svc::ChargeSettings &ngtpCS) const;
        uint32_t getUpdatedDeptTimerCount();

        uint8_t getBandType(uint8_t _tariffIndex, uint8_t _tariffZone);

        bool isTariffZoneChanged(uint8_t _tariffIdx, uint8_t _canBandType, int32_t _canEndTime, uint8_t _tcuBandType, int32_t _tcuEndTime);
        bool isDayOfWeekChanged(uint8_t _idx, uint8_t _canMon, uint8_t _canTue, uint8_t _canWed, uint8_t _canThurs, uint8_t _canFri, uint8_t _canSat, uint8_t _canSun);
        bool isTariffActiveChanged(uint8_t _idx, int32_t _canActive);

        bool isDeptActiveChanged(uint32_t timerIdx, int32_t _canActive);
        bool isDeptTimeTypeChanged(uint32_t timerIdx, int32_t _canTimeType);
        bool isDeptTimeDateChanged(uint32_t timerIdx, int32_t _canDate, int32_t _canMnth, int32_t _canYear);
        bool isDeptDayOfWeekChanged(uint32_t timerIdx, uint8_t _canMon, uint8_t _canTue, uint8_t _canWed, uint8_t _canThurs, uint8_t _canFri, uint8_t _canSat, uint8_t _canSun);
        bool isDeptTimeHMChanged(uint32_t timerIdx, int32_t _canMin, int32_t _canHour);

//    private: /* Temporary public property... */
        uint32_t            creationTimeStamp;
        uint32_t            latestTimeStamp;
        uint32_t            deadBandTimeStamp;
        eChargeNowSetting   chargeNowSetting_field;
        bool                bChargeNowSetting_is_present;
        _MaxStateOfCharge   maxStateOfCharge_field;
        bool                bMaxStateOfCharge_is_present;
        bool                bPermanentMaxSoc_is_updated;
        bool                bOneOffMaxSoc_is_updated;
        eChargingModeChoice chargingModeChoice_field;
        bool                bChargingModeChoice_is_present;

        std::vector<_DepartureTimer>     departureTimer_field;
        std::vector<_DepartureTimer>     new_departureTimer_field;
        bool                bDepartureTimers_is_present;
        bool                bNewDepartureTimers_is_present;
        std::vector<_Tariff>             tariffs_field;
        bool                bTariffs_is_present;
        uint32_t            tariffVecSize;
        uint32_t            departureTimerVecSize;

        bool                bParamOutOfRange;
        std::vector<EvtTariffEndTimes> evtTariffEndTimes_field;
        std::vector<_TimeHM> canDeptTimeHM_field;

        void setChargeNowSettingsFromNGTP(svc::ChargeSettings &ngtpCS) const;
        void setMaxSOCFromNGTP(svc::ChargeSettings &ngtpCS) const;
        void setChargeModeChoicesFromNGTP(svc::ChargeSettings &ngtpCS) const;
        void delDepartureTimersFromNGTP(svc::ChargeSettings &ngtpCS) const;
        void setDepartureTimersFromNGTP(svc::ChargeSettings &ngtpCS) const;
        void setTariffsFromNGTP(svc::ChargeSettings &ngtpCS) const;
};
#endif // _EV_CHARGE_SETTINGS_H_