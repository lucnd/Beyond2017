#define LOG_TAG "[EVT]EvChargeAppHandler"
#include "Log.h"

#include "EvChargeAppHandler.h"
#include "EvServiceManager.h"
#include "EvtUtil.h"
#include <stdio.h>
#include <unistd.h>

using namespace std;

// Handler for Asyn Signal Call aback pettern.
void EvChargeAppHandler::handleMessage(const sp<sl::Message>& msg)
{
    unsigned long txDelayMs = 150;
    switch(msg->what)
    {
        case NGTP_chargeNowSetting_is_present:
        {
            LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- ChargeNow", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
            mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
            mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

            sendChargeNowSettingToCAN(static_cast<svc::ChargeSettings*>(msg->obj));

            usleep(txDelayMs*1000);
            LOGV("[sendChargeNowSettingToCAN] Delayed %d milisec", txDelayMs);
            break;
        }
        case NGTP_maxStateOfCharge_is_present:
        {
            LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- MaxSOC", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
            mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
            mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

            sendMaxStateOfChargeToCAN(static_cast<svc::ChargeSettings*>(msg->obj));

            usleep(txDelayMs*1000);
            LOGV("[sendMaxStateOfChargeToCAN] Delayed %d milisec", txDelayMs);
            break;
        }
        case NGTP_chargingModeChoice_is_present:
        {
            LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- ChargeModeChoice", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
            mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
            mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

            sendChargingModeChoiceToCAN(static_cast<svc::ChargeSettings*>(msg->obj));

            usleep(txDelayMs*1000);
            LOGV("[sendChargingModeChoiceToCAN] Delayed %d milisec", txDelayMs);
            break;
        }
        case NGTP_departureTimers_is_present:
        {
            deleteDepartureTimerToCAN(static_cast<svc::ChargeSettings*>(msg->obj));
            sendDepartureTimerToCAN(static_cast<svc::ChargeSettings*>(msg->obj));
            break;
        }
        case NGTP_tariffs_is_present:
        {
            sendTariffToCAN(static_cast<svc::ChargeSettings*>(msg->obj));
            break;
        }
        case NGTP_CHARGESETTING_REQ:
        {
            //TODO: Implement
            //decodeMsg_ChargeSettings(static_cast<svc::ChargeSettings*>(msg->obj), msg->arg1);
            break;
        }
        // Implement for 3.5.3 Initial Setup -- refer Figure EVC5
        case Start_FT_Status:
        {
            trigger_Start_FT_Status();
            break;
        }
        case FirstTime_EVC_IMC_WKUP_TIME:
        {
            trigger_FirstTime_EVC_IMC_WKUP_TIME();
            break;
        }
        case FirstTime_EVC_IGNR_TIME:
        {
            trigger_FirstTime_EVC_IGNR_TIME();
            break;
        }
        case FirstTime_EVC_IMC_FT_TIME:
        {
            trigger_FirstTime_EVC_IMC_FT_TIME();
            break;
        }
        default:
        {
            break;
        }
    }
}

