#include "EvtChargeSettings.h"
#include "NGTPManagerService/NGTPUtil.h"

#define LOG_TAG "[EVT]ChargeSettings"
#include "Log.h"

EvtTariffEndTimes::EvtTariffEndTimes()
{

}

uint32_t EvtTariffEndTimes::getTariffEndTimeA() const
{
    return tariffEndTimeA;
}

void EvtTariffEndTimes::setTariffEndTimeA(uint32_t _tariffEndA)
{
    tariffEndTimeA = _tariffEndA;
}

uint32_t EvtTariffEndTimes::getTariffEndTimeB() const
{
    return tariffEndTimeB;
}

void EvtTariffEndTimes::setTariffEndTimeB(uint32_t _tariffEndB)
{
    tariffEndTimeB = _tariffEndB;
}

uint32_t EvtTariffEndTimes::getTariffEndTimeC() const
{
    return tariffEndTimeC;
}

void EvtTariffEndTimes::setTariffEndTimeC(uint32_t _tariffEndC)
{
    tariffEndTimeC = _tariffEndC;
}

uint32_t EvtTariffEndTimes::getTariffEndTimeD() const
{
    return tariffEndTimeD;
}

void EvtTariffEndTimes::setTariffEndTimeD(uint32_t _tariffEndD)
{
    tariffEndTimeD = _tariffEndD;
}

EvtChargeSettings::EvtChargeSettings()
{
    resetAll();
};

void EvtChargeSettings::resetAll()
{
    tariffVecSize       = 4;
    departureTimerVecSize = 50;
    chargeNowSetting_field = 0;
    bChargeNowSetting_is_present = false;
    memset(&maxStateOfCharge_field, 0, sizeof(_MaxStateOfCharge));
    bMaxStateOfCharge_is_present = false;
    chargingModeChoice_field = 0;
    bChargingModeChoice_is_present = false;
    departureTimer_field.clear();
    bDepartureTimers_is_present = false;
    tariffs_field.clear();
    bTariffs_is_present = false;
    creationTimeStamp = 0;
    bPermanentMaxSoc_is_updated = false;
    bOneOffMaxSoc_is_updated = false;
    tariffs_field.resize(tariffVecSize);
    departureTimer_field.resize(departureTimerVecSize);
    canDeptTimeHM_field.resize(departureTimerVecSize);
    bNewDepartureTimers_is_present = false;
    bParamOutOfRange = false;
    latestTimeStamp = 0;
    deadBandTimeStamp = 0;
    evtTariffEndTimes_field.resize(4);
};

void EvtChargeSettings::printData()
{
    //LOGV("EvtChargeSetting:\n\t chargeNowSetting[%X],maxStateOfCharge(permanentMaxSoc[%X],oneOffMaxSoc[%X]),chargingModeChoice[%X],cnt_DepartureTimer[%d],cnt_Tariff[%d]", chargeNowSetting_field, maxStateOfCharge_field.permanentMaxSoc_field, maxStateOfCharge_field.oneOffMaxSoc_field, chargingModeChoice_field, cnt_DepartureTimer, cnt_Tariff);
};

eChargeNowSetting EvtChargeSettings::getChargeNowSettingField() const {
    return chargeNowSetting_field;
}
void EvtChargeSettings::setChargeNowSettingField(eChargeNowSetting _chargeNowSettingField) {
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    bChargeNowSetting_is_present = true;
    chargeNowSetting_field = _chargeNowSettingField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated ChargeNowReqExt[%u],[%u]", getChargeNowSettingField(), getCreationTimeStamp());
}

void EvtChargeSettings::setMaxStateOfChargeField(int32_t _permanentMaxSoc, int32_t _oneOffMaxSoc) {
    maxStateOfCharge_field.permanentMaxSoc_field    = _permanentMaxSoc;
    maxStateOfCharge_field.oneOffMaxSoc_field       = _oneOffMaxSoc;
    setCreationTimeStamp(UTIL::get_NGTP_time());
}

 void EvtChargeSettings::setPermanentMaxSocField(int32_t _permanentMaxSoc)
 {
    bPermanentMaxSoc_is_updated = true;
    maxStateOfCharge_field.permanentMaxSoc_field    = _permanentMaxSoc;
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    //bMaxStateOfCharge_is_present = true;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated BulkSoCSetExtUpdate[%u],[%u]", getPermanentMaxSocField(), getCreationTimeStamp());
 }

int32_t EvtChargeSettings::getPermanentMaxSocField() const
{
    return maxStateOfCharge_field.permanentMaxSoc_field;
}

void EvtChargeSettings::setPermanentMaxSocStatus(int8_t _permanentMaxSocStat)
{
    maxStateOfCharge_field.permanentMaxSoc_Status = _permanentMaxSocStat;
    bPermanentMaxSoc_is_updated = true;
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated PermanentMaxSocStatus[%u],[%u]", getPermanentMaxSocStatus(), getCreationTimeStamp());
}

