// Inline Functioin for EVChargeApp.
#include "EvtNextDepartureTimer.h"

#define SIG_CHRG(NUM, TAIL) Signal_TimedCharge##NUM##TAIL
#define EV_COMPARE_TARIFF(A) do{ \
    if(!pTariff->tariffDefinition_is_present()) \
    { \
        LOGV("[EV_COMPARE_TARIFF]Idx[%d] TariffDefinition is not Present", pTariff->get_tariffIndex()); \
        if(EvtVifManager::GetInstance()->queryCanData(SIG_CHRG(A, Active_RX)) == 0) \
        { \
          return true; \
        } \
        else \
        { \
           return false; \
        } \
    } \
    if(pTariff->get_tariffDefinition()->get_isEnabled()==0) \
    { \
        LOGV("[EV_COMPARE_TARIFF]Idx[%d] TariffDefinition is not Enable", pTariff->get_tariffIndex()); \
        if(EvtVifManager::GetInstance()->queryCanData(SIG_CHRG(A, Active_RX)) == 0) \
        { \
            return true; \
        } \
        else \
        { \
            return false; \
        } \
    } \
    if( \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, Active_RX))    == pTariff->get_tariffDefinition()->get_isEnabled()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveMon_RX)) == pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveTues_RX))== pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveWed_RX)) == pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveThur_RX))== pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveFri_RX)) == pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveSat_RX)) == pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday()) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveSun_RX)) == pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday()) && \
        /*Tariff ZoneA*/ \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeA_RX))== pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_minute()/15) ) && \
        (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneA_RX))   == pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType()) \
    ){  \
    }else{  \
        LOGV("[EV_COMPARE_TARIFF] Tariff Value different Idx[%d] Print Value[CAN_NGTP] Enable[%d_%d] M[%d_%d] T[%d_%d] W[%d_%d] Th[%d_%d] F[%d_%d] Sa[%d_%d] Su[%d_%d] ZoneA EndTime[%d_%d] band[%d_%d]" \
        , pTariff->get_tariffIndex() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, Active_RX)) \
        , pTariff->get_tariffDefinition()->get_isEnabled() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveMon_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveTues_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveWed_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveThur_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveFri_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveSat_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ActiveSun_RX)) \
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneA_RX)) \
        , pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType() \
        , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeA_RX)) \
        , pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_minute()/15) \
        ); \
        return false; \
    } \
    /*Tariff ZoneB*/ \
    if(pTariff->get_tariffDefinition()->tariffZoneB_is_present()) \
    { \
        if( \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeB_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_minute()/15) ) && \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneB_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType()) \
        ){ \
        }else{ \
            /*Log prints only in case of value mismatching*/ \
            LOGV("[EV_COMPARE_TARIFF] Tariff Value different Idx[%d] Print Value[CAN_NGTP] ZoneB band[%d_%d] EndTime[%d_%d]" \
                , pTariff->get_tariffIndex() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneB_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeB_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_minute()/15) \
                ); \
            return false; \
        } \
    } \
    else \
    { \
        LOGE("[EV_COMPARE_TARIFF]Idx[%d] ZoneB is not Present", pTariff->get_tariffIndex()); \
    } \
    /*Tariff ZoneC*/ \
    if(pTariff->get_tariffDefinition()->tariffZoneC_is_present()) \
    { \
        if( \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeC_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_minute()/15)) && \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneC_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType()) \
        ){ \
        }else{ \
                /*Log prints only in case of value mismatching*/ \
                LOGV("[EV_COMPARE_TARIFF] Tariff Value different Idx[%d] Print Value[CAN_NGTP] ZoneC band[%d_%d] EndTime[%d_%d]" \
                , pTariff->get_tariffIndex() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneC_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeC_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_minute()/15) \
                ); \
            return false; \
        } \
    } \
    else \
    { \
        LOGE("[EV_COMPARE_TARIFF]Idx[%d] ZoneC is not Present", pTariff->get_tariffIndex()); \
    } \
    /*Tariff ZoneD*/ \
    if(pTariff->get_tariffDefinition()->tariffZoneD_is_present()) \
    { \
        if( \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeD_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_minute()/15)) && \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneD_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType()) \
        ){  \
        }else{ \
                /*Log prints only in case of value mismatching*/ \
                LOGV("[EV_COMPARE_TARIFF] Tariff Value different Idx[%d] Print Value[CAN_NGTP] ZoneD band[%d_%d] EndTime[%d_%d]" \
                , pTariff->get_tariffIndex() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneD_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, EndTimeD_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_hour()*4 + (pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_minute()/15) \
                ); \
            return false; \
        } \
    } \
    else \
    { \
        LOGE("[EV_COMPARE_TARIFF]Idx[%d] ZoneD is not Present", pTariff->get_tariffIndex()); \
    } \
    /*Tariff ZoneE*/ \
    if(pTariff->get_tariffDefinition()->tariffZoneE_is_present()) \
    { \
        if( \
            (EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneE_RX)) == pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType()) \
        ){ \
        }else{ \
                /*Log prints only in case of value mismatching*/ \
                LOGV("[EV_COMPARE_TARIFF] Tariff Value different Idx[%d] Print Value[CAN_NGTP] ZoneE band[%d_%d]" \
                , pTariff->get_tariffIndex() \
                , EvtVifManager::GetInstance()->queryCanData(SIG_CHRG( A, ZoneE_RX)) \
                , pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType() \
                ); \
            return false; \
        } \
    } \
    else \
    { \
        LOGE("[EV_COMPARE_TARIFF]Idx[%d] ZoneE is not Present", pTariff->get_tariffIndex()); \
    } \
    return true; \
}while(0)


inline bool check_ElectricityPeakTimesExtUpdate1(svc::Tariff* pTariff)
{
    EV_COMPARE_TARIFF(1);
}

inline bool check_ElectricityPeakTimesExtUpdate2(svc::Tariff* pTariff)
{
    EV_COMPARE_TARIFF(2);
}

inline bool check_ElectricityPeakTimesExtUpdate3(svc::Tariff* pTariff)
{
    EV_COMPARE_TARIFF(3);
}