//////////////////3.5.3 Initial Setup /////////////////////////////////////////////////////////////////////////////////////////////////
void EvChargeAppHandler::trigger_Start_FT_Status()
{
    LOGI("[Initial Setup]Delay for FirstTime_EVC_IMC_WKUP_TIME [%d]s",mEvChargeApp.getCfgData("EVC_IMC_WKUP_TIME"));
    int64_t delayms = mEvChargeApp.getCfgData("EVC_IMC_WKUP_TIME")*1000;
    this->sendMessageDelayed(/*Message*/this->obtainMessage(/*What*/FirstTime_EVC_IMC_WKUP_TIME), /*DelayTime*/delayms);
}
void EvChargeAppHandler::trigger_FirstTime_EVC_IMC_WKUP_TIME() //After Box of EVC_IMC_WKUP_TIME in Figure EVC5.
{
    mEvChargeApp.sendCanSignal(Signal_FirstTimeChargeSetReq_TX, 1);
    LOGI("[Initial Setup]Delay for FirstTime_EVC_IMC_IGNR_TIME [%d]ms",mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
    mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);
    this->sendMessageDelayed(/*Message*/this->obtainMessage(/*What*/FirstTime_EVC_IGNR_TIME), /*DelayTime*/mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
}
void EvChargeAppHandler::trigger_FirstTime_EVC_IGNR_TIME() //After Box of EVC_IMC_IGNR_TIME in Figure EVC5.
{
    mEvChargeApp.setEVCState(EvChargeApp::EVC_FIRST_TIME_STATE);
    int32_t _FT_status = EvtVifManager::GetInstance()->queryCanData(Signal_FirstTimeChargeSetStat_RX);

    // TCU should FirstTimeChargeSetReq as false regardless of FirstTimeChargeSetStat [Harmony-2112]
    mEvChargeApp.sendCanSignal(Signal_FirstTimeChargeSetReq_TX, 0);
    if(_FT_status == 0)
    {
        LOGE("[Initial Setup] FirstTimeChargeSetStat = false. End initial setup.");
        mEvChargeApp.initialSetupEnd();
    }
    else
    {
        // SRD_ReqEVC0102_v1
        LOGV("[Initial Setup] FirstTimeChargeSetStat = true. Send FirstTimeChargeSetReq = false");
        int64_t delayms = mEvChargeApp.getCfgData("EVC_IMC_TF2_TIME")*1000;
        LOGI("[Initial Setup]Delay for FirstTime_EVC_IMC_FT_TIME (EVC_IMC_TF2_TIME) [%d]ms", mEvChargeApp.getCfgData("EVC_IMC_TF2_TIME"));
        this->sendMessageDelayed(/*Message*/this->obtainMessage(/*What*/FirstTime_EVC_IMC_FT_TIME), /*DelayTime*/delayms);
    }
}
void EvChargeAppHandler::trigger_FirstTime_EVC_IMC_FT_TIME() //After Box of EVC_IMC_FT_TIME in Figure EVC5.
{
    mEvChargeApp.initialSetupEnd();
    // SRD_ReqEVC0104_v1
    int32_t _FT_status = EvtVifManager::GetInstance()->queryCanData(Signal_FirstTimeChargeSetStat_RX);
    if(_FT_status == 0)
    {
        LOGV("[Initial Setup] FirstTimeChargeSetStat = false. TCU will send ChargeSettingUpdate.");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
    }
    else
    {
        LOGE("[Initial Setup] FirstTimeChargeSetStat = true. TCU will not send ChargeSettingUpdate.");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void decodeMsg_ChargeSettings(svc::ChargeSettings* msg, uint32_t _msgCreationTime)
{

}

void EvChargeAppHandler::sendChargeNowSettingToCAN(svc::ChargeSettings* data)
{
    svc::ChargeSettings::chargeNowSetting _chargeNowSetting = *(data->get_chargeNowSetting());
    switch(_chargeNowSetting)
    {
        case svc::ChargeNowSetting_default:
        {
            mEvChargeApp.sendCanSignal(Signal_ChargeNowReqExtUpdte2_TX, 0);
            LOGV("Send CAN [Signal_ChargeNowReqExtUpdte2_TX][0, Default]");
            mEvChargeApp.arrayNgtpResult[NGTP_chargeNowSetting_is_present] = EMPTY;
            break;
        }
        case svc::force_on:
        {
            mEvChargeApp.sendCanSignal(Signal_ChargeNowReqExtUpdte2_TX, 1);
            //mEvChargeApp.sendCanSignal(Signal_ChargeNowReqExtUpdte_TX, 1); //TODO : Workround Code --> CAN DB is not updated yet.
            LOGV("Send CAN [Signal_ChargeNowReqExtUpdte2_TX][1, force_on]");
            break;
        }
        case svc::force_off:
        {
            mEvChargeApp.sendCanSignal(Signal_ChargeNowReqExtUpdte2_TX, 2);
            //mEvChargeApp.sendCanSignal(Signal_ChargeNowReqExtUpdte_TX, 0); //TODO : Workround Code --> CAN DB is not updated yet.
            LOGV("Send CAN [Signal_ChargeNowReqExtUpdte2_TX][2, force_off]");
            break;
        }
        default:
        {
            LOGE("[Send CAN Error] Unknown chargeNowSetting_field[%d] in NGTP", _chargeNowSetting);
            mEvChargeApp.arrayNgtpResult[NGTP_chargeNowSetting_is_present] = svc::ErrorCode::genericError_chosen;
            break;
        }
    }
}
void EvChargeAppHandler::sendMaxStateOfChargeToCAN(svc::ChargeSettings* data)
{
    if(data->get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc())
    {
        uint32_t _permanentMaxSoc = *(data->get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc());
        _permanentMaxSoc = _permanentMaxSoc *10;
        uint8_t tmpPermdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
        tmpPermdata[1] = _permanentMaxSoc & 0xFF;
        tmpPermdata[0] = (_permanentMaxSoc & 0xFF00) >> 8;
        mEvChargeApp.sendCanArraySignal(Signal_BulkSoCSetExtUpdate_TX, tmpPermdata);
        LOGV("Send CAN [permanentMaxSoc:BulkSoCSetExtUpdate][%d]", _permanentMaxSoc);
    }
    else if(data->get_maxStateOfCharge()->get_permanentMaxSoc().get_noChange())
    {
        LOGV("Send CAN [permanentMaxSoc:BulkSoCSetExtUpdate] noChange - not sending");
    }
    else if(data->get_maxStateOfCharge()->get_permanentMaxSoc().get_clear())
    {
        LOGV("Send CAN [permanentMaxSoc:BulkSoCSetExtUpdate] clear 0");
        uint32_t _permanentMaxSoc = 0;
        uint8_t tmpPermdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
        tmpPermdata[1] = _permanentMaxSoc & 0xFF;
        tmpPermdata[0] = (_permanentMaxSoc & 0xFF00) >> 8;
        mEvChargeApp.sendCanArraySignal(Signal_BulkSoCSetExtUpdate_TX, tmpPermdata);
    }
    else
    {
        LOGE("Send CAN [permanentMaxSoc:BulkSoCSetExtUpdate] invalid");
    }

    if(data->get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc())
    {
        uint32_t _oneOffMaxSoc = *(data->get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc());
        _oneOffMaxSoc = _oneOffMaxSoc *10;
        uint8_t tmpOneOffdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
        tmpOneOffdata[1] = _oneOffMaxSoc & 0xFF;
        tmpOneOffdata[0] = (_oneOffMaxSoc & 0xFF00) >> 8;
        mEvChargeApp.sendCanArraySignal(Signal_BulkSoCSingleUseExtUpd_TX, tmpOneOffdata);

        LOGV("Send CAN [oneOffMaxSoc:BulkSoCSingleUseExtUpd][%d]", _oneOffMaxSoc);
    }
    else if(data->get_maxStateOfCharge()->get_oneOffMaxSoc().get_noChange())
    {
        LOGV("Send CAN [oneOffMaxSoc:BulkSoCSingleUseExtUpd] noChange - not sending");
    }
    else if(data->get_maxStateOfCharge()->get_oneOffMaxSoc().get_clear())
    {
        LOGV("Send CAN [oneOffMaxSoc:BulkSoCSingleUseExtUpd] clear 0");
        uint32_t _oneOffMaxSoc = 0;
        uint8_t tmpOneOffdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
        tmpOneOffdata[1] = _oneOffMaxSoc & 0xFF;
        tmpOneOffdata[0] = (_oneOffMaxSoc & 0xFF00) >> 8;
        mEvChargeApp.sendCanArraySignal(Signal_BulkSoCSingleUseExtUpd_TX, tmpOneOffdata);
    }
    else
    {
        LOGE("Send CAN [oneOffMaxSoc:BulkSoCSingleUseExtUpd] invalid");
    }
}

void EvChargeAppHandler::sendChargingModeChoiceToCAN(svc::ChargeSettings* data)
{
    svc::ChargeSettings::chargingModeChoice _chargingModeChoice = *(data->get_chargingModeChoice());
    switch(_chargingModeChoice)
    {
        case svc::timedChargingNotActive:
        {
            mEvChargeApp.sendCanSignal(Signal_ChrgModeChoiceExtUpdte_TX, 0);
            LOGV("Send CAN [Signal_ChrgModeChoiceExtUpdte_TX][0, NotActive]");
            break;
        }
        case svc::timedChargingActive:
        {
            mEvChargeApp.sendCanSignal(Signal_ChrgModeChoiceExtUpdte_TX, 1);
            LOGV("Send CAN [Signal_ChrgModeChoiceExtUpdte_TX][1, Active]");
            break;
        }
        default:
        {
            LOGE("[ERROR][ErrorCode::genericError] Unknown chargingModeChoice_field[%d] from NGTP", _chargingModeChoice);
            mEvChargeApp.arrayNgtpResult[NGTP_chargingModeChoice_is_present] = svc::ErrorCode::genericError_chosen;
            break;
        }
    }
}

#define PUSH_SINGLE_CAN(A, B, C) EvtVifManager::GetInstance()->pushSingleData(A, B, C)

//SRD_ReqEVC0664_v1
void EvChargeAppHandler::deleteDepartureTimerToCAN(svc::ChargeSettings* data)
{
    svc::DepartureTimers::timers _DepartureTimer_list = data->get_departureTimers()->get_timers();
    void* pOssIndex = _DepartureTimer_list.first();
    uint32_t timerIdx;
    unsigned long txDelayMs = 150;

    do{
        //VIF CAN Container.
        sp<vifCANContainer> indata = new vifCANContainer();

        //OssIndex Null Check
        if(pOssIndex == NULL)
        {
            LOGE("[deleteDepartureTimerToCAN] pOssIndex is null");
            continue;
        }

        svc::DepartureTimer* pDepartureTimer = _DepartureTimer_list.at(pOssIndex);
        timerIdx = pDepartureTimer->get_timerIndex();

        // Delete Request's timerIndex Range : 0~49. (50 is excluded- NEW ADD Timer)
        if( (!pDepartureTimer->departureTimerDefinition_is_present())&& (timerIdx < 50) && (timerIdx >= 0))
        {
            LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- DepartureTimer[Delete]", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
            mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
            mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

            //TimerIndex
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeIndexExtUpd_TX, timerIdx);

            //DepartureTimer Type
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActiveExtUpd_TX, 0);

            //departureTime
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeHourExtUpd_TX, 24);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeMinsExtUpd_TX, 60);

            //TimeDate
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeDateExtUpd_TX, 31);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeMnthExtUpd_TX, 12);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeYearExtUpd_TX, 255);

            // TimeType
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeTypeExtUpd_TX, 1);

            //DayOfWeekSchedules
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActMonExtUpd_TX, 0);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActTueExtUpd_TX, 0);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActWedExtUpd_TX, 0);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActThuExtUpd_TX, 0);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActFriExtUpd_TX, 0);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActSatExtUpd_TX, 0);
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActSunExtUpd_TX, 0);

            EvtVifManager::GetInstance()->sendCanMultiSignal(indata->NumofSignals, indata);
            LOGV("[deleteDepartureTimerToCAN] num of Signals : %d", indata->NumofSignals);

            usleep(txDelayMs*1000);
            LOGV("[deleteDepartureTimerToCAN] Delayed %d milisec", txDelayMs);
        }

        pOssIndex = _DepartureTimer_list.next(pOssIndex);
    }while(pOssIndex != NULL);
}