int8_t EvtChargeSettings::getPermanentMaxSocStatus() const
{
    return maxStateOfCharge_field.permanentMaxSoc_Status;
}

void EvtChargeSettings::setOneOffMaxSocField(int32_t _oneOffMaxSoc)
{
    maxStateOfCharge_field.oneOffMaxSoc_field       = _oneOffMaxSoc;
    bOneOffMaxSoc_is_updated = true;
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    //bMaxStateOfCharge_is_present = true;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated BulkSoCSingleUseExtUpd[%u],[%u]", getOneOffMaxSocField(), getCreationTimeStamp());
}

int32_t EvtChargeSettings::getOneOffMaxSocField() const
{
    return maxStateOfCharge_field.oneOffMaxSoc_field;
}

void EvtChargeSettings::setOneOffMaxSocStatus(int8_t _oneOffMaxSocStat)
{
    maxStateOfCharge_field.oneOffMaxSoc_Status = _oneOffMaxSocStat;
    bOneOffMaxSoc_is_updated = true;
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated OneOffMaxSocStatus[%u],[%u]", getOneOffMaxSocStatus(), getCreationTimeStamp());
}

int8_t EvtChargeSettings::getOneOffMaxSocStatus() const
{
    return maxStateOfCharge_field.oneOffMaxSoc_Status;
}

eChargingModeChoice EvtChargeSettings::getChargingModeChoiceField() const {
    return chargingModeChoice_field;
}

void EvtChargeSettings::setChargingModeChoiceField(eChargingModeChoice _chargingModeChoiceField) {
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    bChargingModeChoice_is_present = true;
    chargingModeChoice_field = _chargingModeChoiceField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated ChargingModeChoice[%u],[%u]", getChargingModeChoiceField(), getCreationTimeStamp());
}

void EvtChargeSettings::setCreationTimeStamp(uint32_t _creationTimeStamp){
    creationTimeStamp = _creationTimeStamp;
}

uint32_t EvtChargeSettings::getCreationTimeStamp() const{
    return creationTimeStamp;
}

uint32_t EvtChargeSettings::getLatestTimeStamp() const{
    return latestTimeStamp;
}

void EvtChargeSettings::setLatestTimeStamp(uint32_t _latestTimeStamp){
    latestTimeStamp = _latestTimeStamp;
    LOGV("[setLatestTimeStamp] %u", _latestTimeStamp);
}


uint32_t EvtChargeSettings::getDeadBandTimeStamp() const
{
    return deadBandTimeStamp;
}

void EvtChargeSettings::setDeadBandTimeStamp(uint32_t _deadBandTimeStamp)
{
    deadBandTimeStamp = _deadBandTimeStamp;
    LOGV("[setDeadBandTimeStamp] %u", _deadBandTimeStamp);
}

// SRD_ReqEVC0606_v1
void EvtChargeSettings::setChargeSettingsFromNGTP(svc::ChargeSettings &ngtpCS) const
{
    if(ngtpCS.chargeNowSetting_is_present())
    {
        setChargeNowSettingsFromNGTP(ngtpCS);
    }
    if(ngtpCS.maxStateOfCharge_is_present())
    {
        setMaxSOCFromNGTP(ngtpCS);
    }
    if(ngtpCS.chargingModeChoice_is_present())
    {
        setChargeModeChoicesFromNGTP(ngtpCS);
    }
    if(ngtpCS.departureTimers_is_present())
    {
        delDepartureTimersFromNGTP(ngtpCS);
        setDepartureTimersFromNGTP(ngtpCS);
    }
    if(ngtpCS.tariffs_is_present())
    {
        setTariffsFromNGTP(ngtpCS);
    }
}

void EvtChargeSettings::setChargeNowSettingsFromNGTP(svc::ChargeSettings &ngtpCS) const
{
    eChargeNowSetting _chargeNowSetting = *(ngtpCS.get_chargeNowSetting());
    switch(_chargeNowSetting)
    {
        case svc::ChargeNowSetting_default:
        {
            LOGV("[setChargeNowSettingsFromNGTP] ChargeNowSetting_default");
            setChargeNowSettingField(_chargeNowSetting);
            break;
        }
        case svc::force_on:
        {
            LOGV("[setChargeNowSettingsFromNGTP] force_on");
            setChargeNowSettingField(_chargeNowSetting);
            break;
        }
        case svc::force_off:
        {
            LOGV("[setChargeNowSettingsFromNGTP] force_off");
            setChargeNowSettingField(_chargeNowSetting);
            break;
        }
        default:
        {
            LOGE("[setChargeNowSettingsFromNGTP] invalid Value : %d",_chargeNowSetting);
            break;
        }
    }
}