inline bool check_ElectricityPeakTimesExtUpdate4(svc::Tariff* pTariff)
{
    EV_COMPARE_TARIFF(4);
}
#undef EV_COMPARE_TARIFF
#undef SIG_CHRG

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define QUERY_CAN(B) pEvCanFrame->search(B)
#define SIG_CHRGR(NUM, TAIL) Signal_TimedCharge##NUM##TAIL
#define SET_TIME(A) TimeHM((A)/60, (A)%60)
inline void rcv_Notification_GWM_PMZ_R_Hybrid(EvCanFrame *pEvCanFrame)
{
    int endTimeTmp;
    uint8_t bandTypeTmp;
    uint32_t tariffIdx = 0;
    int32_t endHourTmp;
    int32_t endMinTmp;
    _Tariff evtTariff;
    _TariffDefinition evtTariffDefinition;
    _DayOfWeekSchedule evtDayOfWeekSchedule;
    _TariffZone evtTariffZoneA, evtTariffZoneB, evtTariffZoneC, evtTariffZoneD, evtTariffZoneE;
    _TimeHM evtTimeHM_A, evtTimeHM_B, evtTimeHM_C, evtTimeHM_D, evtTimeHM_E;

    bool isValueChanged = false;

    // TariffZone A
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(1, ZoneA_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(1, EndTimeA_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(0, ZoneA)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].getTariffEndTimeA()))
    {
        LOG_EV("[Comp Tariff]A Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_A = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].setTariffEndTimeA(endTimeTmp);
    evtTariffZoneA = { evtTimeHM_A, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone B
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(1, ZoneB_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(1, EndTimeB_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(0, ZoneB)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].getTariffEndTimeB()))
    {
        LOG_EV("[Comp Tariff]B Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_B = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].setTariffEndTimeB(endTimeTmp);
    evtTariffZoneB = { evtTimeHM_B, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone C
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(1, ZoneC_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(1, EndTimeC_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(0, ZoneC)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].getTariffEndTimeC()))
    {
        LOG_EV("[Comp Tariff]C Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_C = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].setTariffEndTimeC(endTimeTmp);
    evtTariffZoneC = { evtTimeHM_C, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone D
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(1, ZoneD_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(1, EndTimeD_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(0, ZoneD)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].getTariffEndTimeD()))
    {
        LOG_EV("[Comp Tariff]D Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_D = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[0].setTariffEndTimeD(endTimeTmp);
    evtTariffZoneD = { evtTimeHM_D, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone E
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(1, ZoneE_RX));
    if(bandTypeTmp != EvtChargeSettings::GetInstance()->getBandType(0, ZoneE))
    {
        LOG_EV("[Comp Tariff]E Changed Idx[%d] Band-C[%d]T[%d]", tariffIdx, bandTypeTmp, EvtChargeSettings::GetInstance()->getBandType(0, ZoneE));
        isValueChanged = true;
    }
    evtTariffZoneE = { evtTimeHM_E, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid]EndTimeA h[%d] m[%d] EndTimeB h[%d] m[%d] EndTimeC h[%d] m[%d] EndTimeD h[%d] m[%d]"
            , evtTimeHM_A.hour_field, evtTimeHM_A.minute_field, evtTimeHM_B.hour_field, evtTimeHM_B.minute_field
            , evtTimeHM_C.hour_field, evtTimeHM_C.minute_field, evtTimeHM_D.hour_field, evtTimeHM_D.minute_field );

    LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid] TariffZoneA BandTypeField[%d] B-BandTypeField[%d] C-BandTypeField[%d] D-BandTypeField[%d] E-BandTypeField[%d]"
            , evtTariffZoneA.bandType_field
            , evtTariffZoneB.bandType_field
            , evtTariffZoneC.bandType_field
            , evtTariffZoneD.bandType_field
            , evtTariffZoneE.bandType_field );

    // DayOfWeekSchedule (Member of TariffDefinition)
    if(EvtChargeSettings::GetInstance()->isDayOfWeekChanged(0
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveMon_RX))
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveTues_RX))
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveWed_RX))
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveThur_RX))
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveFri_RX))
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveSat_RX))
                                                             , QUERY_CAN(SIG_CHRGR(1, ActiveSun_RX))))
    {
        isValueChanged = true;
    }
    evtDayOfWeekSchedule = {  QUERY_CAN(SIG_CHRGR(1, ActiveMon_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(1, ActiveTues_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(1, ActiveWed_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(1, ActiveThur_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(1, ActiveFri_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(1, ActiveSat_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(1, ActiveSun_RX)) == 1 ? true : false
                           };
    LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid] DayOfWeekSchedule Mon[%d] Tue[%d] Wed[%d] Thurs[%d] Fri[%d] Sat[%d] Sun[%d]"
          , evtDayOfWeekSchedule.monday_field, evtDayOfWeekSchedule.tuesday_field,evtDayOfWeekSchedule.wednesday_field
          , evtDayOfWeekSchedule.thursday_field, evtDayOfWeekSchedule.friday_field, evtDayOfWeekSchedule.saturday_field
          , evtDayOfWeekSchedule.sunday_field);
    // compare Tariff Active
    if(EvtChargeSettings::GetInstance()->isTariffActiveChanged(0, QUERY_CAN(SIG_CHRGR(1, Active_RX))))
    {
        isValueChanged = true;
    }

    //TariffDefinition (Member of Tariff)
    evtTariffDefinition = { QUERY_CAN(SIG_CHRGR(1, Active_RX)) == 1 ? true : false
                            , evtDayOfWeekSchedule
                            , evtTariffZoneA
                            , evtTariffZoneB
                            , evtTariffZoneC
                            , evtTariffZoneD
                            , evtTariffZoneE
                            , true
                            , true
                            , true
                            , true
                            , true };
    LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid] isEnabled_field [%d] ZoneA[%d] ZoneB[%d] ZoneC[%d] ZoneD[%d] ZoneE[%d]"
            , evtTariffDefinition.isEnabled_field
            , evtTariffDefinition._bTariffZoneA_is_present
            , evtTariffDefinition._bTariffZoneB_is_present
            , evtTariffDefinition._bTariffZoneC_is_present
            , evtTariffDefinition._bTariffZoneD_is_present
            , evtTariffDefinition._bTariffZoneE_is_present);

    //Tariff
    evtTariff = {true, tariffIdx, evtTariffDefinition, true};

    // Add to Tariff Vector
    EvtChargeSettings::GetInstance()->tariffs_field[tariffIdx] = evtTariff;
    EvtChargeSettings::GetInstance()->bTariffs_is_present = true;

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());
    // send to TSP
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;

    // #if defined(PCR041ENABLE)
    // // Signal Echo
    // if(!EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    // {
    //     EvtVifManager::GetInstance()->triggerHandler(CAN_FRAME_ECHO, SignalFrame_GWM_PMZ_R_Hybrid_RX);
    // }
    // else
    // {
    //     LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid] IMC_IGNR_STATE. Not Sending ECHO");
    // }
    // #endif

    if(isValueChanged)
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid] value was changed. Sending CHARGE_SETTING_UPDATE");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_R_Hybrid] value was not changed. Not Sending CHARGE_SETTING_UPDATE");
    }
}