void EvChargeAppHandler::sendDepartureTimerToCAN(svc::ChargeSettings* data)
{
    svc::DepartureTimers::timers _DepartureTimer_list = data->get_departureTimers()->get_timers();
    void* pOssIndex = _DepartureTimer_list.first();
    uint32_t tmpValue;
    uint32_t timerIdx;
    unsigned long txDelayMs = 150;

    do{
        //VIF CAN Container.
        sp<vifCANContainer> indata = new vifCANContainer();

        if(pOssIndex == NULL)
        {
            LOGE("[sendDepartureTimerToCAN] pOssIndex is null");
            continue;
        }
        svc::DepartureTimer* pDepartureTimer = _DepartureTimer_list.at(pOssIndex);
        timerIdx = pDepartureTimer->get_timerIndex();

        // timerIdx's valid range is 0~50 (50 is New DepartureTimer)
        if(pDepartureTimer->departureTimerDefinition_is_present()&& timerIdx < 51 && timerIdx >= 0)
        {
            LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- DepartureTimer", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
            mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
            mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

            //TimerIndex
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeIndexExtUpd_TX, timerIdx);
            //DepartureTimer Type
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActiveExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerType());
            //departureTime
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeHourExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_hour());
            PUSH_SINGLE_CAN(indata, Signal_PreDepTimeMinsExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_minute());

            //timerTarget - SingleDay.
            if(pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay())
            {
              // Apply CAN Signal offset value when sending to IMC (JLRTCU-2501)
              tmpValue = pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_day();
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeDateExtUpd_TX, tmpValue-1);

              // Apply CAN Signal offset value when sending to IMC (JLRTCU-2501)
              tmpValue = pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_month();
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeMnthExtUpd_TX, tmpValue -1);

              // Apply CAN Signal offset value when sending to IMC (JLRTCU-2501)
              tmpValue = pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_year();
              tmpValue = (tmpValue >= 2000) ? tmpValue - 2000 : tmpValue;
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeYearExtUpd_TX, tmpValue);

              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeTypeExtUpd_TX, 0);

              // For un-recieved message send CAN Signal default value (JLRTCU-2523)
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActMonExtUpd_TX,  0);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActTueExtUpd_TX,  0);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActWedExtUpd_TX,  0);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActThuExtUpd_TX,  0);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActFriExtUpd_TX,  0);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActSatExtUpd_TX,  0);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActSunExtUpd_TX,  0);

              // Print multiple values into a line
              LOGV("[sendDepartureTimerToCAN] timerIdx[%d] TimerType[%d] DepartureTime h[%d]m[%d] SingleDay d[%d]m[%d]y[%d]"
                , pDepartureTimer->get_timerIndex()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerType()
                , pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_hour()
                , pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_minute()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_day()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_month()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_singleDay()->get_year());
            }
            //timerTarget - Repeat Schedule.
            if(pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule())
            {
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActMonExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_monday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActTueExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_tuesday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActWedExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_wednesday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActThuExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_thursday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActFriExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_friday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActSatExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_saturday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeActSunExtUpd_TX, pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_sunday());
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeTypeExtUpd_TX, 1);

              // For un-recieved message send CAN Signal default value (JLRTCU-2523)
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeDateExtUpd_TX, 31);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeMnthExtUpd_TX, 12);
              PUSH_SINGLE_CAN(indata, Signal_PreDepTimeYearExtUpd_TX, 255);

              // Print multiple values into a line
              LOGV("[sendDepartureTimerToCAN] timerIdx[%d] TimerType[%d] DepartureTime h[%d]m[%d] RepeatSchedule M[%d]T[%d]W[%d]Th[%d]F[%d]S[%d]Su[%d]"
                , pDepartureTimer->get_timerIndex()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerType()
                , pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_hour()
                , pDepartureTimer->get_departureTimerDefinition()->get_departureTime().get_minute()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_monday()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_tuesday()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_wednesday()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_thursday()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_friday()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_saturday()
                , pDepartureTimer->get_departureTimerDefinition()->get_timerTarget().get_repeatSchedule()->get_sunday());
            }
            LOGV("[sendDepartureTimerToCAN] num of Signals : %d", indata->NumofSignals);
            EvtVifManager::GetInstance()->sendCanMultiSignal(indata->NumofSignals, indata);

            usleep(txDelayMs*1000);
            LOGV("[sendDepartureTimerToCAN] Delayed %d milisec", txDelayMs);
        }
        else
        {
          LOGE("[sendDepartureTimerToCAN] Delete or Invalid Index", pDepartureTimer->get_timerIndex());
        }
        pOssIndex = _DepartureTimer_list.next(pOssIndex);
    }while(pOssIndex != NULL);
}