void EvtChargeSettings::setMaxSOCFromNGTP(svc::ChargeSettings &ngtpCS) const
{
     // noChange_chosen = 1, clear_chosen = 2,maxSoc_chosen = 3
     if(ngtpCS.get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc())
     {
         int _permanentMaxSoc = *(ngtpCS.get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc());
         setPermanentMaxSocField(_permanentMaxSoc);
         setPermanentMaxSocStatus(3);
         LOGV("[setMaxSOCFromNGTP]permanentMaxSoc[%d]", _permanentMaxSoc);
     }
     else if(ngtpCS.get_maxStateOfCharge()->get_permanentMaxSoc().get_clear())
     {
         setPermanentMaxSocStatus(2);
         setPermanentMaxSocField(0);
         LOGV("[setMaxSOCFromNGTP]permanentMaxSoc-Clear");
     }
     else if(ngtpCS.get_maxStateOfCharge()->get_permanentMaxSoc().get_noChange())
     {
         setPermanentMaxSocStatus(1);
         LOGV("[setMaxSOCFromNGTP]permanentMaxSoc-NoChange");
     }
     else
     {
         LOGV("[setMaxSOCFromNGTP]permanentMaxSoc-Invalid");
     }

     // noChange_chosen = 1, clear_chosen = 2,maxSoc_chosen = 3
     if(ngtpCS.get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc())
     {
         int _oneOffMaxSoc = *(ngtpCS.get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc());
         setOneOffMaxSocField(_oneOffMaxSoc);
         setOneOffMaxSocStatus(3);
         LOGV("[setMaxSOCFromNGTP]oneOffMaxSoc[%d]", _oneOffMaxSoc);
     }
     else if(ngtpCS.get_maxStateOfCharge()->get_oneOffMaxSoc().get_clear())
     {
         setOneOffMaxSocStatus(2);
		 setOneOffMaxSocField(0);
         LOGV("[setMaxSOCFromNGTP]oneOffMaxSoc-Clear");
     }
     else if(ngtpCS.get_maxStateOfCharge()->get_oneOffMaxSoc().get_noChange())
     {
         setOneOffMaxSocStatus(1);
         LOGV("[setMaxSOCFromNGTP]oneOffMaxSoc-NoChange");
     }
     else
     {
         LOGV("[setMaxSOCFromNGTP]oneOffMaxSoc-Invalid");
     }
}

void EvtChargeSettings::setChargeModeChoicesFromNGTP(svc::ChargeSettings &ngtpCS) const
{
    eChargingModeChoice _chargingModeChoice = *(ngtpCS.get_chargingModeChoice());
    switch(_chargingModeChoice)
    {
        case svc::timedChargingNotActive:
        {
            LOGV("[setChargeModeChoicesFromNGTP] timedChargingNotActive");
            setChargingModeChoiceField(_chargingModeChoice);
            break;
        }
        case svc::timedChargingActive:
        {
            LOGV("[setChargeModeChoicesFromNGTP] timedChargingActive");
            setChargingModeChoiceField(_chargingModeChoice);
            break;
        }
        default:
        {
            LOGE("[setChargeModeChoicesFromNGTP] Invalid Value : %d", _chargingModeChoice);
            break;
        }
    }
}

// SRD_ReqEVC0663_v1 : TCU process Delete Request first
void EvtChargeSettings::delDepartureTimersFromNGTP(svc::ChargeSettings &ngtpCS) const
{
    uint32_t timerIdx;
    eDepartureTimerList _DepartureTimer_list = ngtpCS.get_departureTimers()->get_timers();
    uint32_t curUpdatedDeptCnt;
    void* pOssIndex = _DepartureTimer_list.first();
    do{
         eDepartureTimer* pDepartureTimer = _DepartureTimer_list.at(pOssIndex);
         curUpdatedDeptCnt = getUpdatedDeptTimerCount();

         timerIdx = pDepartureTimer->get_timerIndex();

         // Delete Request's timerIndex Range : 0~49. (50 is excluded- NEW ADD Timer)
         if((timerIdx >= 0) && (timerIdx < 50))
         {
            _DepartureTimer           _departureTimer;
            if(!pDepartureTimer->departureTimerDefinition_is_present())
            {
                _departureTimer._bDepartureTimerDefinition_is_present  = false;
                _departureTimer._bDepartureTimer_is_updated = true;

                EvtChargeSettings::GetInstance()->departureTimer_field[timerIdx] = _departureTimer;
                LOGV("[delDepartureTimersFromNGTP] Delete Req Index[%d]", pDepartureTimer->get_timerIndex());
            }
            EvtChargeSettings::GetInstance()->bDepartureTimers_is_present = true;
            EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
         }
        pOssIndex = _DepartureTimer_list.next(pOssIndex);
    }while(pOssIndex != NULL);
    curUpdatedDeptCnt = getUpdatedDeptTimerCount();
    LOGV("[delDepartureTimersFromNGTP] Total Dept Cnt : %d", curUpdatedDeptCnt);
}