inline void rcv_Notification_GWM_PMZ_S_Hybrid(EvCanFrame *pEvCanFrame)
{
    int endTimeTmp;
    uint8_t bandTypeTmp;
    uint32_t tariffIdx = 1;
    int32_t endHourTmp;
    int32_t endMinTmp;
    _Tariff evtTariff;
    _TariffDefinition evtTariffDefinition;
    _DayOfWeekSchedule evtDayOfWeekSchedule;
    _TariffZone evtTariffZoneA, evtTariffZoneB, evtTariffZoneC, evtTariffZoneD, evtTariffZoneE;
    _TimeHM evtTimeHM_A, evtTimeHM_B, evtTimeHM_C, evtTimeHM_D, evtTimeHM_E;

    bool isValueChanged = false;

    // TariffZone A
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(2, ZoneA_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(2, EndTimeA_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(1, ZoneA)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].getTariffEndTimeA()))
    {
        LOG_EV("[Comp Tariff]A Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_A = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].setTariffEndTimeA(endTimeTmp);
    evtTariffZoneA = { evtTimeHM_A, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone B
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(2, ZoneB_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(2, EndTimeB_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(1, ZoneB)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].getTariffEndTimeB()))
    {
        LOG_EV("[Comp Tariff]B Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_B = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].setTariffEndTimeB(endTimeTmp);
    evtTariffZoneB = { evtTimeHM_B, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone C
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(2, ZoneC_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(2, EndTimeC_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(1, ZoneC)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].getTariffEndTimeC()))
    {
        LOG_EV("[Comp Tariff]C Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_C = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].setTariffEndTimeC(endTimeTmp);
    evtTariffZoneC = { evtTimeHM_C, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone D
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(2, ZoneD_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(2, EndTimeD_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(1, ZoneD)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].getTariffEndTimeD()))
    {
        LOG_EV("[Comp Tariff]D Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_D = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[1].setTariffEndTimeD(endTimeTmp);
    evtTariffZoneD = { evtTimeHM_D, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone E
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(2, ZoneE_RX));
    if(bandTypeTmp != EvtChargeSettings::GetInstance()->getBandType(1, ZoneE))
    {
        LOG_EV("[Comp Tariff]E Changed Idx[%d] Band-C[%d]T[%d]", tariffIdx, bandTypeTmp, EvtChargeSettings::GetInstance()->getBandType(1, ZoneE));
        isValueChanged = true;
    }
    evtTariffZoneE = { evtTimeHM_E, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid]EndTimeA h[%d] m[%d] EndTimeB h[%d] m[%d] EndTimeC h[%d] m[%d] EndTimeD h[%d] m[%d]"
            , evtTimeHM_A.hour_field, evtTimeHM_A.minute_field, evtTimeHM_B.hour_field, evtTimeHM_B.minute_field
            , evtTimeHM_C.hour_field, evtTimeHM_C.minute_field, evtTimeHM_D.hour_field, evtTimeHM_D.minute_field );

    LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid] TariffZoneA BandTypeField[%d] B-BandTypeField[%d] C-BandTypeField[%d] D-BandTypeField[%d] E-BandTypeField[%d]"
            , evtTariffZoneA.bandType_field
            , evtTariffZoneB.bandType_field
            , evtTariffZoneC.bandType_field
            , evtTariffZoneD.bandType_field
            , evtTariffZoneE.bandType_field );

    // DayOfWeekSchedule (Member of TariffDefinition)
    if(EvtChargeSettings::GetInstance()->isDayOfWeekChanged( 1
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveMon_RX))
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveTues_RX))
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveWed_RX))
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveThur_RX))
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveFri_RX))
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveSat_RX))
                                                             , QUERY_CAN(SIG_CHRGR(2, ActiveSun_RX))))
    {
        isValueChanged = true;
    }
    evtDayOfWeekSchedule = {  QUERY_CAN(SIG_CHRGR(2, ActiveMon_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(2, ActiveTues_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(2, ActiveWed_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(2, ActiveThur_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(2, ActiveFri_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(2, ActiveSat_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(2, ActiveSun_RX)) == 1 ? true : false
                           };
    LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid] DayOfWeekSchedule Mon[%d] Tue[%d] Wed[%d] Thurs[%d] Fri[%d] Sat[%d] Sun[%d]"
          , evtDayOfWeekSchedule.monday_field, evtDayOfWeekSchedule.tuesday_field,evtDayOfWeekSchedule.wednesday_field
          , evtDayOfWeekSchedule.thursday_field, evtDayOfWeekSchedule.friday_field, evtDayOfWeekSchedule.saturday_field
          , evtDayOfWeekSchedule.sunday_field);

    // compare Tariff Active
    if(EvtChargeSettings::GetInstance()->isTariffActiveChanged(1, QUERY_CAN(SIG_CHRGR(2, Active_RX))))
    {
        isValueChanged = true;
    }

    //TariffDefinition (Member of Tariff)
    evtTariffDefinition = { QUERY_CAN(SIG_CHRGR(2, Active_RX)) == 1 ? true : false
                            , evtDayOfWeekSchedule
                            , evtTariffZoneA
                            , evtTariffZoneB
                            , evtTariffZoneC
                            , evtTariffZoneD
                            , evtTariffZoneE
                            , true
                            , true
                            , true
                            , true
                            , true };
    LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid] isEnabled_field [%d] ZoneA[%d] ZoneB[%d] ZoneC[%d] ZoneD[%d] ZoneE[%d]"
            , evtTariffDefinition.isEnabled_field
            , evtTariffDefinition._bTariffZoneA_is_present
            , evtTariffDefinition._bTariffZoneB_is_present
            , evtTariffDefinition._bTariffZoneC_is_present
            , evtTariffDefinition._bTariffZoneD_is_present
            , evtTariffDefinition._bTariffZoneE_is_present);

    //Tariff
    evtTariff = {true, tariffIdx, evtTariffDefinition, true};

    // Add to Tariff Vector
    EvtChargeSettings::GetInstance()->tariffs_field[tariffIdx] = evtTariff;
    EvtChargeSettings::GetInstance()->bTariffs_is_present = true;

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());

    // // Signal Echo
    // #if defined(PCR041ENABLE)
    // if(!EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    // {
    //     EvtVifManager::GetInstance()->triggerHandler(CAN_FRAME_ECHO, SignalFrame_GWM_PMZ_S_Hybrid_RX);
    // }
    // else
    // {
    //     LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid] IMC_IGNR_STATE. Not Sending ECHO");
    // }
    // #endif

    // send to TSP
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    if(isValueChanged)
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid] value was changed. Sending CHARGE_SETTING_UPDATE");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_S_Hybrid] value was not changed. Not Sending CHARGE_SETTING_UPDATE");
    }
}