void EvChargeAppHandler::sendTariffToCAN(svc::ChargeSettings* data)
{
    LOGE("[sendTariffToCAN] Start");
    svc::Tariffs::tariffs _Tariff_list = data->get_tariffs()->get_tariffs();
    void* pOssIndex = _Tariff_list.first();
    unsigned long txDelayMs = 150;

    do{
        svc::Tariff* pTariff = _Tariff_list.at(pOssIndex);

        switch(pTariff->get_tariffIndex())
        {
            case 0:
            {
                LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- Tariff[0]", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
                mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
                mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

                send_ElectricityPeakTimesExtUpdate1(pTariff);
                usleep(txDelayMs*1000);
                LOGV("[sendTariffToCAN] Delayed %d milisec", txDelayMs);
                break;
            }
            case 1:
            {
                LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- Tariff[1]", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
                mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
                mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

                send_ElectricityPeakTimesExtUpdate2(pTariff);
                usleep(txDelayMs*1000);
                LOGV("[sendTariffToCAN] Delayed %d milisec", txDelayMs);
                break;
            }
            case 2:
            {
                LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- Tariff[2]", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
                mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
                mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

                send_ElectricityPeakTimesExtUpdate3(pTariff);
                usleep(txDelayMs*1000);
                LOGV("[sendTariffToCAN] Delayed %d milisec", txDelayMs);
                break;
            }
            case 3:
            {
                LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!- Tariff[3]", mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"));
                mEvChargeApp.restartTimer(EvChargeApp::EVC_IMC_IGNR_TIMER, mEvChargeApp.getCfgData("EVC_IMC_IGNR_TIME"), true);
                mEvChargeApp.setEVCState(EvChargeApp::EVC_WAIT_IMC_IGNR_STATE);

                send_ElectricityPeakTimesExtUpdate4(pTariff);
                usleep(txDelayMs*1000);
                LOGV("[sendTariffToCAN] Delayed %d milisec", txDelayMs);
                break;
            }
            default:
            {
                LOGE("[ERROR] Unknown tariffIndex[%d] from NGTP", pTariff->get_tariffIndex());
                break;
            }
        }
        pOssIndex = _Tariff_list.next(pOssIndex);
    }while(pOssIndex != NULL);
    LOGE("[sendTariffToCAN] End");
}