void EvtChargeSettings::setDepartureTimersFromNGTP(svc::ChargeSettings &ngtpCS) const
{
    uint32_t timerIdx;
    eDepartureTimerList _DepartureTimer_list = ngtpCS.get_departureTimers()->get_timers();
    uint32_t curUpdatedDeptCnt;

    //Initialize newDeptCnt
    uint32_t newDeptCnt = 0;

    void* pOssIndex = _DepartureTimer_list.first();
    do{
         eDepartureTimer* pDepartureTimer = _DepartureTimer_list.at(pOssIndex);
         curUpdatedDeptCnt = getUpdatedDeptTimerCount();
         // timerIdx's valid range is 0~50 (50 is New DepartureTimer)
         timerIdx = pDepartureTimer->get_timerIndex();
         if(timerIdx > 50 || timerIdx < 0)
         {
           LOGE("[setDepartureTimersFromNGTP] Invalid Timer Index Value[%d] Not added to DepartureTimer Vector", timerIdx);
         }
         else
         {
            _TimeDate                 _singleDayTimeDate;
            _DayOfWeekSchedule        _dayOfWeekSchedule;
            _TimerTarget              _timerTarget;
            _TimeHM                   _timeHM;
            _DepartureTimerDefinition _departureTimerDefinition;
            _DepartureTimer           _departureTimer;
            uint8_t                   _timeType;
            if(pDepartureTimer->departureTimerDefinition_is_present())
            {
                //timerTarget - SingleDay.
                if(pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay())
                {
                    /*
                    _singleDayTimeDate = {  pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_day()
                                        , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_month()
                                        , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_year()};
                    */
                 _singleDayTimeDate.day = pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_day();
                 _singleDayTimeDate.month = pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_month();
                 _singleDayTimeDate.year = pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_year();
                 //timeType - singleEvent
                  _timeType = 0;
                }

                //timerTarget - Repeat Schedule.
                if(pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule())
                {
                  _dayOfWeekSchedule = {pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_monday()
                                      , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_tuesday()
                                      , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_wednesday()
                                      , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_thursday()
                                      , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_friday()
                                      , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_saturday()
                                      , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_sunday()};
                 //timeType - recurring Event
                  _timeType = 1;
                }

                // TimerTarget (Member of DepartureTimerDefinition)
                _timerTarget = {_timeType, _singleDayTimeDate, _dayOfWeekSchedule};

                //departureTime
                _timeHM = { pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_hour()
                          , pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_minute()};

                // DepartureTimerDefinition (Member of DepartureTimer)
                _departureTimerDefinition = { pDepartureTimer->get_departureTimerDefinition()->get_timerType()
                                            , _timeHM
                                            , _timerTarget };

                // DepartureTimer
                _departureTimer = {  timerIdx
                                   , _departureTimerDefinition
                                   , true
                                   , true};

                //Print Log within one line (SingleDay Enabled)
                if(pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay())
                {
                    LOGV("[setDepartureTimersFromNGTP]timerIndex[%d] timerType[%d] TimeHM h[%d] m[%d] TimeDate d[%d] m[%d] y[%d]"
                        , timerIdx
                        , _departureTimerDefinition.timerType_field
                        , _timeHM.hour_field
                        , _timeHM.minute_field
                        , _singleDayTimeDate.day
                        , _singleDayTimeDate.month
                        , _singleDayTimeDate.year);
                }
                //Print Log within one line (RepeatSchedule Enabled)
                if(pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule())
                {
                    LOGV("[setDepartureTimersFromNGTP]timerIndex[%d] timerType[%d] DayOfWeekSchedule mon[%d]tue[%d]wed[%d]thurs[%d]fri[%d]sat[%d]sun[%d]"
                        , timerIdx
                        , _departureTimerDefinition.timerType_field
                        , _timeHM.hour_field
                        , _timeHM.minute_field
                        , _dayOfWeekSchedule.monday_field
                        , _dayOfWeekSchedule.tuesday_field
                        , _dayOfWeekSchedule.wednesday_field
                        , _dayOfWeekSchedule.thursday_field
                        , _dayOfWeekSchedule.friday_field
                        , _dayOfWeekSchedule.saturday_field
                        , _dayOfWeekSchedule.sunday_field);
                }
                // Add to DepartureTimer Vector
                if(timerIdx == 50)
                {
                    if(curUpdatedDeptCnt >= 50)
                    {
                        LOGV("[setDepartureTimersFromNGTP] Max DeptTimer Count Full. No More New Timer");
                        EvtChargeSettings::GetInstance()->bParamOutOfRange = true;
                    }
                    else
                    {
                        EvtChargeSettings::GetInstance()->new_departureTimer_field.push_back(_departureTimer);
                        LOGV("[setDepartureTimersFromNGTP] new Dept Count[%d] VecSize[%d]", newDeptCnt ,EvtChargeSettings::GetInstance()->new_departureTimer_field.size());
                    }
                    newDeptCnt++;
                }
                else
                {
                    EvtChargeSettings::GetInstance()->departureTimer_field[timerIdx] = _departureTimer;
                    LOGV("[setDepartureTimersFromNGTP] DepartureTimer Set Count[%d] VecSize[%d]", curUpdatedDeptCnt ,EvtChargeSettings::GetInstance()->departureTimer_field.size());
                }
            }
            else
            {
                LOGE("[setDepartureTimersFromNGTP] DepartimerDefinition is not present[%d]", pDepartureTimer->get_timerIndex());
            }
            EvtChargeSettings::GetInstance()->bDepartureTimers_is_present = true;
            EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
         }
        pOssIndex = _DepartureTimer_list.next(pOssIndex);
    }while(pOssIndex != NULL);

    // SRD_ReqEVC0663_v1 - Parameter OutofRange Check : Current + New Timer Count > 50
    curUpdatedDeptCnt = getUpdatedDeptTimerCount() + newDeptCnt;
    EvtChargeSettings::GetInstance()->bParamOutOfRange = (curUpdatedDeptCnt > 50) ? true : false;
    LOGV("[setDepartureTimersFromNGTP] Total Cnt[%d] ParamOutOfRange[%s]", curUpdatedDeptCnt, bParamOutOfRange?"True":"False");
}