inline void rcv_Notification_GWM_PMZ_T_Hybrid(EvCanFrame *pEvCanFrame)
{
    int endTimeTmp;
    uint8_t bandTypeTmp;
    uint32_t tariffIdx = 2;
    int32_t endHourTmp;
    int32_t endMinTmp;
    _Tariff evtTariff;
    _TariffDefinition evtTariffDefinition;
    _DayOfWeekSchedule evtDayOfWeekSchedule;
    _TariffZone evtTariffZoneA, evtTariffZoneB, evtTariffZoneC, evtTariffZoneD, evtTariffZoneE;
    _TimeHM evtTimeHM_A, evtTimeHM_B, evtTimeHM_C, evtTimeHM_D, evtTimeHM_E;

    bool isValueChanged = false;

    // TariffZone A
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(3, ZoneA_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(3, EndTimeA_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(2, ZoneA)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].getTariffEndTimeA()))
    {
        LOG_EV("[Comp Tariff]A Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_A = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].setTariffEndTimeA(endTimeTmp);
    evtTariffZoneA = { evtTimeHM_A, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone B
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(3, ZoneB_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(3, EndTimeB_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(2, ZoneB)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].getTariffEndTimeB()))
    {
        LOG_EV("[Comp Tariff]B Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_B = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].setTariffEndTimeB(endTimeTmp);
    evtTariffZoneB = { evtTimeHM_B, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone C
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(3, ZoneC_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(3, EndTimeC_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(2, ZoneC)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].getTariffEndTimeC()))
    {
        LOG_EV("[Comp Tariff]C Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_C = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].setTariffEndTimeC(endTimeTmp);
    evtTariffZoneC = { evtTimeHM_C, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone D
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(3, ZoneD_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(3, EndTimeD_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(2, ZoneD)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].getTariffEndTimeD()))
    {
        LOG_EV("[Comp Tariff]D Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_D = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[2].setTariffEndTimeD(endTimeTmp);
    evtTariffZoneD = { evtTimeHM_D, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone E
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(3, ZoneE_RX));
    if(bandTypeTmp != EvtChargeSettings::GetInstance()->getBandType(2, ZoneE))
    {
        LOG_EV("[Comp Tariff]E Changed Idx[%d] Band-C[%d]T[%d]", tariffIdx, bandTypeTmp, EvtChargeSettings::GetInstance()->getBandType(2, ZoneE));
        isValueChanged = true;
    }
    evtTariffZoneE = { evtTimeHM_E, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid]EndTimeA h[%d] m[%d] EndTimeB h[%d] m[%d] EndTimeC h[%d] m[%d] EndTimeD h[%d] m[%d]"
            , evtTimeHM_A.hour_field, evtTimeHM_A.minute_field, evtTimeHM_B.hour_field, evtTimeHM_B.minute_field
            , evtTimeHM_C.hour_field, evtTimeHM_C.minute_field, evtTimeHM_D.hour_field, evtTimeHM_D.minute_field );

    LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid] TariffZoneA BandTypeField[%d] B-BandTypeField[%d] C-BandTypeField[%d] D-BandTypeField[%d] E-BandTypeField[%d]"
            , evtTariffZoneA.bandType_field
            , evtTariffZoneB.bandType_field
            , evtTariffZoneC.bandType_field
            , evtTariffZoneD.bandType_field
            , evtTariffZoneE.bandType_field );

    // DayOfWeekSchedule (Member of TariffDefinition)
    if(EvtChargeSettings::GetInstance()->isDayOfWeekChanged( 2
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveMon_RX))
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveTues_RX))
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveWed_RX))
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveThur_RX))
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveFri_RX))
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveSat_RX))
                                                             , QUERY_CAN(SIG_CHRGR(3, ActiveSun_RX))))
    {
        isValueChanged = true;
    }
    evtDayOfWeekSchedule = {  QUERY_CAN(SIG_CHRGR(3, ActiveMon_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(3, ActiveTues_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(3, ActiveWed_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(3, ActiveThur_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(3, ActiveFri_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(3, ActiveSat_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(3, ActiveSun_RX)) == 1 ? true : false
                           };
    LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid] DayOfWeekSchedule Mon[%d] Tue[%d] Wed[%d] Thurs[%d] Fri[%d] Sat[%d] Sun[%d]"
          , evtDayOfWeekSchedule.monday_field, evtDayOfWeekSchedule.tuesday_field,evtDayOfWeekSchedule.wednesday_field
          , evtDayOfWeekSchedule.thursday_field, evtDayOfWeekSchedule.friday_field, evtDayOfWeekSchedule.saturday_field
          , evtDayOfWeekSchedule.sunday_field);

    // compare Tariff Active
    if(EvtChargeSettings::GetInstance()->isTariffActiveChanged(2, QUERY_CAN(SIG_CHRGR(3, Active_RX))))
    {
        isValueChanged = true;
    }

    //TariffDefinition (Member of Tariff)
    evtTariffDefinition = { QUERY_CAN(SIG_CHRGR(3, Active_RX)) == 1 ? true : false
                            , evtDayOfWeekSchedule
                            , evtTariffZoneA
                            , evtTariffZoneB
                            , evtTariffZoneC
                            , evtTariffZoneD
                            , evtTariffZoneE
                            , true
                            , true
                            , true
                            , true
                            , true };
    LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid] isEnabled_field [%d] ZoneA[%d] ZoneB[%d] ZoneC[%d] ZoneD[%d] ZoneE[%d]"
            , evtTariffDefinition.isEnabled_field
            , evtTariffDefinition._bTariffZoneA_is_present
            , evtTariffDefinition._bTariffZoneB_is_present
            , evtTariffDefinition._bTariffZoneC_is_present
            , evtTariffDefinition._bTariffZoneD_is_present
            , evtTariffDefinition._bTariffZoneE_is_present);

    //Tariff
    evtTariff = {true, tariffIdx, evtTariffDefinition, true};

    // Add to Tariff Vector
    EvtChargeSettings::GetInstance()->tariffs_field[tariffIdx] = evtTariff;
    EvtChargeSettings::GetInstance()->bTariffs_is_present = true;

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());

    // // Signal Echo
    // #if defined(PCR041ENABLE)
    // if(!EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    // {
    //     EvtVifManager::GetInstance()->triggerHandler(CAN_FRAME_ECHO, SignalFrame_GWM_PMZ_T_Hybrid_RX);
    // }
    // else
    // {
    //     LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid] IMC_IGNR_STATE. Not Sending ECHO");
    // }
    // #endif

    // send to TSP
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    if(isValueChanged)
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid] value was changed. Sending CHARGE_SETTING_UPDATE");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_T_Hybrid] value was not changed. Not Sending CHARGE_SETTING_UPDATE");
    }
}