#define SIG_CHRG(NUM, TAIL) Signal_TmdChrg##NUM##TAIL
error_t EvChargeAppHandler::send_ElectricityPeakTimesExtUpdate1(svc::Tariff* pTariff)
{
    sp<vifCANContainer> CanData = new vifCANContainer();

    if(pTariff->tariffDefinition_is_present()
       && pTariff->get_tariffDefinition()->get_isEnabled())
    {
        LOGV("[send_ElectricityPeakTimesExtUpdate1] Tariff Enable : %d", pTariff->get_tariffIndex());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvExtUpd_TX),     pTariff->get_tariffDefinition()->get_isEnabled());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvMonExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvTuesExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvWedExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvThurExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvFriExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvSatExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvSunExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday());
        print_TariffSchedule(pTariff);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeAExtUpd_TX), convertEndTime(&(pTariff->get_tariffDefinition()->get_tariffZoneA())));
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneAExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType());
        print_TariffZone(pTariff, 1);

        //Tariff ZoneB
        if(pTariff->get_tariffDefinition()->tariffZoneB_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeBExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneB()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneBExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType());
            print_TariffZone(pTariff, 2);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff B is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeBExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneBExtUpd_TX), 3);
        }

        //Tariff ZoneC
        if(pTariff->get_tariffDefinition()->tariffZoneC_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeCExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneC()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneCExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType());
            print_TariffZone(pTariff, 3);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff C is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeCExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneCExtUpd_TX), 3);
        }

        //Tariff ZoneD
        if(pTariff->get_tariffDefinition()->tariffZoneD_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeDExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneD()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneDExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType());
            print_TariffZone(pTariff, 4);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff D is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeDExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneDExtUpd_TX), 3);
        }

        //Tariff ZoneE
        if(pTariff->get_tariffDefinition()->tariffZoneE_is_present())
        {
            // CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneEExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType());
            print_TariffZone(pTariff, 5);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff E is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneEExtUpd_TX), 3);
        }
    }
    else
    {
        // If Tariff is Disabled, TCU Send All Signals as 0 (SRD_ReqEVC0672_v1)
        LOGV("[send_ElectricityPeakTimesExtUpdate1] DelReq:isPresent[%d]", pTariff->tariffDefinition_is_present());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvExtUpd_TX),     0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvMonExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvTuesExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvWedExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvThurExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvFriExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvSatExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ActvSunExtUpd_TX),  0);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeAExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneAExtUpd_TX),    0);

        //Tariff ZoneB
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeBExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneBExtUpd_TX),    0);

        //Tariff ZoneC
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeCExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneCExtUpd_TX),    0);

        //Tariff ZoneD
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, EndTimeDExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneDExtUpd_TX),    0);

        //Tariff ZoneE (CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists)
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(1, ZoneEExtUpd_TX),    0);
    }
    LOGV("[send_ElectricityPeakTimesExtUpdate1] num of Signals : %d", CanData->NumofSignals);
    return EvtVifManager::GetInstance()->sendCanMultiSignal(CanData->NumofSignals, CanData);
}