void EvtChargeSettings::setTariffsFromNGTP(svc::ChargeSettings &ngtpCS) const
{
    eTariffList _Tariff_list = ngtpCS.get_tariffs()->get_tariffs();
    void* pOssIndex = _Tariff_list.first();

    do{
        eTariff* pTariff = _Tariff_list.at(pOssIndex);
        // pOssIndex should be checked if it is null or not.
        if(pOssIndex == NULL)
        {
            LOGV("STFN-pOssIndex is null");
            continue;
        }
        _Tariff evtTariff;
        _TariffDefinition evtTariffDefinition;
        uint32_t tariffIndex = pTariff->get_tariffIndex();
        if( pTariff->tariffDefinition_is_present()
            && pTariff->get_tariffDefinition()->get_isEnabled())
        {
            //
            int32_t endTimeTmp;
            uint8_t bandTypeTmp;

            bool bTariffZoneB = false;
            bool bTariffZoneC = false;
            bool bTariffZoneD = false;
            bool bTariffZoneE = false;

            //_Tariff evtTariff;
            //_TariffDefinition evtTariffDefinition;
            _DayOfWeekSchedule evtDayOfWeekSchedule;
            _TariffZone evtTariffZoneA, evtTariffZoneB, evtTariffZoneC, evtTariffZoneD, evtTariffZoneE;
            _TimeHM evtTimeHM_A, evtTimeHM_B, evtTimeHM_C, evtTimeHM_D, evtTimeHM_E;

            //Repeat Schedule
            evtDayOfWeekSchedule = {  pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday()
                                    , pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday()
                                    , pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday()
                                    , pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday()
                                    , pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday()
                                    , pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday()
                                    , pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday()};

            //Tariff ZoneA
            evtTimeHM_A = {  pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_hour()
                           , pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_minute()};
            evtTariffZoneA = {evtTimeHM_A, pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType()};

            //Tariff ZoneB
            if(pTariff->get_tariffDefinition()->tariffZoneB_is_present())
            {
                evtTimeHM_B = {  pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_hour()
                               , pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_minute()};
                evtTariffZoneB = {evtTimeHM_B, pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType()};
                bTariffZoneB = true;
            }

            //Tariff ZoneC
            if(pTariff->get_tariffDefinition()->tariffZoneC_is_present())
            {
                evtTimeHM_C = {  pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_hour()
                               , pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_minute()};
                evtTariffZoneC = {evtTimeHM_C, pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType()};
                bTariffZoneC = true;
            }

            //Tariff ZoneD
            if(pTariff->get_tariffDefinition()->tariffZoneD_is_present())
            {
                evtTimeHM_D = {  pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_hour()
                               , pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_minute()};
                evtTariffZoneD = {evtTimeHM_D, pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType()};
                bTariffZoneD = true;
            }

            //Tariff ZoneE
            if(pTariff->get_tariffDefinition()->tariffZoneE_is_present())
            {
                evtTariffZoneE = {evtTimeHM_E, pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType()};
                bTariffZoneE = true;
            }

           //TariffDefinition (Member of Tariff)
           evtTariffDefinition = {(pTariff->get_tariffDefinition()->get_isEnabled() == 1) ? true : false
                                , evtDayOfWeekSchedule
                                , evtTariffZoneA
                                , evtTariffZoneB
                                , evtTariffZoneC
                                , evtTariffZoneD
                                , evtTariffZoneE
                                , true
                                , bTariffZoneB
                                , bTariffZoneC
                                , bTariffZoneD
                                , bTariffZoneE };

           //Tariff
           evtTariff = {true, tariffIndex, evtTariffDefinition, true};
           LOGV("[setTariffsFromNGTP] isEnable[%d]A[%d]B[%d]C[%d]D[%d]E[%d] DayOfWeek Mon[%d]Tue[%d]Wed[%d]Thur[%d]Fri[%d]Sat[%d]Sun[%d]"
                , evtTariffDefinition.isEnabled_field
                , evtTariffDefinition._bTariffZoneA_is_present
                , evtTariffDefinition._bTariffZoneB_is_present
                , evtTariffDefinition._bTariffZoneC_is_present
                , evtTariffDefinition._bTariffZoneD_is_present
                , evtTariffDefinition._bTariffZoneE_is_present
                , evtDayOfWeekSchedule.monday_field
                , evtDayOfWeekSchedule.tuesday_field
                , evtDayOfWeekSchedule.wednesday_field
                , evtDayOfWeekSchedule.thursday_field
                , evtDayOfWeekSchedule.friday_field
                , evtDayOfWeekSchedule.saturday_field
                , evtDayOfWeekSchedule.sunday_field);

           LOGV("[setTariffsFromNGTP]EndTime A:h[%d]m[%d] B:h[%d]m[%d] C:h[%d]m[%d] D:h[%d]m[%d] BandType A[%d]B[%d]C[%d]D[%d]E[%d]"
                , evtTimeHM_A.hour_field, evtTimeHM_A.minute_field
                , evtTimeHM_B.hour_field, evtTimeHM_B.minute_field
                , evtTimeHM_C.hour_field, evtTimeHM_C.minute_field
                , evtTimeHM_D.hour_field, evtTimeHM_D.minute_field
                , evtTariffZoneA.bandType_field
                , evtTariffZoneB.bandType_field
                , evtTariffZoneC.bandType_field
                , evtTariffZoneD.bandType_field
                , evtTariffZoneE.bandType_field);
        }
        else
        {
            if(!pTariff->tariffDefinition_is_present())
            {
                LOGE("STFN-isPresent[%d]", pTariff->tariffDefinition_is_present());
            }
            else
            {
                LOGE("STFN-isPresent[%d]Enable[%d]", pTariff->tariffDefinition_is_present(), pTariff->get_tariffDefinition()->get_isEnabled());
            }
            evtTariff = {true, tariffIndex, evtTariffDefinition, false};
        }
        // Add to Tariff Vector
        EvtChargeSettings::GetInstance()->tariffs_field[tariffIndex] = evtTariff;
        EvtChargeSettings::GetInstance()->bTariffs_is_present = true;
        pOssIndex = _Tariff_list.next(pOssIndex);
    }while(pOssIndex != NULL);
}