inline void rcv_Notification_GWM_PMZ_U_Hybrid(EvCanFrame *pEvCanFrame)
{
    int endTimeTmp;
    uint8_t bandTypeTmp;
    uint32_t tariffIdx = 3;
    int32_t endHourTmp;
    int32_t endMinTmp;
    _Tariff evtTariff;
    _TariffDefinition evtTariffDefinition;
    _DayOfWeekSchedule evtDayOfWeekSchedule;
    _TariffZone evtTariffZoneA, evtTariffZoneB, evtTariffZoneC, evtTariffZoneD, evtTariffZoneE;
    _TimeHM evtTimeHM_A, evtTimeHM_B, evtTimeHM_C, evtTimeHM_D, evtTimeHM_E;

    bool isValueChanged = false;

    // TariffZone A
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(4, ZoneA_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(4, EndTimeA_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(3, ZoneA)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].getTariffEndTimeA()))
    {
        LOG_EV("[Comp Tariff]A Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_A = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].setTariffEndTimeA(endTimeTmp);
    evtTariffZoneA = { evtTimeHM_A, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone B
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(4, ZoneB_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(4, EndTimeB_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(3, ZoneB)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].getTariffEndTimeB()))
    {
        LOG_EV("[Comp Tariff]B Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_B = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].setTariffEndTimeB(endTimeTmp);
    evtTariffZoneB = { evtTimeHM_B, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone C
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(4, ZoneC_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(4, EndTimeC_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(3, ZoneC)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].getTariffEndTimeC()))
    {
        LOG_EV("[Comp Tariff]C Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_C = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].setTariffEndTimeC(endTimeTmp);
    evtTariffZoneC = { evtTimeHM_C, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone D
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(4, ZoneD_RX));
    endTimeTmp = QUERY_CAN(SIG_CHRGR(4, EndTimeD_RX));
    if(EvtChargeSettings::GetInstance()->isTariffZoneChanged( tariffIdx
                                                            , bandTypeTmp
                                                            , endTimeTmp
                                                            , EvtChargeSettings::GetInstance()->getBandType(3, ZoneD)
                                                            , EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].getTariffEndTimeD()))
    {
        LOG_EV("[Comp Tariff]D Changed");
        isValueChanged = true;
    }
    endHourTmp = endTimeTmp / 4;
    endMinTmp  = (endTimeTmp % 4)*15;
    evtTimeHM_D = {  EvChargeApp::GetInstance()->getValueWithLimit(endHourTmp, 0, 23, 0)
                   , EvChargeApp::GetInstance()->getValueWithLimit(endMinTmp, 0, 59, 0) };
    EvtChargeSettings::GetInstance()->evtTariffEndTimes_field[3].setTariffEndTimeD(endTimeTmp);
    evtTariffZoneD = { evtTimeHM_D, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    // TariffZone E
    bandTypeTmp = QUERY_CAN(SIG_CHRGR(4, ZoneE_RX));
    if(bandTypeTmp != EvtChargeSettings::GetInstance()->getBandType(3, ZoneE))
    {
        LOG_EV("[Comp Tariff]E Changed Idx[%d] Band-C[%d]T[%d]", tariffIdx, bandTypeTmp, EvtChargeSettings::GetInstance()->getBandType(3, ZoneE));
        isValueChanged = true;
    }
    evtTariffZoneE = { evtTimeHM_E, EvChargeApp::GetInstance()->getValueWithLimit(bandTypeTmp, 0, 3, 0)};

    LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid]EndTimeA h[%d] m[%d] EndTimeB h[%d] m[%d] EndTimeC h[%d] m[%d] EndTimeD h[%d] m[%d]"
            , evtTimeHM_A.hour_field, evtTimeHM_A.minute_field, evtTimeHM_B.hour_field, evtTimeHM_B.minute_field
            , evtTimeHM_C.hour_field, evtTimeHM_C.minute_field, evtTimeHM_D.hour_field, evtTimeHM_D.minute_field );

    LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid] TariffZoneA BandTypeField[%d] B-BandTypeField[%d] C-BandTypeField[%d] D-BandTypeField[%d] E-BandTypeField[%d]"
            , evtTariffZoneA.bandType_field
            , evtTariffZoneB.bandType_field
            , evtTariffZoneC.bandType_field
            , evtTariffZoneD.bandType_field
            , evtTariffZoneE.bandType_field );

    // DayOfWeekSchedule (Member of TariffDefinition)
    if(EvtChargeSettings::GetInstance()->isDayOfWeekChanged( 3
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveMon_RX))
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveTues_RX))
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveWed_RX))
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveThur_RX))
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveFri_RX))
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveSat_RX))
                                                             , QUERY_CAN(SIG_CHRGR(4, ActiveSun_RX))))
    {
        isValueChanged = true;
    }
    evtDayOfWeekSchedule = {  QUERY_CAN(SIG_CHRGR(4, ActiveMon_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(4, ActiveTues_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(4, ActiveWed_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(4, ActiveThur_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(4, ActiveFri_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(4, ActiveSat_RX)) == 1 ? true : false
                            , QUERY_CAN(SIG_CHRGR(4, ActiveSun_RX)) == 1 ? true : false
                           };
    LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid] DayOfWeekSchedule Mon[%d] Tue[%d] Wed[%d] Thurs[%d] Fri[%d] Sat[%d] Sun[%d]"
          , evtDayOfWeekSchedule.monday_field, evtDayOfWeekSchedule.tuesday_field,evtDayOfWeekSchedule.wednesday_field
          , evtDayOfWeekSchedule.thursday_field, evtDayOfWeekSchedule.friday_field, evtDayOfWeekSchedule.saturday_field
          , evtDayOfWeekSchedule.sunday_field);

    // compare Tariff Active
    if(EvtChargeSettings::GetInstance()->isTariffActiveChanged(3, QUERY_CAN(SIG_CHRGR(4, Active_RX))))
    {
        isValueChanged = true;
    }

    //TariffDefinition (Member of Tariff)
    evtTariffDefinition = { QUERY_CAN(SIG_CHRGR(4, Active_RX)) == 1 ? true : false
                            , evtDayOfWeekSchedule
                            , evtTariffZoneA
                            , evtTariffZoneB
                            , evtTariffZoneC
                            , evtTariffZoneD
                            , evtTariffZoneE
                            , true
                            , true
                            , true
                            , true
                            , true };
    LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid] isEnabled_field [%d] ZoneA[%d] ZoneB[%d] ZoneC[%d] ZoneD[%d] ZoneE[%d]"
            , evtTariffDefinition.isEnabled_field
            , evtTariffDefinition._bTariffZoneA_is_present
            , evtTariffDefinition._bTariffZoneB_is_present
            , evtTariffDefinition._bTariffZoneC_is_present
            , evtTariffDefinition._bTariffZoneD_is_present
            , evtTariffDefinition._bTariffZoneE_is_present);

    //Tariff
    evtTariff = {true, tariffIdx, evtTariffDefinition, true};

    // Add to Tariff Vector
    EvtChargeSettings::GetInstance()->tariffs_field[tariffIdx] = evtTariff;
    EvtChargeSettings::GetInstance()->bTariffs_is_present = true;

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());

    // // Signal Echo
    // #if defined(PCR041ENABLE)
    // if(!EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    // {
    //     EvtVifManager::GetInstance()->triggerHandler(CAN_FRAME_ECHO, SignalFrame_GWM_PMZ_U_Hybrid_RX);
    // }
    // else
    // {
    //     LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid] IMC_IGNR_STATE. Not Sending ECHO");
    // }
    // #endif

    // send to TSP
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;
    if(isValueChanged)
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid] value was changed. Sending CHARGE_SETTING_UPDATE");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_U_Hybrid] value was not changed. Not Sending CHARGE_SETTING_UPDATE");
    }
}