error_t EvChargeAppHandler::send_ElectricityPeakTimesExtUpdate2(svc::Tariff* pTariff)
{
    sp<vifCANContainer> CanData = new vifCANContainer();

    if(pTariff->tariffDefinition_is_present()
       && pTariff->get_tariffDefinition()->get_isEnabled())
    {
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvExtUpd_TX),     pTariff->get_tariffDefinition()->get_isEnabled());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvMonExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvTuesExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvWedExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvThurExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvFriExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvSatExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvSunExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday());
        print_TariffSchedule(pTariff);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeAExtUpd_TX), convertEndTime(&(pTariff->get_tariffDefinition()->get_tariffZoneA())));
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneAExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType());
        print_TariffZone(pTariff, 1);

        //Tariff ZoneB
        if(pTariff->get_tariffDefinition()->tariffZoneB_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeBExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneB()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneBExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType());
            print_TariffZone(pTariff, 2);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff B is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeBExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneBExtUpd_TX), 3);
        }

        //Tariff ZoneC
        if(pTariff->get_tariffDefinition()->tariffZoneC_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeCExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneC()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneCExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType());
            print_TariffZone(pTariff, 3);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff C is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeCExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneCExtUpd_TX), 3);
        }

        //Tariff ZoneD
        if(pTariff->get_tariffDefinition()->tariffZoneD_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeDExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneD()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneDExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType());
            print_TariffZone(pTariff, 4);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff D is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeDExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneDExtUpd_TX), 3);
        }

        //Tariff ZoneE
        if(pTariff->get_tariffDefinition()->tariffZoneE_is_present())
        {
            // CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneEExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType());
            print_TariffZone(pTariff, 5);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff E is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneEExtUpd_TX), 3);
        }
    }
    else
    {
        // If Tariff is Disabled, TCU Send All Signals as 0 (SRD_ReqEVC0672_v1)
        LOGV("[send_ElectricityPeakTimesExtUpdate1] DelReq:isPresent[%d]", pTariff->tariffDefinition_is_present());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvExtUpd_TX),     0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvMonExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvTuesExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvWedExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvThurExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvFriExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvSatExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ActvSunExtUpd_TX),  0);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeAExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneAExtUpd_TX),    0);

        //Tariff ZoneB
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeBExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneBExtUpd_TX),    0);

        //Tariff ZoneC
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeCExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneCExtUpd_TX),    0);

        //Tariff ZoneD
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, EndTimeDExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneDExtUpd_TX),    0);

        //Tariff ZoneE (CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists)
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(2, ZoneEExtUpd_TX),    0);
    }
    LOGV("[send_ElectricityPeakTimesExtUpdate1] num of Signals : %d", CanData->NumofSignals);
    return EvtVifManager::GetInstance()->sendCanMultiSignal(CanData->NumofSignals, CanData);
}