uint32_t EvtChargeSettings::getUpdatedDeptTimerCount()
{
    uint32_t timerCount = 0;
    _DepartureTimer _departureTimer;

    for( std::vector<_DepartureTimer>::size_type i ; i < departureTimer_field.size() ; i++)
    {
        _departureTimer = departureTimer_field[i];
        if(_departureTimer._bDepartureTimer_is_updated
            && _departureTimer._bDepartureTimerDefinition_is_present)
        {
            timerCount++;
        }
    }

    // In Case of DeparturTimer Delete Request, count will be reduced and bParamOutOfRange should be false.
    if(timerCount < 50)
    {
        EvtChargeSettings::GetInstance()->bParamOutOfRange = false;
    }
    LOGV("[getUpdatedDeptTimerCount] count[%d]", timerCount);
    return timerCount;
}

uint8_t EvtChargeSettings::getBandType(uint8_t _tariffIndex, uint8_t _tariffZone)
{
    _Tariff _tmpTar = tariffs_field[_tariffIndex];
    switch(_tariffZone)
    {
        case ZoneA:
            return _tmpTar.tariffDefinition_field.tariffZoneA_field.bandType_field;
            break;
        case ZoneB:
            return _tmpTar.tariffDefinition_field.tariffZoneB_field.bandType_field;
            break;
        case ZoneC:
            return _tmpTar.tariffDefinition_field.tariffZoneC_field.bandType_field;
            break;
        case ZoneD:
            return _tmpTar.tariffDefinition_field.tariffZoneD_field.bandType_field;
            break;
        case ZoneE:
            return _tmpTar.tariffDefinition_field.tariffZoneE_field.bandType_field;
            break;
        default:
            return 0;
            break;
    }
}