inline void rcv_Notification_GWM_PMZ_AD_Hybrid(EvCanFrame *pEvCanFrame)
{
    uint32_t timerIdx = QUERY_CAN(Signal_PreDepTimeIndex_RX);
    _TimeDate _timeDate;
    _DayOfWeekSchedule _dayOfWeekSchedule;
    _TimerTarget _timerTarget;
    _TimeHM _timeHM;
    _DepartureTimerDefinition _departureTimerDefinition;
    _DepartureTimer _departureTimer;
    uint32_t _preDepTimeActive;
    uint8_t _timeType;
    bool isValueChanged = false;

    // timerIdx's valid range is 0~49
    if(timerIdx > 49 || timerIdx < 0)
    {
        LOGE("[rcv_Notification_GWM_PMZ_AD_Hybrid] Invalid Timer Index Value[%d] Not added to DepartureTimer Vector", timerIdx);
    }
    else
    {
    // preDepTimeActive - 0(DepartureTimerType_off), 1(chargeOnly), 2(preconditionOnly), 3(bothChargeAndPrecondition)
    _preDepTimeActive = EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeActive_RX), 0, 3, 0);
    if(EvtChargeSettings::GetInstance()->isDeptActiveChanged(timerIdx, _preDepTimeActive))
    {
        isValueChanged = true;
    }
    // timeType - 0(Single Event), 1(Recurring Event)
    _timeType = EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeType_RX), 0, 1, 0);
    if(EvtChargeSettings::GetInstance()->isDeptTimeTypeChanged(timerIdx,_timeType))
    {
        isValueChanged = true;
    }

    // Store CAN Signal Raw Value to compare next
    //EvtChargeSettings::GetInstance()->setCanDeptDateField(QUERY_CAN(Signal_PreDepTimeDate_RX));
    //EvtChargeSettings::GetInstance()->setCanDeptMnthField(QUERY_CAN(Signal_PreDepTimeMnth_RX));
    //EvtChargeSettings::GetInstance()->setCanDeptYearField(QUERY_CAN(Signal_PreDepTimeYear_RX));

    if(EvtChargeSettings::GetInstance()->isDeptTimeDateChanged(timerIdx
                                                             , QUERY_CAN(Signal_PreDepTimeDate_RX)
                                                             , QUERY_CAN(Signal_PreDepTimeMnth_RX)
                                                             , QUERY_CAN(Signal_PreDepTimeYear_RX)))
    {
        isValueChanged = true;
    }
    // TimeDate (Member of TimerTarget) - Applied getValueWithLimit() to avoid crash when EV encodes NGTPMessage
    /*
    _timeDate = { EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeDate_RX)+1, 1, 31 , 1)
                , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeMnth_RX)+1, 1, 12 , 1)
                , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeYear_RX)+2000, 0, 2255, 2000)};
    */
    _timeDate.can_Day = QUERY_CAN(Signal_PreDepTimeDate_RX);
    _timeDate.can_Month = QUERY_CAN(Signal_PreDepTimeMnth_RX);
    _timeDate.can_Year = QUERY_CAN(Signal_PreDepTimeYear_RX);

    _timeDate.day = EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeDate_RX)+1, 1, 31 , 1);
    _timeDate.month = EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeMnth_RX)+1, 1, 12 , 1);
    _timeDate.year = EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeYear_RX)+2000, 0, 2255, 2000);

    if(EvtChargeSettings::GetInstance()->isDeptDayOfWeekChanged(timerIdx
                                                               , QUERY_CAN(Signal_PreDepTimeActMon_RX)
                                                               , QUERY_CAN(Signal_PreDepTimeActTue_RX)
                                                               , QUERY_CAN(Signal_PreDepTimeActWed_RX)
                                                               , QUERY_CAN(Signal_PreDepTimeActThu_RX)
                                                               , QUERY_CAN(Signal_PreDepTimeActFri_RX)
                                                               , QUERY_CAN(Signal_PreDepTimeActSat_RX)
                                                               , QUERY_CAN(Signal_PreDepTimeActSun_RX)))

    {
        isValueChanged = true;
    }
    // DayOfWeekSchedule (Member of TimerTarget)
    _dayOfWeekSchedule = { QUERY_CAN(Signal_PreDepTimeActMon_RX)
                         , QUERY_CAN(Signal_PreDepTimeActTue_RX)
                         , QUERY_CAN(Signal_PreDepTimeActWed_RX)
                         , QUERY_CAN(Signal_PreDepTimeActThu_RX)
                         , QUERY_CAN(Signal_PreDepTimeActFri_RX)
                         , QUERY_CAN(Signal_PreDepTimeActSat_RX)
                         , QUERY_CAN(Signal_PreDepTimeActSun_RX) };
    LOG_EV("[rcv_Notification_GWM_PMZ_AD_Hybrid] TimeType[%d] TimeDate d[%d] m[%d] y[%d] DayOfWeekSchedule mon[%d]tue[%d]wed[%d]thurs[%d]fri[%d]sat[%d]sun[%d]"
         , _timeType
         , _timeDate.day, _timeDate.month, _timeDate.year
         , _dayOfWeekSchedule.monday_field
         , _dayOfWeekSchedule.tuesday_field
         , _dayOfWeekSchedule.wednesday_field
         , _dayOfWeekSchedule.thursday_field
         , _dayOfWeekSchedule.friday_field
         , _dayOfWeekSchedule.saturday_field
         , _dayOfWeekSchedule.sunday_field);
    // TimerTarget (Member of DepartureTimerDefinition)
    _timerTarget = {_timeType, _timeDate, _dayOfWeekSchedule};

    if(EvtChargeSettings::GetInstance()->isDeptTimeHMChanged(timerIdx
                                                             , QUERY_CAN(Signal_PreDepTimeMins_RX)
                                                             , QUERY_CAN(Signal_PreDepTimeHour_RX)))
    {
        isValueChanged = true;
    }
    // TimeHM (Member of DepartureTimerDefinition) - Applied getValueWithLimit() to avoid crash when EV encodes NGTPMessage
    _timeHM = {EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeHour_RX), 0, 23 , 0)
             , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_PreDepTimeMins_RX), 0, 59 , 0)};

    _TimeHM tmpCANValues = {QUERY_CAN(Signal_PreDepTimeHour_RX), QUERY_CAN(Signal_PreDepTimeMins_RX)};
    EvtChargeSettings::GetInstance()->canDeptTimeHM_field[timerIdx] = tmpCANValues;

    // DepartureTimerDefinition (Member of DepartureTimer)
    _departureTimerDefinition = { _preDepTimeActive, _timeHM, _timerTarget};

    // DepartureTimer
    bool _bTarDefIsTrue = (_preDepTimeActive == 0) ? false : true ;
    _departureTimer = {timerIdx, _departureTimerDefinition, _bTarDefIsTrue, true};

    LOG_EV("[rcv_Notification_GWM_PMZ_AD_Hybrid] timerIndex[%d] timerType[%d] TimeHM h[%d] m[%d]"
            , timerIdx
            , _departureTimerDefinition.timerType_field
            , _timeHM.hour_field
            , _timeHM.minute_field);
    // Add to DepartureTimer Vector
    EvtChargeSettings::GetInstance()->departureTimer_field[timerIdx] = _departureTimer;
    EvtChargeSettings::GetInstance()->bDepartureTimers_is_present = true;

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());

    // // Signal Echo
    // #if defined(PCR041ENABLE)
    // if(!EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    // {
    //     EvtVifManager::GetInstance()->triggerHandler(CAN_FRAME_ECHO, SignalFrame_GWM_PMZ_AD_Hybrid_RX);
    // }
    // else
    // {
    //     LOG_EV("[rcv_Notification_GWM_PMZ_AD_Hybrid] IMC_IGNR_STATE. Not Sending ECHO");
    // }
    // #endif

    // send to TSP
    if(isValueChanged)
    {
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOG_EV("[rcv_Notification_GWM_PMZ_AD_Hybrid] Not Sending CS Update");
    }
    EvtElectricVehicleInformation::GetInstance()->bChargeSetting_field_is_updated = true;

    }
}