error_t EvChargeAppHandler::send_ElectricityPeakTimesExtUpdate3(svc::Tariff* pTariff)
{
    sp<vifCANContainer> CanData = new vifCANContainer();

    if(pTariff->tariffDefinition_is_present()
       && pTariff->get_tariffDefinition()->get_isEnabled())
    {
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvExtUpd_TX),     pTariff->get_tariffDefinition()->get_isEnabled());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvMonExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvTuesExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvWedExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvThurExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvFriExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvSatExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvSunExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday());
        print_TariffSchedule(pTariff);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeAExtUpd_TX), convertEndTime(&(pTariff->get_tariffDefinition()->get_tariffZoneA())));
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneAExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType());
        print_TariffZone(pTariff, 1);

        //Tariff ZoneB
        if(pTariff->get_tariffDefinition()->tariffZoneB_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeBExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneB()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneBExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType());
            print_TariffZone(pTariff, 2);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff B is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeBExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneBExtUpd_TX), 3);
        }

        //Tariff ZoneC
        if(pTariff->get_tariffDefinition()->tariffZoneC_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeCExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneC()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneCExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType());
            print_TariffZone(pTariff, 3);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff C is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeCExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneCExtUpd_TX), 3);
        }

        //Tariff ZoneD
        if(pTariff->get_tariffDefinition()->tariffZoneD_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeDExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneD()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneDExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType());
            print_TariffZone(pTariff, 4);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff D is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeDExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneDExtUpd_TX), 3);
        }

        //Tariff ZoneE
        if(pTariff->get_tariffDefinition()->tariffZoneE_is_present())
        {
            // CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneEExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType());
            print_TariffZone(pTariff, 5);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff E is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneEExtUpd_TX), 3);
        }
    }
    else
    {
        // If Tariff is Disabled, TCU Send All Signals as 0 (SRD_ReqEVC0672_v1)
        LOGV("[send_ElectricityPeakTimesExtUpdate1] DelReq:isPresent[%d]", pTariff->tariffDefinition_is_present());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvExtUpd_TX),     0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvMonExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvTuesExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvWedExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvThurExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvFriExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvSatExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ActvSunExtUpd_TX),  0);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeAExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneAExtUpd_TX),    0);

        //Tariff ZoneB
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeBExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneBExtUpd_TX),    0);

        //Tariff ZoneC
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeCExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneCExtUpd_TX),    0);

        //Tariff ZoneD
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, EndTimeDExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneDExtUpd_TX),    0);

        //Tariff ZoneE (CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists)
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(3, ZoneEExtUpd_TX),    0);
    }
    LOGV("[send_ElectricityPeakTimesExtUpdate3] num of Signals : %d", CanData->NumofSignals);
    return EvtVifManager::GetInstance()->sendCanMultiSignal(CanData->NumofSignals, CanData);
}

error_t EvChargeAppHandler::send_ElectricityPeakTimesExtUpdate4(svc::Tariff* pTariff)
{
    sp<vifCANContainer> CanData = new vifCANContainer();
    if(pTariff->tariffDefinition_is_present()
       && pTariff->get_tariffDefinition()->get_isEnabled())
    {
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvExtUpd_TX),     pTariff->get_tariffDefinition()->get_isEnabled());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvMonExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvTuesExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvWedExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvThurExtUpd_TX), pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvFriExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvSatExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvSunExtUpd_TX),  pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday());
        print_TariffSchedule(pTariff);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeAExtUpd_TX), convertEndTime(&(pTariff->get_tariffDefinition()->get_tariffZoneA())));
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneAExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType());
        print_TariffZone(pTariff, 1);

        //Tariff ZoneB
        if(pTariff->get_tariffDefinition()->tariffZoneB_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeBExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneB()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneBExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType());
            print_TariffZone(pTariff, 2);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff B is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeBExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneBExtUpd_TX), 3);
        }

        //Tariff ZoneC
        if(pTariff->get_tariffDefinition()->tariffZoneC_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeCExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneC()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneCExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType());
            print_TariffZone(pTariff, 3);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff C is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeCExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneCExtUpd_TX), 3);
        }

        //Tariff ZoneD
        if(pTariff->get_tariffDefinition()->tariffZoneD_is_present())
        {
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeDExtUpd_TX), convertEndTime(pTariff->get_tariffDefinition()->get_tariffZoneD()));
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneDExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType());
            print_TariffZone(pTariff, 4);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff D is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeDExtUpd_TX), 127);
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneDExtUpd_TX), 3);
        }

        //Tariff ZoneE
        if(pTariff->get_tariffDefinition()->tariffZoneE_is_present())
        {
            // CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneEExtUpd_TX),    pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType());
            print_TariffZone(pTariff, 5);
        }
        else
        {
            // SRD_ReqEVC0673_v1 by PCR039
            LOGV("[send_ElectricityPeakTimesExtUpdate1] IDX[%d] Tariff E is sent as Default to CAN", pTariff->get_tariffIndex());
            PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneEExtUpd_TX), 3);
        }
    }
    else
    {
        // If Tariff is Disabled, TCU Send All Signals as 0 (SRD_ReqEVC0672_v1)
        LOGV("[send_ElectricityPeakTimesExtUpdate1] DelReq:isPresent[%d]", pTariff->tariffDefinition_is_present());
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvExtUpd_TX),     0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvMonExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvTuesExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvWedExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvThurExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvFriExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvSatExtUpd_TX),  0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ActvSunExtUpd_TX),  0);

        //Tariff ZoneA
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeAExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneAExtUpd_TX),    0);

        //Tariff ZoneB
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeBExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneBExtUpd_TX),    0);

        //Tariff ZoneC
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeCExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneCExtUpd_TX),    0);

        //Tariff ZoneD
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, EndTimeDExtUpd_TX), 0);
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneDExtUpd_TX),    0);

        //Tariff ZoneE (CAN Signal TmdChrg1EndTimeEExtUpd doesn't exists)
        PUSH_SINGLE_CAN(CanData, SIG_CHRG(4, ZoneEExtUpd_TX),    0);
    }
    LOGV("[send_ElectricityPeakTimesExtUpdate4] num of Signals : %d", CanData->NumofSignals);
    return EvtVifManager::GetInstance()->sendCanMultiSignal(CanData->NumofSignals, CanData);
}