bool EvtChargeSettings::isTariffZoneChanged(uint8_t _tariffIdx, uint8_t _canBandType, int32_t _canEndTime, uint8_t _tcuBandType, int32_t _tcuEndTime)
{
    if((_canBandType != _tcuBandType) ||( _canEndTime != _tcuEndTime))
    {
        LOGV("[Comp Tariff] Idx[%d] Band-C[%d]T[%d] EndTimeC[%d]T[%d]", _tariffIdx, _canBandType, _tcuBandType, _canEndTime, _tcuEndTime);
        return true;
    }
    else
    {
        return false;
    }
}

bool EvtChargeSettings::isDayOfWeekChanged(uint8_t _idx, uint8_t _canMon, uint8_t _canTue, uint8_t _canWed, uint8_t _canThurs, uint8_t _canFri, uint8_t _canSat, uint8_t _canSun)
{
    bool bValueChanged = false;
    _Tariff _tmpTar = tariffs_field[_idx];
    _DayOfWeekSchedule tmpSchedule = _tmpTar.tariffDefinition_field.repeatSchedule_field;

    uint8_t tcuMon = (tmpSchedule.monday_field) ? 1 : 0;
    uint8_t tcuTue = (tmpSchedule.tuesday_field) ? 1 : 0;
    uint8_t tcuWed = (tmpSchedule.wednesday_field) ? 1 : 0;
    uint8_t tcuThurs = (tmpSchedule.thursday_field) ? 1 : 0;
    uint8_t tcuFri = (tmpSchedule.friday_field) ? 1 : 0;
    uint8_t tcuSat = (tmpSchedule.saturday_field) ? 1 : 0;
    uint8_t tcuSun = (tmpSchedule.sunday_field) ? 1 : 0;

    if( (_canMon != tcuMon) || (_canTue != tcuTue) || (_canWed != tcuWed) || (_canThurs != tcuThurs) || (_canFri != tcuFri) || (_canSat != tcuSat) || (_canSun != tcuSun) )
    {
        LOGV("[Comp Tariff] DayOfWeek Changed Idx[%d] CAN_TCU [%d_%d] [%d_%d] [%d_%d] [%d_%d] [%d_%d] [%d_%d] [%d_%d]"
        , _idx, _canMon, tcuMon, _canTue, tcuTue, _canWed, tcuWed, _canThurs, tcuThurs, _canFri, tcuFri, _canSat, tcuSat, _canSun, tcuSun );
        bValueChanged = true;
    }
    else
    {
        LOGV("[Comp Tariff] DayOfWeek Not Changed Idx[%d] CAN_TCU [%d_%d] [%d_%d] [%d_%d] [%d_%d] [%d_%d] [%d_%d] [%d_%d]"
        , _idx, _canMon, tcuMon, _canTue, tcuTue, _canWed, tcuWed, _canThurs, tcuThurs, _canFri, tcuFri, _canSat, tcuSat, _canSun, tcuSun );
    }

    return bValueChanged;
}

bool EvtChargeSettings::isTariffActiveChanged(uint8_t _idx, int32_t _canActive)
{
    _Tariff _tmpTar = tariffs_field[_idx];
    bool bValueChanged = false;

    int32_t tcuTarActive = _tmpTar.tariffDefinition_field.isEnabled_field ? 1 : 0;
    if(_canActive != tcuTarActive)
    {
        LOGV("[Comp Tariff] Tar Active Changed Idx[%d]CAN[%d]TCU[%d]", _idx, _canActive, tcuTarActive);
        bValueChanged = true;
    }
    else
    {
        LOGV("[Comp Tariff] Tar Active Not Changed Idx[%d]CAN[%d]TCU[%d]", _idx, _canActive, tcuTarActive);
    }
    return bValueChanged;
}

bool EvtChargeSettings::isDeptActiveChanged(uint32_t timerIdx, int32_t _canActive)
{
    bool bChanged = false;

    _DepartureTimer _tmpDeptTimer = departureTimer_field[timerIdx];
    uint32_t tcuActive = _tmpDeptTimer.departureTimerDefinition_field.timerType_field;

    //TODO Implement Comparison
    if(_canActive != tcuActive)
    {
        bChanged = true;
    }
    LOGV("[Comp DeptTimer] isActiveChanged[%s]Idx[%d] [%d_%d]", bChanged ? "T" : "F", timerIdx, _canActive, tcuActive);
    return bChanged;
}