inline void rcv_Notification_PCM_PMZ_Q_Hybrid(EvCanFrame *pEvCanFrame)
{
    // minutesToBulkCharged
    EvtElectricVehicleInformation::GetInstance()->setMinutesToBulkChargedFromCAN(
                                                   QUERY_CAN(Signal_TimeToBulkSoCHoursDisp_RX)
                                                 , QUERY_CAN(Signal_TimeToBulkSoCMinsDisp_RX));
    // minutesToFullyCharged
    EvtElectricVehicleInformation::GetInstance()->setMinutesToFullyChargedFromCAN(
                                                   QUERY_CAN(Signal_TimeToFullSoCHoursDisp_RX)
                                                 , QUERY_CAN(Signal_TimeToFullSoCMinsDisp_RX));

    LOGV("[rcv_Notification_PCM_PMZ_Q_Hybrid] minutesToBulkCharged[%d] minutesToFullyCharged[%d]"
        , EvtElectricVehicleInformation::GetInstance()->getMinutesToBulkChargedField()
        , EvtElectricVehicleInformation::GetInstance()->getMinutesToFullyChargedField());
    EvtElectricVehicleInformation::GetInstance()->bChargingInformation_is_present = true;
    //EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE, E_OK);
}

//SRD_ReqEVC0201_v2
inline void rcv_Notification_PCM_PMZ_S_Hybrid(EvCanFrame *pEvCanFrame)
{
    bool bChanged = EvtNextDepartureTimer::GetInstance()->isNextDepartureTimerChanged(QUERY_CAN(Signal_TimedChrgStartYearDisp_RX)
                                                                                , QUERY_CAN(Signal_TimedChrgStartMnthDisp_RX)
                                                                                , QUERY_CAN(Signal_TimedChrgStartDateDisp_RX)
                                                                                , QUERY_CAN(Signal_TimedChrgStartHourDisp_RX)
                                                                                , QUERY_CAN(Signal_TimedChrgStartMinsDisp_RX));
    // Set Queried CAN Signals' values
    EvtNextDepartureTimer::GetInstance()->setAllValues(  EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_TimedChrgStartYearDisp_RX)+2000, 0, 2255, 2000)
                                                       , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_TimedChrgStartMnthDisp_RX)+1, 1, 12 , 1)
                                                       , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_TimedChrgStartDateDisp_RX)+1, 1, 31 , 1)
                                                       , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_TimedChrgStartHourDisp_RX), 0, 23 , 0)
                                                       , EvChargeApp::GetInstance()->getValueWithLimit(QUERY_CAN(Signal_TimedChrgStartMinsDisp_RX), 0, 59 , 0));

    // save OffSet Free CAN Signal values to compare next CAN Signal.
    EvtNextDepartureTimer::GetInstance()->setCanDateYearField(QUERY_CAN(Signal_TimedChrgStartYearDisp_RX));
    EvtNextDepartureTimer::GetInstance()->setCanDateMonthField(QUERY_CAN(Signal_TimedChrgStartMnthDisp_RX));
    EvtNextDepartureTimer::GetInstance()->setCanDateDayField(QUERY_CAN(Signal_TimedChrgStartDateDisp_RX));
    EvtNextDepartureTimer::GetInstance()->setCanDateHourField(QUERY_CAN(Signal_TimedChrgStartHourDisp_RX));
    EvtNextDepartureTimer::GetInstance()->setCanDateMinField(QUERY_CAN(Signal_TimedChrgStartMinsDisp_RX));

    LOGV("[rcv_Notification_PCM_PMZ_S_Hybrid] NextDepartureTimer y[%d]m[%d]d[%d]h[%d]m[%d]"
          , EvtNextDepartureTimer::GetInstance()->getDateYearField()
          , EvtNextDepartureTimer::GetInstance()->getDateMonthField()
          , EvtNextDepartureTimer::GetInstance()->getDateDayField()
          , EvtNextDepartureTimer::GetInstance()->getDateHourField()
          , EvtNextDepartureTimer::GetInstance()->getDateMinuteField());

    // send to TSP
    EvtElectricVehicleInformation::GetInstance()->bNextDepartureTimer_is_present = true;

    if(bChanged)
    {
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOGV("[rcv_Notification_PCM_PMZ_S_Hybrid] Not Sending CSUpdate");
    }
}
#undef QUERY_CAN
#undef SIG_CHRG
#undef SET_TIME