void EvChargeAppHandler::print_TariffSchedule(svc::Tariff* pTariff)
{
    LOGV("[print_TariffSchedule] tariffIndex[%d] isEnabled[%d] RepeatSchedule M[%d]T[%d]W[%d]Th[%d]F[%d]S[%d]SU[%d]"
        , pTariff->get_tariffIndex()
        , pTariff->get_tariffDefinition()->get_isEnabled()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_monday()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_tuesday()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_wednesday()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_thursday()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_friday()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_saturday()
        , pTariff->get_tariffDefinition()->get_repeatSchedule().get_sunday());
}

void EvChargeAppHandler::print_TariffZone(svc::Tariff* pTariff, uint8_t tariffZoneNum)
{
    switch(tariffZoneNum)
    {
        case 1:
            LOGV("[print_TariffZone] TariffZoneA tariffIndex[%d] EndTime h[%d]m[%d] BandType[%d]"
                , pTariff->get_tariffIndex()
                , pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_hour()
                , pTariff->get_tariffDefinition()->get_tariffZoneA().get_endTime()->get_minute()
                , pTariff->get_tariffDefinition()->get_tariffZoneA().get_bandType());
            break;
        case 2:
            LOGV("[print_TariffZone] TariffZoneB tariffIndex[%d] EndTime h[%d]m[%d] BandType[%d]"
                , pTariff->get_tariffIndex()
                , pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_hour()
                , pTariff->get_tariffDefinition()->get_tariffZoneB()->get_endTime()->get_minute()
                , pTariff->get_tariffDefinition()->get_tariffZoneB()->get_bandType());
            break;
        case 3:
            LOGV("[print_TariffZone] TariffZoneC tariffIndex[%d] EndTime h[%d]m[%d] BandType[%d]"
                , pTariff->get_tariffIndex()
                , pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_hour()
                , pTariff->get_tariffDefinition()->get_tariffZoneC()->get_endTime()->get_minute()
                , pTariff->get_tariffDefinition()->get_tariffZoneC()->get_bandType());
            break;
        case 4:
            LOGV("[print_TariffZone] TariffZoneD tariffIndex[%d] EndTime h[%d]m[%d] BandType[%d]"
                , pTariff->get_tariffIndex()
                , pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_hour()
                , pTariff->get_tariffDefinition()->get_tariffZoneD()->get_endTime()->get_minute()
                , pTariff->get_tariffDefinition()->get_tariffZoneD()->get_bandType());
            break;
        case 5:
            LOGV("[print_TariffZone] TariffZoneE tariffIndex[%d] EndTime h[%d]m[%d] BandType[%d]"
                , pTariff->get_tariffIndex()
                , pTariff->get_tariffDefinition()->get_tariffZoneE()->get_endTime()->get_hour()
                , pTariff->get_tariffDefinition()->get_tariffZoneE()->get_endTime()->get_minute()
                , pTariff->get_tariffDefinition()->get_tariffZoneE()->get_bandType());
            break;
        default :
            LOGE("[print_TariffZone]Invalid Zone : %d", tariffZoneNum);
            break;
    }
}

uint32_t EvChargeAppHandler::convertEndTime(svc::TariffZone* tariffZone)
{
    uint32_t endTimeMin;
    endTimeMin = (tariffZone->get_endTime()->get_hour()*4) + (tariffZone->get_endTime()->get_minute()/15);
    return endTimeMin;
}
//////////////Complare
#undef SIG_CHRG