bool EvtChargeSettings::isDeptTimeTypeChanged(uint32_t timerIdx, int32_t _canTimeType)
{
    bool bChanged = false;
    _DepartureTimer _tmpDeptTimer = departureTimer_field[timerIdx];
    uint8_t tcuTimeType = _tmpDeptTimer.departureTimerDefinition_field.timerTarget_field.timeType_field;

    if(_canTimeType != tcuTimeType)
    {
        bChanged = true;
    }
    LOGV("[Comp DeptTimer] isDeptTimeTypeChanged[%s]Idx[%d] [%d_%d]", bChanged ? "T" : "F", timerIdx, _canTimeType, tcuTimeType);
    return bChanged;
}

bool EvtChargeSettings::isDeptTimeDateChanged(uint32_t timerIdx, int32_t _canDate, int32_t _canMnth, int32_t _canYear)
{
    bool bChanged = false;

    _DepartureTimer _tmpDeptTimer = departureTimer_field[timerIdx];
    _TimeDate _tmpTimeDate = _tmpDeptTimer.departureTimerDefinition_field.timerTarget_field.singleDay_field;
    int32_t tcuDate = _tmpTimeDate.can_Day;
    int32_t tcuMnth = _tmpTimeDate.can_Month;
    int32_t tcuYear = _tmpTimeDate.can_Year;

    if((_canDate != tcuDate) ||(_canMnth != tcuMnth) || (_canYear != tcuYear))
    {
        bChanged = true;
    }
    LOGV("[Comp DeptTimer] isDeptTimeDateChanged[%s]Idx[%d] D[%d_%d]M[%d_%d]Y[%d_%d]"
        , bChanged ? "T" : "F", timerIdx, _canDate, tcuDate, _canMnth, tcuMnth, _canYear, tcuYear);
    return bChanged;
}

bool EvtChargeSettings::isDeptDayOfWeekChanged(uint32_t timerIdx, uint8_t _canMon, uint8_t _canTue, uint8_t _canWed, uint8_t _canThurs, uint8_t _canFri, uint8_t _canSat, uint8_t _canSun)
{
    bool bChanged = false;
    _DepartureTimer _tmpDeptTimer = departureTimer_field[timerIdx];
    _DayOfWeekSchedule _tmpDows = _tmpDeptTimer.departureTimerDefinition_field.timerTarget_field.repeatSchedule_field;

    uint8_t tcuMon  =      _tmpDows.monday_field ? 1 : 0;
    uint8_t tcuTues  =     _tmpDows.tuesday_field ? 1 : 0;
    uint8_t tcuWed   =     _tmpDows.wednesday_field ? 1 : 0;
    uint8_t tcuThurs  =    _tmpDows.thursday_field ? 1 : 0;
    uint8_t tcuFri   =     _tmpDows.friday_field ? 1 : 0;
    uint8_t tcuSat   =     _tmpDows.saturday_field ? 1 : 0;
    uint8_t tcuSun   =     _tmpDows.sunday_field ? 1 : 0;

    if((_canMon       != tcuMon)
        ||( _canTue   != tcuTues)
        ||(_canWed    != tcuWed)
        ||(_canThurs  != tcuThurs)
        ||(_canFri    != tcuFri)
        ||(_canSat    != tcuSat)
        ||(_canSun    != tcuSun))
    {
        bChanged =  true;
    }
    LOGV("[Comp DeptTimer] isDeptDayOfWeekChanged[%s]Idx[%d] M[%d_%d]T[%d_%d]W[%d_%d]Th[%d_%d]F[%d_%d]S[%d_%d]Su[%d_%d]"
        , bChanged ? "T" : "F", timerIdx
        , _canMon, tcuMon
        , _canTue , tcuTues
        , _canWed, tcuWed
        , _canThurs, tcuThurs
        , _canFri, tcuFri
        , _canSat, tcuSat
        , _canSun, tcuSun);
    return bChanged;
}

bool EvtChargeSettings::isDeptTimeHMChanged(uint32_t timerIdx, int32_t _canMin, int32_t _canHour)
{
    bool bChanged = false;
    //_DepartureTimer _tmpDeptTimer = departureTimer_field[timerIdx];
    //_TimeHM _tmpTimeHM = _tmpDeptTimer.departureTimerDefinition_field.departureTime_field;
    _TimeHM _tmpTimeHM = canDeptTimeHM_field[timerIdx];

    uint32_t tcuHour = _tmpTimeHM.hour_field;
    uint32_t tcuMin = _tmpTimeHM.minute_field;
    if((_canMin != tcuMin) || (_canHour != tcuHour))
    {
        bChanged =  true;
    }

    LOGV("[Comp DeptTimer] isDeptTimeHMChanged[%s]Idx[%d] M[%d_%d]H[%d_%d]"
        , bChanged ? "T" : "F", timerIdx, _canMin, tcuMin, _canHour, tcuHour);

    return bChanged;
}