inline uint16_t getAlertDataForEVC(int32_t changeSigId)
{
    int _ChargingStatusDisp     = EvtVifManager::GetInstance()->queryCanData(Signal_ChargingStatusDisp_RX);
    int _WlChargingStatusDisp   = EvtVifManager::GetInstance()->queryCanData(Signal_WlChargingStatusDisp_RX);
    int _ChargePwrInterruptDisp = EvtVifManager::GetInstance()->queryCanData(Signal_ChargePwrInterruptDisp_RX);
    int _ChargNotReadyByDepDisp = EvtVifManager::GetInstance()->queryCanData(Signal_ChrgNotReadyByDepDisp_RX);
    int _ChargePlugDiscAlarmReq = EvtVifManager::GetInstance()->queryCanData(Signal_ChargePlugDiscAlarmReq_RX);
    int _BulkSoCTargetDisp      = EvtVifManager::GetInstance()->queryCanData(Signal_BulkSoCTargetDisp_RX);
    // Applied CAN Signal scale factor 0.1
    _BulkSoCTargetDisp          = _BulkSoCTargetDisp > 1000 ? -1 : _BulkSoCTargetDisp/10 ;
    int _BulkSoCSingleUseDisp   = EvtVifManager::GetInstance()->queryCanData(Signal_BulkSoCSingleUseDisp_RX);
    _BulkSoCSingleUseDisp       = _BulkSoCSingleUseDisp > 1000 ? -1 : _BulkSoCSingleUseDisp/10 ;

    // set ChangeFlags
    bool bChargingStatusDisp     = changeSigId == Signal_ChargingStatusDisp_RX ? true : false;
    bool bWlChargingStatusDisp   = changeSigId == Signal_WlChargingStatusDisp_RX ? true : false;
    bool bChargePwrInterruptDisp = changeSigId == Signal_ChargePwrInterruptDisp_RX ? true : false;
    bool bChargNotReadyByDepDisp = changeSigId == Signal_ChrgNotReadyByDepDisp_RX ? true : false;
    bool bChargePlugDiscAlarmReq = changeSigId == Signal_ChargePlugDiscAlarmReq_RX ? true : false;
    bool bBulkSoCTargetDisp      = changeSigId == Signal_BulkSoCTargetDisp_RX ? true : false;
    bool bBulkSoCSingleUseDisp   = changeSigId == Signal_BulkSoCSingleUseDisp_RX ? true : false;


    LOG_EV("Calculate CHARGING_STATUS_UPDATE_ALERT condition -> DetectedSignal[%s] _ChargingStatusDisp[%d] _WlChargingStatusDisp[%d] _ChargePwrInterruptDisp[%d] _ChargNotReadyByDepDisp[%d] _ChargePlugDiscAlarmReq[%d] _BulkSoCTargetDisp[%d] _BulkSoCSingleUseDisp[%d]"
        , EvtVifManager::GetInstance()->getCanName(changeSigId)
        , _ChargingStatusDisp
        , _WlChargingStatusDisp
        , _ChargePwrInterruptDisp
        , _ChargNotReadyByDepDisp
        , _ChargePlugDiscAlarmReq
        , _BulkSoCTargetDisp
        , _BulkSoCSingleUseDisp);

    if(bChargingStatusDisp)
    {
        // evChargeComplete - Wired Charging
        if(_ChargingStatusDisp == 1)
        {
            LOG_EV("[getAlertDataForEVC] evChargeComplete_chosen");
            return svc::AlertData::evChargeComplete_chosen;
        }

        // evBulkChargeComplete - Wired Charging
        if(_ChargingStatusDisp == 2)
        {
            LOG_EV("[getAlertDataForEVC] evBulkChargeComplete_chosen");
            return svc::AlertData::evBulkChargeComplete_chosen;
        }
    }

    if(bChargingStatusDisp || bChargePwrInterruptDisp)
    {
        // evChargeFault
        if(_ChargingStatusDisp == 5 && _ChargePwrInterruptDisp == 0)
        {
            LOG_EV("[getAlertDataForEVC] evChargeFault_chosen");
            return svc::AlertData::evChargeFault_chosen;
        }

        // evChargeIncomplete
        if(_ChargingStatusDisp == 5 && _ChargePwrInterruptDisp == 1)
        {
            LOG_EV("[getAlertDataForEVC] evChargeIncomplete_chosen");
            return svc::AlertData::evChargeIncomplete_chosen;
        }
    }

    // evChargeInterrupted (Wired Charging)
    if( bChargePlugDiscAlarmReq && (_ChargePlugDiscAlarmReq == 1) )
    {
        LOG_EV("[getAlertDataForEVC] evChargeInterrupted_chosen");
        return svc::AlertData::evChargeInterrupted_chosen;
    }

    if(bWlChargingStatusDisp)
    {
        // evChargeInitiated (Wireless Charging)
        if(_WlChargingStatusDisp == 6)
        {
            LOG_EV("[getAlertDataForEVC] evChargeInitiated_chosen");
            return svc::AlertData::evChargeInitiated_chosen;
        }

        // evChargeComplete (Wireless Charging)
        if(_WlChargingStatusDisp == 1)
        {
            LOG_EV("[getAlertDataForEVC] evChargeComplete_chosen");
            return svc::AlertData::evChargeComplete_chosen;
        }

        // evBulkChargeComplete (Wireless Charging)
        if(_WlChargingStatusDisp == 2)
        {
            LOG_EV("[getAlertDataForEVC] evBulkChargeComplete_chosen");
            return svc::AlertData::evBulkChargeComplete_chosen;
        }
    }

    if(bWlChargingStatusDisp || bChargePwrInterruptDisp)
    {
        // evChargeFault (Wireless Charging)
        if(_WlChargingStatusDisp == 5 && _ChargePwrInterruptDisp == 0)
        {
            LOG_EV("[getAlertDataForEVC] evChargeFault_chosen");
            return svc::AlertData::evChargeFault_chosen;
        }

        // evChargeIncomplete (Wireless Charging)
        if(_WlChargingStatusDisp == 5 && _ChargePwrInterruptDisp == 1)
        {
            LOG_EV("[getAlertDataForEVC] evChargeIncomplete_chosen");
            return svc::AlertData::evChargeIncomplete_chosen;
        }
    }

    if(bChargNotReadyByDepDisp || bBulkSoCTargetDisp || bBulkSoCSingleUseDisp)
    {
        // evCannotReachFullCharge (Both Wired and Wireless)
        if(_ChargNotReadyByDepDisp == 1 && (_BulkSoCTargetDisp == 100 || _BulkSoCSingleUseDisp == 100))
        {
            LOG_EV("[getAlertDataForEVC] evCannotReachFullCharge_chosen");
            return svc::AlertData::evCannotReachFullCharge_chosen;
        }

        // evCannotReachMaxSOC (Both Wired and Wireless)
        if(_ChargNotReadyByDepDisp == 1 && (_BulkSoCTargetDisp < 100 || _BulkSoCSingleUseDisp < 100))
        {
            LOG_EV("[getAlertDataForEVC] evCannotReachMaxSOC_chosen");
            return svc::AlertData::evCannotReachMaxSOC_chosen;
        }
    }
    return svc::AlertData::unused_chosen;
}