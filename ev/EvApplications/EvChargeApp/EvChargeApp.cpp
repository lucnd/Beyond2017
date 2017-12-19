//EVC Feature Static Class.
#include "EvServiceManager.h"
#include "EvChargeApp.h"
#include "EvChargeAppHandler.h"
#include "EvChargeAppTimer.h"
#if defined(PCR041ENABLE)
#include "EvVifHandler.h"
#endif

#include "EvtUtil/EvtUtil.h"
#include "vifManagerService/LGE_CANSignalFrameID.h"
#include "EvtUtil.h"

using namespace SignalFrame_PMZ_e;

#define LOG_TAG "[EVT]EVChargeApp"
#include "Log.h"

/////////////////////////////// About CAN Frame Notification ////////////////////////
inline void rcv_Notification_GWM_PMZ_AD_Hybrid(EvCanFrame *pEvCanFrame);
inline void rcv_Notification_GWM_PMZ_R_Hybrid(EvCanFrame *pEvCanFrame);
inline void rcv_Notification_GWM_PMZ_S_Hybrid(EvCanFrame *pEvCanFrame);
inline void rcv_Notification_GWM_PMZ_T_Hybrid(EvCanFrame *pEvCanFrame);
inline void rcv_Notification_GWM_PMZ_U_Hybrid(EvCanFrame *pEvCanFrame);
inline void rcv_Notification_PCM_PMZ_Q_Hybrid(EvCanFrame *pEvCanFrame);
inline void rcv_Notification_PCM_PMZ_S_Hybrid(EvCanFrame *pEvCanFrame);
// signal frame GWM_PMZ_AD_Hybrid --> Implement OK
// signal frame GWM_PMZ_AE_Hybrid --> No spec.
// signal frame GWM_PMZ_R_Hybrid --> Implement OK
// signal frame GWM_PMZ_S_Hybrid --> Implement OK
// signal frame GWM_PMZ_T_Hybrid --> Implement OK
// signal frame GWM_PMZ_U_Hybrid --> Implement OK
// signal frame GWM_PMZ_V_Hybrid --> No
// signal frame PCM_PMZ_B_Hybrid --> No
// signal frame PCM_PMZ_C_Hybrid --> No
// signal frame PCM_PMZ_G_Hybrid --> No
// signal frame PCM_PMZ_H_Hybrid --> No
// signal frame PCM_PMZ_J_Hybrid --> No
// signal frame PCM_PMZ_K_Hybrid --> No
// signal frame PCM_PMZ_L_Hybrid --> No
// signal frame PCM_PMZ_Q_Hybrid --> No
// signal frame PCM_PMZ_S_Hybrid --> No
//////////////////////////////////////////////////////////////////////////////////////

EvChargeApp::EvChargeApp()
{
    state = EVC_OFF;
    m_Looper = SLLooper::myLooper();
    mHandler = new EvChargeAppHandler(m_Looper, *this);

    //memset(arrayNgtpResult, EMPTY, sizeof(arrayNgtpResult)*sizeof(int32_t));
    arrayNgtpResult.fill(EMPTY);

    current_ChargeSettings.omit_chargeNowSetting();
    current_ChargeSettings.omit_maxStateOfCharge();
    current_ChargeSettings.omit_chargingModeChoice();
    current_ChargeSettings.omit_departureTimers();
    current_ChargeSettings.omit_tariffs();

    m_mapFrameFunc[SignalFrame_GWM_PMZ_AD_Hybrid_RX] = &rcv_Notification_GWM_PMZ_AD_Hybrid;
    m_mapFrameFunc[SignalFrame_GWM_PMZ_R_Hybrid_RX] = &rcv_Notification_GWM_PMZ_R_Hybrid;
    m_mapFrameFunc[SignalFrame_GWM_PMZ_S_Hybrid_RX] = &rcv_Notification_GWM_PMZ_S_Hybrid;
    m_mapFrameFunc[SignalFrame_GWM_PMZ_T_Hybrid_RX] = &rcv_Notification_GWM_PMZ_T_Hybrid;
    m_mapFrameFunc[SignalFrame_GWM_PMZ_U_Hybrid_RX] = &rcv_Notification_GWM_PMZ_U_Hybrid;
    m_mapFrameFunc[SignalFrame_PCM_PMZ_Q_Hybrid_RX] = &rcv_Notification_PCM_PMZ_Q_Hybrid;
    m_mapFrameFunc[SignalFrame_PCM_PMZ_S_Hybrid_RX] = &rcv_Notification_PCM_PMZ_S_Hybrid;

    bEVC_EVI_TIMER_ALIVE = false;
    latestErrorCode = E_OK;

    initializeCompFlags();
}

void EvChargeApp::onStart()
{
    for(int i=0 ; i < EVC_END_TIMER ; i++)
    {
        EVC_Timers[i] = NULL;
    }

    setState(EVC_IDLE);
    triggerECWstate();
}

void EvChargeApp::initialSetupStart() // for FIRST Time Setting.
{
    setState(EVC_FIRST_TIME_STATE);

    //SRD_ReqEVC0100_v1
    sendCanSignal(Signal_TelematicWakeUpReq_TX, 1);

    //BUS HOLD
    EvVehicleStatus::GetInstance()->evKeepCANbus(__func__);

    // Send Handler for Start First Time Process.
    mHandler->obtainMessage(/*What*/EvChargeAppHandler::Start_FT_Status)->sendToTarget();
}
void EvChargeApp::initialSetupEnd()
{
    //release BUS - SRD_ReqEVC0104_v1
    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);

    sendCanSignal(Signal_TelematicWakeUpReq_TX, 0);
    setState(EVC_IDLE);
}

bool EvChargeApp::check_ChargDataWakeUpAllow()
{
    int32_t _ChargDataWakeUpAllowed = EvtVifManager::GetInstance()->queryCanData(Signal_ChargDataWakeUpAllowed_RX);
    if(_ChargDataWakeUpAllowed != 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void EvChargeApp::rcv_NgtpMessage(svc::ChargeSettings* msg, uint32_t _msgCreationTime)
{
    // SRD_ReqEVC0035 - In Cas of EVC Disable, EVC will not do anything
    if(EvConfigData::GetInstance()->configData["AppModeEVC"] != ENABLED)
    {
        LOGE("[rcv_NgtpMessage] EVC Disabled. Not Sending CSUpdate");
        return;
    }

    //SRD_ReqEVC0035_v2, SRD_ReqEVC0613_v1
    if(isConflictWithOnBoardChange())
    {
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::conflictWithOnboardChange_chosen);
        return;
    }

    // SRD_ReqEVC0618_v1
    if(getState() != EVC_IDLE)
    {
        LOGE("[rcv_NgtpMessage] EVC is not 'EVC_IDLE' status! (current status = [%d]).", getState());
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::incorrectState_chosen);
        return;
    }

    //SRD_ReqEVC0021_v2
    if(check_ChargDataWakeUpAllow() == false)
    {
        LOGV("[rcv_NgtpMessage] <ChargDataWakeUpAllowed> is not allowed. ErrorCode:notAllowed!");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::notAllowed_chosen);
        return;
    }

    // SRD_ReqEVC0600_v2 && SRD_ReqEVC0616_v1
    if(EvConfigData::GetInstance()->getEvCfgData("POWER_MODE") >= 7)
    {
        LOGE("[rcv_NgtpMessage] PM >= 7 : [%d]  EVC will send errorCode response : conditionsNotCorrect", EvConfigData::GetInstance()->getEvCfgData("POWER_MODE"));
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::conditionsNotCorrect_chosen );
        return;
    }

    int32_t dupResult = check_ChargeSettingDuplication(msg);
    LOGV("[rcv_NgtpMessage] dup[%d] latestErr[%d]", dupResult, latestErrorCode);

    if(dupResult == E_OK)
    {
        LOGE("[rcv_NgtpMessage] ChargeSettingRequest is same as local stored value : parameterAlreadyInRequestedState");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen );
        return;
    }

/*
// This is workaround code.
// SRD_ReqEVC0615_v1, Fix JLRTCU-2493 - Avoid ParameterAlreadyRequested when latestErrorCode is ExecutionFailure
if( (dupResult== E_OK) && (latestErrorCode == E_OK))
{
    LOGE("[rcv_NgtpMessage] ChargeSettingRequest is same as local stored value : parameterAlreadyInRequestedState");
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen );
    return;
}else
{
    latestErrorCode = E_OK; // initilization.
}
*/


    LOGV("[rcv_NgtpMessage]EVC is starting to receive NGTP Message [%d]", _msgCreationTime);

    // EVC should start store NGTP Message only if no error code generated.
    //EvtElectricVehicleInformation::GetInstance()->setChargeSetting(*msg);

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setDeadBandTimeStamp(UTIL::get_NGTP_time());

    //SRD_ReqEVC0021_v1
    sendCanSignal(Signal_TelematicWakeUpReq_TX, 1);

    //BUS HOLD
    EvVehicleStatus::GetInstance()->evKeepCANbus(__func__);
    setTimer(EVC_GWM_BUSOPEN_TIMER, getCfgData("EVC_GWM_BUSOPEN_TIME"));  // Trigger Timer --> EVC_IMC_WAKEUP_TIMER
    LOGV("EVC_GWM_BUSOPEN_TIMER[%u]s start!", getCfgData("EVC_GWM_BUSOPEN_TIME"));

    // Trigger Timer --> EVC_IMC_WKUP_TIMER
    setTimer(EVC_IMC_WKUP_TIMER, getCfgData("EVC_IMC_WKUP_TIME"), false);  // Trigger Timer --> EVC_IMC_WAKEUP_TIMER
    LOGV("EVC_IMC_WKUP_TIMER[%u]s start!", getCfgData("EVC_IMC_WKUP_TIME"));

    // Keep Current NGTP Data
    list_Ngtp_ChargeSettings.push_back(new svc::ChargeSettings(*msg));           // Manage Heap using List!!

    // State change.
    setState(EVC_WAIT_WAKEUP_STATE);
}

void EvChargeApp::DBG_NgtpMessage(svc::ChargeSettings* msg)
{
    if(EvConfigData::GetInstance()->configData["AppModeEVC"] != ENABLED)
    {
        LOGE("[DBG_NgtpMessage] EVC Disabled. Not Sending CSUpdate");
        return;
    }

    //SRD_ReqEVC0035_v2, SRD_ReqEVC0613_v1
    if(isConflictWithOnBoardChange())
    {
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::conflictWithOnboardChange_chosen);
        return;
    }
    // SRD_ReqEVC0600_v2 && SRD_ReqEVC0616_v1
    if(EvConfigData::GetInstance()->getEvCfgData("POWER_MODE") >= 7)
    {
        LOGE("[DBG_NgtpMessage] PM >= 7 : [%d]  EVC will send errorCode response", EvConfigData::GetInstance()->getEvCfgData("POWER_MODE"));
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::conditionsNotCorrect_chosen );
        return;
    }

    int32_t dupResult = check_ChargeSettingDuplication(msg);
    LOGV("[DBG_NgtpMessage] dup[%d] latestErr[%d]", dupResult, latestErrorCode);


    if(dupResult == E_OK)
    {
        LOGE("[DBG_NgtpMessage] ChargeSettingRequest is same as local stored value : parameterAlreadyInRequestedState");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen );
        return;
    }

    /*
    // SRD_ReqEVC0615_v1, Fix JLRTCU-2493 - Avoid ParameterAlreadyRequested when latestErrorCode is ExecutionFailure
    if( (dupResult== E_OK) && (latestErrorCode == E_OK))
    {
        LOGE("[DBG_NgtpMessage] ChargeSettingRequest is same as local stored value : parameterAlreadyInRequestedState");
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen );
        return;
    }else
    {
        latestErrorCode = E_OK; // initilization.
    }
    */


    LOGV("[DBG_NgtpMessage] EVC is starting to receive NGTP Message");
    // EVC should start store NGTP Message only if no error code generated.
    //EvtElectricVehicleInformation::GetInstance()->setChargeSetting(*msg);

    // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
    EvtChargeSettings::GetInstance()->setDeadBandTimeStamp(UTIL::get_NGTP_time());

    //SRD_ReqEVC0021_v1
    sendCanSignal(Signal_TelematicWakeUpReq_TX, 1);

    //BUS HOLD
    EvVehicleStatus::GetInstance()->evKeepCANbus(__func__);
    setTimer(EVC_GWM_BUSOPEN_TIMER, getCfgData("EVC_GWM_BUSOPEN_TIME"));  // Trigger Timer --> EVC_IMC_WAKEUP_TIMER
    LOGV("EVC_GWM_BUSOPEN_TIMER[%u]s start!", getCfgData("EVC_GWM_BUSOPEN_TIME"));

    // Trigger Timer --> EVC_IMC_WKUP_TIMER
    setTimer(EVC_IMC_WKUP_TIMER, getCfgData("EVC_IMC_WKUP_TIME"), false);  // Trigger Timer --> EVC_IMC_WAKEUP_TIMER
    LOGV("EVC_IMC_WKUP_TIMER[%u]s start!", getCfgData("EVC_IMC_WKUP_TIME"));

    // Keep Current NGTP Data
    list_Ngtp_ChargeSettings.push_back(new svc::ChargeSettings(*msg));           // Manage Heap using List!!

    // State change.
    setState(EVC_WAIT_WAKEUP_STATE);
}

int32_t EvChargeApp::check_ChargeSettingDuplication(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    int32_t result = E_OK;
    int32_t tmpResult;
    if(ngtp_ChargeSettingMsg->chargeNowSetting_is_present())
    {
        tmpResult = check_ChargeNowSetting_Dup(ngtp_ChargeSettingMsg);
        result += tmpResult;
        LOGV("[check_ChargeSettingDuplication] chargeNowSetting result[%d] Total[%d]",tmpResult , result);
    }
    if(ngtp_ChargeSettingMsg->maxStateOfCharge_is_present())
    {
        tmpResult = check_MaxStateOfCharge_Dup(ngtp_ChargeSettingMsg);
        result += tmpResult;
        LOGV("[check_ChargeSettingDuplication] maxStateOfCharge result[%d] Total[%d]",tmpResult , result);
    }
    if(ngtp_ChargeSettingMsg->chargingModeChoice_is_present())
    {
        tmpResult = check_ChargingModeChoice_Dup(ngtp_ChargeSettingMsg);
        result += tmpResult;
        LOGV("[check_ChargeSettingDuplication] ChargingModeChoice result[%d] Total[%d]",tmpResult , result);
    }
    if(ngtp_ChargeSettingMsg->departureTimers_is_present())
    {
        tmpResult = check_DepartureTimers_Service(COMPARE_TO_TSP, ngtp_ChargeSettingMsg);
        result += tmpResult;
        LOGV("[check_ChargeSettingDuplication] DepartureTimers result[%d] Total[%d]",tmpResult , result);
    }
    if(ngtp_ChargeSettingMsg->tariffs_is_present())
    {
        tmpResult = check_Tariffs_Service(ngtp_ChargeSettingMsg);
        result += tmpResult;
        LOGV("[check_ChargeSettingDuplication] tariffs result[%d] Total[%d]",tmpResult , result);
    }
    LOGV("[check_ChargeSettingDuplication] result[%d] [%s]", result, result == 0 ? "Duplicated CSRequest" : "Non-Duplicated CSRequest");
    return result;
}

//EVC_IMC_WAKEUP_TIMER receiver!!
#define EVC_START_CHARGESETTING_HANDLER(A) do{ \
                                if(arrayNgtpResult[A] == EMPTY){ \
                                    if(!mHandler->hasMessages(A)){ \
                                        mHandler->obtainMessage(A, new_ChargeSettingMsg)->sendToTarget(); \
                                        arrayNgtpResult[A] = WORKING; \
                                    } \
                                } \
                                else{LOGE("[ERROR] Duplicated [%s] has occurred!!! Contact JLR or wCar side!", #A);}\
                            }while(0)

void EvChargeApp::timeout_EVC_IMC_WKUP_TIMER()
{
    svc::ChargeSettings* new_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();

    // Start Service.
    if(new_ChargeSettingMsg->chargeNowSetting_is_present())
    {
        EVC_START_CHARGESETTING_HANDLER(EvChargeAppHandler::NGTP_chargeNowSetting_is_present);
    }
    if(new_ChargeSettingMsg->maxStateOfCharge_is_present())
    {
        EVC_START_CHARGESETTING_HANDLER(EvChargeAppHandler::NGTP_maxStateOfCharge_is_present);
    }
    if(new_ChargeSettingMsg->chargingModeChoice_is_present())
    {
        EVC_START_CHARGESETTING_HANDLER(EvChargeAppHandler::NGTP_chargingModeChoice_is_present);
    }
    if(new_ChargeSettingMsg->departureTimers_is_present())
    {
        EVC_START_CHARGESETTING_HANDLER(EvChargeAppHandler::NGTP_departureTimers_is_present);
    }
    if(new_ChargeSettingMsg->tariffs_is_present())
    {
        EVC_START_CHARGESETTING_HANDLER(EvChargeAppHandler::NGTP_tariffs_is_present);
    }

    // State change.
    //setState(EVC_WAIT_IMC_IGNR_STATE);

    // Trigger Timer --> EVC_IMC_IGNR_TIMER
    //setTimer(EVC_IMC_IGNR_TIMER, getCfgData("EVC_IMC_IGNR_TIME"), true);
    //LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!", getCfgData("EVC_IMC_IGNR_TIME"));
}
#undef EVC_START_CHARGESETTING_HANDLER

//EVC_IMC_IGNR_TIMER
void EvChargeApp::timeout_EVC_IMC_IGNR_TIMER()
{

    int32_t _result = check_ChargeSettingRequest();

    // set to IDLE before sending ChargeSettingResponse
    setState(EVC_IDLE);

    send_ChargeSettingSResponse(_result);

    cancelTimer(EVC_GWM_BUSOPEN_TIMER);

    // SRD_ReqEVC0025_v1
    sendCanSignal(Signal_TelematicWakeUpReq_TX, 0);

    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
}

//EVC_GWM_BUSOPEN_TIMER
void EvChargeApp::timeout_EVC_GWM_BUSOPEN_TIMER()
{
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::timeout_chosen);

    cancelAllTimer();
    EvVehicleStatus::GetInstance()->evReleaseCANbus(__func__);
    setState(EVC_IDLE);
}

//EVC_EVI_PUSH_TIMER
void EvChargeApp::timeout_EVC_EVI_PUSH_TIMER()
{
    bEVC_EVI_TIMER_ALIVE =  false;
    LOGV("[timeout_EVC_EVI_PUSH_TIMER] expired - bEVC_EVI_TIMER_ALIVE = false");
    sendChargingStatusByCondition();
}

void EvChargeApp::sendChargingStatusByCondition()
{
    // In Cas of EVC Disable, EVC will not do anything
    if(EvConfigData::GetInstance()->configData["AppModeEVC"] != ENABLED)
    {
        LOGE("[sendChargingStatusByCondition] EVC Disabled. Not Sending CSUpdate");
        return;
    }

    // SRD_ReqEVC0040_v1
    EvtElectricVehicleInformation *evi = EvtElectricVehicleInformation::GetInstance();
    uint8_t power_mode;
    int32_t socField;
    int32_t latestSocField;
    int32_t diff;
    int32_t EVC_EVI_PUSH_PERC;

    power_mode = EvVehicleStatus::GetInstance()->getCurPowerMode();
    if(power_mode < 7)
    {
        socField = (int32_t)evi->getStateOfChargeField();
        latestSocField = (int32_t)evi->getLatestStateOfChargeField();
        diff = abs(socField - latestSocField);
        EVC_EVI_PUSH_PERC = getCfgData("EVC_EVI_PUSH_PERC");
        if(diff >= EVC_EVI_PUSH_PERC)
        {
            LOGV("[sendChargingStatusByCondition] EVC_EVI_PUSH_PERC[%d] diff[%d] cur[%d] Latest[%d]", EVC_EVI_PUSH_PERC ,diff ,socField, latestSocField);
            // setCurrent SOC value to compare with upcoming SOC Value.
             evi->setLatestStateOfChargeField(socField);
            // send ChargingStatusUpdate
            EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE, E_OK);
            // Start EVC_EVI_PUSH_TIMER
            start_EVC_EVI_PUSH_TIMER();
        }
        else
        {
            LOGV("[sendChargingStatusByCondition] SOC Difference is less than EVC_EVI_PUSH_PERC[%d] diff[%d] cur[%d] Latest[%d]", EVC_EVI_PUSH_PERC ,diff ,socField, latestSocField);
        }
    }
    else
    {
        LOGV("[sendChargingStatusByCondition] PowerMode is larger than 7 : [%d]", power_mode);
    }

}


int32_t EvChargeApp::check_ChargeSettingRequest()
{
    //Get service result.
    svc::ChargeSettings* ngtp_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();
    int32_t _all_service_result = E_OK;

    // set All compFlags as False
    initializeCompFlags();

    if(arrayNgtpResult[EvChargeAppHandler::NGTP_chargeNowSetting_is_present] == WORKING)
    {
        arrayNgtpResult[EvChargeAppHandler::NGTP_chargeNowSetting_is_present] = check_ChargeNowSetting_Service(ngtp_ChargeSettingMsg);

        // setCompareFlags and store values to internalmemory
        if(arrayNgtpResult[EvChargeAppHandler::NGTP_chargeNowSetting_is_present] == E_OK)
        {
            bCompChargeNow = true;
            EvtChargeSettings::GetInstance()->setChargeNowSettingsFromNGTP(*ngtp_ChargeSettingMsg);
        }
        LOGV("[check_ChargeSettingRequest]Result of chargeNowSetting [%d] Comp[%s]", arrayNgtpResult[EvChargeAppHandler::NGTP_chargeNowSetting_is_present]
                                                                            , bCompChargeNow ? "T" : "F");
    }

    if(arrayNgtpResult[EvChargeAppHandler::NGTP_maxStateOfCharge_is_present] == WORKING)
    {
        arrayNgtpResult[EvChargeAppHandler::NGTP_maxStateOfCharge_is_present] = check_MaxStateOfCharge_Service(COMPARE_TO_IMC, ngtp_ChargeSettingMsg);

        // setCompareFlags and store values to internalmemory
        if(arrayNgtpResult[EvChargeAppHandler::NGTP_maxStateOfCharge_is_present] == E_OK)
        {
            bCompMaxSOC = true;
            EvtChargeSettings::GetInstance()->setMaxSOCFromNGTP(*ngtp_ChargeSettingMsg);
        }
        LOGV("[check_ChargeSettingRequest]Result of MaxStateOfCharge [%d] Comp[%s]", arrayNgtpResult[EvChargeAppHandler::NGTP_maxStateOfCharge_is_present]
                                                                             , bCompMaxSOC ? "T" : "F");
    }

    if(arrayNgtpResult[EvChargeAppHandler::NGTP_chargingModeChoice_is_present] == WORKING)
    {
        arrayNgtpResult[EvChargeAppHandler::NGTP_chargingModeChoice_is_present] = check_ChargingModeChoice_Service(ngtp_ChargeSettingMsg);

        // setCompareFlags and store values to internalmemory
        if(arrayNgtpResult[EvChargeAppHandler::NGTP_chargingModeChoice_is_present] == E_OK)
        {
            bCompChargingMode = true;
            EvtChargeSettings::GetInstance()->setChargeModeChoicesFromNGTP(*ngtp_ChargeSettingMsg);
        }
        LOGV("[check_ChargeSettingRequest]Result of ChargeModeChoice [%d] Comp[%s]", arrayNgtpResult[EvChargeAppHandler::NGTP_chargingModeChoice_is_present]
                                                                             , bCompChargingMode ? "T" : "F");
    }

    if(arrayNgtpResult[EvChargeAppHandler::NGTP_departureTimers_is_present] == WORKING)
    {
        arrayNgtpResult[EvChargeAppHandler::NGTP_departureTimers_is_present] = check_DepartureTimers_Service(COMPARE_TO_IMC, ngtp_ChargeSettingMsg);

        // setCompareFlags and store values to internalmemory
        if(arrayNgtpResult[EvChargeAppHandler::NGTP_departureTimers_is_present] == E_OK)
        {
            bCompDeptTimer = true;
            //EvtChargeSettings::GetInstance()->delDepartureTimersFromNGTP(*ngtp_ChargeSettingMsg);
            //EvtChargeSettings::GetInstance()->setDepartureTimersFromNGTP(*ngtp_ChargeSettingMsg);
        }
        LOGV("[check_ChargeSettingRequest]Result of DepartureTimer [%d] Comp[%s]", arrayNgtpResult[EvChargeAppHandler::NGTP_departureTimers_is_present]
                                                                           , bCompDeptTimer ? "T" : "F");
    }

    if(arrayNgtpResult[EvChargeAppHandler::NGTP_tariffs_is_present] == WORKING)
    {
        arrayNgtpResult[EvChargeAppHandler::NGTP_tariffs_is_present] = check_Tariffs_Service(ngtp_ChargeSettingMsg);

        // setCompareFlags and store values to internalmemory
        if(arrayNgtpResult[EvChargeAppHandler::NGTP_tariffs_is_present] == E_OK)
        {
            bCompTariff = true;
            EvtChargeSettings::GetInstance()->setTariffsFromNGTP(*ngtp_ChargeSettingMsg);
        }
        LOGV("[check_ChargeSettingRequest]Result of Tariff [%d] Comp[%s]", arrayNgtpResult[EvChargeAppHandler::NGTP_tariffs_is_present]
                                                                  , bCompTariff ? "T" : "F");
    }

    //Check All service status.
    if(EvtChargeSettings::GetInstance()->bParamOutOfRange)
    {
        _all_service_result = svc::ErrorCode::parameterOutOfRange_chosen;
    }
    else
    {
        for(int32_t _result : arrayNgtpResult)
        {
            if(_result != EMPTY && _result != E_OK)
            {
                _all_service_result = _result;
            }
        }
    }
    LOGV("[check_ChargeSettingRequest]Return _all_service_result : %d",_all_service_result );
    return _all_service_result;
}

void EvChargeApp::send_ChargeSettingSResponse(int32_t _result)
{
    if(_result == E_OK) //E_OK = 0
    {
        LOGV("[send_ChargeSettingSResponse] E_OK ErrorCode : %d", _result);
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, E_OK);
    }
    else
    {
        LOGV("[send_ChargeSettingSResponse] Not E_OK ErrorCode : %d", _result);
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, _result);
    }

    if(!list_Ngtp_ChargeSettings.empty())
    {
        current_ChargeSettings = svc::ChargeSettings(*(list_Ngtp_ChargeSettings.front()));
        list_Ngtp_ChargeSettings.pop_front();
    }
    // Initialiize arrayNGTP Values
    arrayNgtpResult.fill(EMPTY);
}

//SRD_ReqEVC0631_v1:
error_t EvChargeApp::check_ChargeNowSetting_Service(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    //svc::ChargeSettings* ngtp_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();
    const uint8_t _reqValue = *(ngtp_ChargeSettingMsg->get_chargeNowSetting());
    const int _canValue = EvtVifManager::GetInstance()->queryCanData(Signal_ChargeNowRequestExt_RX);
    // Question about Signal_ChargeNowRequest_RX --> Signal_ChargeNowRequestExt_RX ??????
    LOGV("[check_ChargeNowSetting_Service] NGTPValue[%d] CAN[%d]", _reqValue, _canValue);
    if((_reqValue == svc::ChargeNowSetting_default) && (_canValue == 0))
    {
        return E_OK;
    }
    if((_reqValue == svc::force_on) && (_canValue == 1))
    {
        return E_OK;
    }
    if((_reqValue == svc::force_off) && (_canValue == 2))
    {
        return E_OK;
    }
    latestErrorCode += svc::ErrorCode::executionFailure_chosen;
    return svc::ErrorCode::executionFailure_chosen;
}

//SRD_ReqEVC0641_v1
error_t EvChargeApp::check_MaxStateOfCharge_Service(uint8_t compareMode, svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    //svc::ChargeSettings* ngtp_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();

    // Check if MaxSoc Value is valid
    bool bPermMaxSocValid = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc()) ? true : false;
    bool bOneOffMaxSocValid = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc()) ? true : false;

    int32_t _reqValue_permanentMaxSoc;
    int32_t _canValue_permanentMaxSoc;

    int32_t _reqValue_oneOffMaxSoc;
    int32_t _canValue_oneOffMaxSoc;

    if(bPermMaxSocValid)
    {
        _reqValue_permanentMaxSoc = *(ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc());
        _canValue_permanentMaxSoc = EvtVifManager::GetInstance()->queryCanData(Signal_BulkSoCTargetDisp_RX);
        _canValue_permanentMaxSoc = _canValue_permanentMaxSoc > 1000 ? -1 : _canValue_permanentMaxSoc/10 ;

        LOG_EV("permanentMaxSoc Ngtp[%d] CAN[%d]",_reqValue_permanentMaxSoc, _canValue_permanentMaxSoc);
    }
    else
    {
        LOGE("PermMaxSOC is false");
    }

    if(bOneOffMaxSocValid)
    {
        _reqValue_oneOffMaxSoc = *(ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc());
        _canValue_oneOffMaxSoc = EvtVifManager::GetInstance()->queryCanData(Signal_BulkSoCSingleUseDisp_RX);
        _canValue_oneOffMaxSoc = _canValue_oneOffMaxSoc > 1000 ? -1 : _canValue_oneOffMaxSoc/10 ;

        LOG_EV("oneOFfMaxSoc Ngtp[%d] CAN[%d]", _reqValue_oneOffMaxSoc, _canValue_oneOffMaxSoc);
    }
    else
    {
        LOGE("OneOffMaxSOC is false");
    }

    if( (bPermMaxSocValid && (_reqValue_permanentMaxSoc == _canValue_permanentMaxSoc))
        || (bOneOffMaxSocValid && (_reqValue_oneOffMaxSoc == _canValue_oneOffMaxSoc)))
    {
        LOGV("Compare Mode[%d] : Equal", compareMode);
        return E_OK;
    }
    else
    {
        LOGV("Compare Mode[%d] : Non Equal", compareMode);
        latestErrorCode += svc::ErrorCode::executionFailure_chosen;
        return svc::ErrorCode::executionFailure_chosen;
    }
}

//SRD_ReqEVC0651_v1
error_t EvChargeApp::check_ChargingModeChoice_Service(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    //svc::ChargeSettings* ngtp_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();
    const int _reqValue = *(ngtp_ChargeSettingMsg->get_chargingModeChoice());
    const int _canValue = EvtVifManager::GetInstance()->queryCanData(Signal_ChargingModeChoice_RX);
    LOG_EV("check_ChargingModeChoice_Service => NgtpReq[%d] Signal_ChargingModeChoice_RX[%d]",_reqValue, _canValue);

    if(_reqValue == _canValue)
    {
        return E_OK;
    }
    latestErrorCode += svc::ErrorCode::executionFailure_chosen;
    return svc::ErrorCode::executionFailure_chosen;
}

//SRD_ReqEVC0661_v1:
error_t EvChargeApp::check_DepartureTimers_Service(uint8_t compareMode, svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    //svc::ChargeSettings* ngtp_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();
    eDepartureTimerList _DepartureTimer_list;
    void* pOssIndex;
    uint32_t depTimerIndex;
    bool result;

    // SRD_ReqEVC0662_v3 ,SRD_ReqEVC0619_v1
    if(EvtChargeSettings::GetInstance()->bParamOutOfRange)
    {
        LOGE("[check_DepartureTimers_Service]bParamOutOfRange is true");
        return svc::ErrorCode::parameterOutOfRange_chosen;
    }

    // DepartureTimer existence Check
    if(!ngtp_ChargeSettingMsg->departureTimers_is_present())
    {
        LOGE("[check_DepartureTimers_Service] pTariff_is_present = false");
        latestErrorCode += svc::ErrorCode::executionFailure_chosen;
        return svc::ErrorCode::executionFailure_chosen;
    }

    // Compare Values between TCU - IMC
    _DepartureTimer_list = ngtp_ChargeSettingMsg->get_departureTimers()->get_timers();
    pOssIndex = _DepartureTimer_list.first();
    do{
        // pOssIndex Null Check
        if(pOssIndex == NULL)
        {
            LOGE("[check_DepartureTimers_Service] pOssIndex is NULL");
            continue;
        }

        eDepartureTimer* pDepartureTimer = _DepartureTimer_list.at(pOssIndex);
        depTimerIndex = pDepartureTimer->get_timerIndex();

        // Compare Data
        result = compareDepartureTimer(compareMode, pDepartureTimer);
        pOssIndex = _DepartureTimer_list.next(pOssIndex);
    }while( (pOssIndex != NULL) && result);

    //return (result) ? E_OK : svc::ErrorCode::executionFailure_chosen;
    if(result)
    {
        return E_OK;
    }
    else
    {
        latestErrorCode += svc::ErrorCode::executionFailure_chosen;
        return svc::ErrorCode::executionFailure_chosen;
    }
}

bool EvChargeApp::compareDepartureTimer(uint8_t compareMode, eDepartureTimer* ngtpDepartureTimer)
{
    bool result = false;
    uint32_t depTimerIndex = ngtpDepartureTimer->get_timerIndex();
    _DepartureTimer canDepartureTimer = EvtChargeSettings::GetInstance()->departureTimer_field[depTimerIndex];

    LOGV("[compareDepartureTimer] NGTP[%d]CAN[%d]", depTimerIndex, canDepartureTimer.timerIndex_field);
    eDepartureTimerDefinition* ngtpDeptTimerDef = ngtpDepartureTimer->get_departureTimerDefinition();
    _DepartureTimerDefinition canDeptTimerDef = canDepartureTimer.departureTimerDefinition_field;

    /**
      paulwj.choi : If DepartureTimerDefinition is empty(delete request)
      - COMPARE_TO_IMC : Unnecessary to compare (Return true)
      - COMPARE_TO_TSP : Unnecessary to compare (Return false to avoid ParameteralreadyRequested error)
    */
    if(!ngtpDepartureTimer->departureTimerDefinition_is_present())
    {
        if(compareMode == COMPARE_TO_IMC)
        {

            if(canDeptTimerDef.timerType_field == 0)
            {
                LOGV("[compareDepartureTimer]DeptDefinition disable.Comp[T]Idx[%d]TimerType[%d]", depTimerIndex, canDeptTimerDef.timerType_field);
                return true;
            }
            else
            {
                LOGV("[compareDepartureTimer]DeptDefinition disable.Comp[F]Idx[%d]TimerType[%d]", depTimerIndex, canDeptTimerDef.timerType_field);
                return false;
            }
            return true;
        }
        else
        {
            LOGE("[compareDepartureTimer]Del Request[%d]", depTimerIndex);
            return false;
        }
    }

    if(depTimerIndex == getCfgData("MAX_NUMBER_DEPT_TIMERS"))
    {
        if(compareMode == COMPARE_TO_IMC)
        {
            LOGE("[compareDepartureTimer]New Dept Timer. Not Necessary to compare[%d]", depTimerIndex);
            return true;
        }
        else
        {
            LOGE("[compareDepartureTimer]New Dept Timer[%d]", depTimerIndex);
            return false;
        }
    }

    //Compare DepartureTimerType
    result = compareTimerType(ngtpDeptTimerDef, canDeptTimerDef);
    if(!result)
    {
        return false;
    }

    //Compare DepartureTimer
    result = compareDepartureTime(ngtpDeptTimerDef, canDeptTimerDef);
    if(!result)
    {
        return false;
    }

    //Compare timerTarget
    result = compareTimerTarget(ngtpDeptTimerDef, canDeptTimerDef);

    return result;
}

bool EvChargeApp::compareTimerType(eDepartureTimerDefinition* ngtpDeptTimerDef, _DepartureTimerDefinition& canDeptTimerDef)
{
    bool result = false;
    eDepartureTimerType ngtpDeptTimerType = ngtpDeptTimerDef->get_timerType();
    uint8_t canTimerType = canDeptTimerDef.timerType_field;
    switch(ngtpDeptTimerType)
    {
        case svc::DepartureTimerType::DepartureTimerType_off:
            LOGV("[compareTimerType] CAN[%d] NGTP[0-DepartureTimerType_off]", canTimerType);
            result = (canTimerType == 0) ? true : false;
            break;
        case svc::DepartureTimerType::chargeOnly:
            LOGV("[compareTimerType] CAN[%d] NGTP[1-chargeOnly]", canTimerType);
            result = (canTimerType == 1) ? true : false;
            break;
        case svc::DepartureTimerType::preconditionOnly:
            LOGV("[compareTimerType] CAN[%d] NGTP[2-preconditionOnly]", canTimerType);
            result = (canTimerType == 2) ? true : false;
            break;
        case svc::DepartureTimerType::bothChargeAndPrecondition:
            LOGV("[compareTimerType] CAN[%d] NGTP[3-bothChargeAndPrecondition]", canTimerType);
            result = (canTimerType == 3) ? true : false;
            break;
        default:
            LOGE("[compareTimerType] Invalid Enum Value");
            break;
    }
    return result;
}

bool EvChargeApp::compareDepartureTime(eDepartureTimerDefinition* ngtpDeptTimerDef, _DepartureTimerDefinition& canDeptTimerDef)
{
    bool result = false;
    eDepartureTime ngtpDeptTime = ngtpDeptTimerDef->get_departureTime();
    _TimeHM canDeptTime = canDeptTimerDef.departureTime_field;

    if((ngtpDeptTime.get_hour() == canDeptTime.hour_field)
        && (ngtpDeptTime.get_minute() == canDeptTime.minute_field))
    {
        result = true;
    }

    LOGV("[compareDepartureTime]CAN h[%d]m[%d] NGTP h[%d]m[%d]"
    , canDeptTime.hour_field
    , canDeptTime.minute_field
    , ngtpDeptTime.get_hour()
    , ngtpDeptTime.get_minute()
    );
    return result;
}

bool EvChargeApp::compareTimerTarget(eDepartureTimerDefinition* ngtpDeptTimerDef, _DepartureTimerDefinition& canDeptTimerDef)
{
    bool result = false;

    eTimerTarget ngtpTimerTarget = ngtpDeptTimerDef->get_timerTarget();
    _TimerTarget canTimerTarget = canDeptTimerDef.timerTarget_field;

    // SingleDay
    if(ngtpTimerTarget.get_singleDay())
    {
        eSingleDay* ngtpSingleDay = ngtpTimerTarget.get_singleDay();
        _TimeDate canSingleDay = canTimerTarget.singleDay_field;
        if((ngtpSingleDay->get_day() == canSingleDay.day)
            &&(ngtpSingleDay->get_month() == canSingleDay.month)
            &&(ngtpSingleDay->get_year() == canSingleDay.year))
        {
            result = true;
        }
        LOGV("[compareTimerTarget] SingleDay [NGTP_CAN] d[%d_%d] m[%d_%d] y[%d_%d]"
        , ngtpSingleDay->get_day()
        , canSingleDay.day
        , ngtpSingleDay->get_month()
        , canSingleDay.month
        , ngtpSingleDay->get_year()
        , canSingleDay.year);
    }
    else
    {
        LOGE("[compareTimerTarget] get_singleDay = false");
    }

    // RepeatSchedule
    if(ngtpTimerTarget.get_repeatSchedule())
    {
        eDepartureRepeatSchedule* ngtpDeptRepSchedule = ngtpTimerTarget.get_repeatSchedule();
        _DayOfWeekSchedule canDeptRepSchedule = canTimerTarget.repeatSchedule_field;
        if(compareDayOfWeek(ngtpDeptRepSchedule->get_monday(),canDeptRepSchedule.monday_field)
            && compareDayOfWeek(ngtpDeptRepSchedule->get_tuesday(),canDeptRepSchedule.tuesday_field)
            && compareDayOfWeek(ngtpDeptRepSchedule->get_wednesday(),canDeptRepSchedule.wednesday_field)
            && compareDayOfWeek(ngtpDeptRepSchedule->get_thursday(),canDeptRepSchedule.thursday_field)
            && compareDayOfWeek(ngtpDeptRepSchedule->get_friday(),canDeptRepSchedule.friday_field)
            && compareDayOfWeek(ngtpDeptRepSchedule->get_saturday(),canDeptRepSchedule.saturday_field)
            && compareDayOfWeek(ngtpDeptRepSchedule->get_sunday(),canDeptRepSchedule.sunday_field))
        {
            result = true;
        }
        LOGV("[compareTimerTarget] RepeatSchedule[NGTP-CAN] M[%s_%s] T[%s_%s] W[%s_%s] Th[%s_%s] F[%s_%s] S[%s_%s] Su[%s_%s]"
            , ngtpDeptRepSchedule->get_monday() ? "T" : "F", canDeptRepSchedule.monday_field ? "T" : "F"
            , ngtpDeptRepSchedule->get_tuesday() ? "T" : "F", canDeptRepSchedule.tuesday_field ? "T" : "F"
            , ngtpDeptRepSchedule->get_wednesday() ? "T" : "F", canDeptRepSchedule.wednesday_field ? "T" : "F"
            , ngtpDeptRepSchedule->get_thursday() ? "T" : "F", canDeptRepSchedule.thursday_field ? "T" : "F"
            , ngtpDeptRepSchedule->get_friday() ? "T" : "F", canDeptRepSchedule.friday_field ? "T" : "F"
            , ngtpDeptRepSchedule->get_saturday() ? "T" : "F", canDeptRepSchedule.saturday_field ? "T" : "F"
            , ngtpDeptRepSchedule->get_sunday() ? "T" : "F", canDeptRepSchedule.sunday_field ? "T" : "F");
    }
    else
    {
        LOGE("[compareTimerTarget] get_repeatSchedule = false");
    }
    return result;
}

bool EvChargeApp::compareDayOfWeek(char& ngtpDay, bool canBool)
{
    return ((ngtpDay) && (canBool)) || ((!ngtpDay) && (!canBool));
}

inline bool check_ElectricityPeakTimesExtUpdate1(svc::Tariff* pTariff);
inline bool check_ElectricityPeakTimesExtUpdate2(svc::Tariff* pTariff);
inline bool check_ElectricityPeakTimesExtUpdate3(svc::Tariff* pTariff);
inline bool check_ElectricityPeakTimesExtUpdate4(svc::Tariff* pTariff);

error_t EvChargeApp::check_Tariffs_Service(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    //svc::ChargeSettings* ngtp_ChargeSettingMsg = list_Ngtp_ChargeSettings.front();
    svc::Tariffs::tariffs _Tariff_list = ngtp_ChargeSettingMsg->get_tariffs()->get_tariffs();
    void* pOssIndex = _Tariff_list.first();

    bool result = true;
    do{
        // pOssIndex should be checked if it is null or not.
        if(pOssIndex == NULL)
        {
            LOGV("pOssIndex is null");
            continue;
        }
        svc::Tariff* pTariff = _Tariff_list.at(pOssIndex);

        switch(pTariff->get_tariffIndex())
        {
            case 0:
            {
                result &= check_ElectricityPeakTimesExtUpdate1(pTariff);
                LOGV("[check_Tariffs_Service]Idx 0 [%s]", (result)?"E_OK":"Failure");
                break;
            }
            case 1:
            {
                result &= check_ElectricityPeakTimesExtUpdate2(pTariff);
                LOGV("[check_Tariffs_Service]Idx 1 [%s]", (result)?"E_OK":"Failure");
                break;
            }
            case 2:
            {
                result &= check_ElectricityPeakTimesExtUpdate3(pTariff);
                LOGV("[check_Tariffs_Service]Idx 2 [%s]", (result)?"E_OK":"Failure");
                break;
            }
            case 3:
            {
                result &= check_ElectricityPeakTimesExtUpdate4(pTariff);
                LOGV("[check_Tariffs_Service]Idx 3 [%s]", (result)?"E_OK":"Failure");
                break;
            }
            default:
            {
                LOGE("[check_Tariffs_Service] Unknown tariffIndex[%d] from NGTP", pTariff->get_tariffIndex());
                break;
            }
        }
        pOssIndex = _Tariff_list.next(pOssIndex);
    }while(pOssIndex != NULL);

    //return (result) ? E_OK : svc::ErrorCode::executionFailure_chosen;
    if(result)
    {
        return E_OK;
    }
    else
    {
        latestErrorCode += svc::ErrorCode::executionFailure_chosen;
        return svc::ErrorCode::executionFailure_chosen;
    }

}

error_t EvChargeApp::check_ChargeNowSetting_Dup(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    const uint8_t _reqValue = *(ngtp_ChargeSettingMsg->get_chargeNowSetting());
    const int _tcuValue = EvtChargeSettings::GetInstance()->chargeNowSetting_field;

    LOGV("[check_ChargeNowSetting_Dup] NGTP[%d] TCU[%d]", _reqValue, _tcuValue);
    if((_reqValue == svc::ChargeNowSetting_default) && (_tcuValue == 0))
    {
        return E_OK;
    }
    if((_reqValue == svc::force_on) && (_tcuValue == 1))
    {
        return E_OK;
    }
    if((_reqValue == svc::force_off) && (_tcuValue == 2))
    {
        return E_OK;
    }
    latestErrorCode += svc::ErrorCode::executionFailure_chosen;
    return svc::ErrorCode::executionFailure_chosen;
}

error_t EvChargeApp::check_MaxStateOfCharge_Dup(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    // Check if MaxSoc Value is valid
    bool bPermMaxSocValid = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc()) ? true : false;
    bool bOneOffMaxSocValid = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc()) ? true : false;

    bool bPermClear = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_permanentMaxSoc().get_clear()) ? true : false;
    bool bOneOffClear = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_oneOffMaxSoc().get_clear()) ? true : false;

    bool bPermNoChange = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_permanentMaxSoc().get_noChange()) ? true : false;
    bool bOneOffNoChange = (ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_oneOffMaxSoc().get_noChange()) ? true : false;

    int32_t _reqValue_permanentMaxSoc;
    int32_t _tcuValue_permanentMaxSoc;

    int32_t _reqValue_oneOffMaxSoc;
    int32_t _tcuValue_oneOffMaxSoc;
    // noChange_chosen = 1, clear_chosen = 2,maxSoc_chosen = 3
    if(bPermMaxSocValid)
    {
        _reqValue_permanentMaxSoc = *(ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_permanentMaxSoc().get_maxSoc());
        _tcuValue_permanentMaxSoc = EvtChargeSettings::GetInstance()->maxStateOfCharge_field.permanentMaxSoc_field;

        LOG_EV("[check_MaxStateOfCharge_Dup]perm Ngtp[%d] TCU[%d]",_reqValue_permanentMaxSoc, _tcuValue_permanentMaxSoc);
    }
    else if(bPermClear)
    {
        LOG_EV("[check_MaxStateOfCharge_Dup]perm Clear");
    }
    else if(bPermNoChange)
    {
        LOG_EV("[check_MaxStateOfCharge_Dup]perm No Change");
    }
    else
    {
        LOGE("[check_MaxStateOfCharge_Dup]perm is false");
    }

    if(bOneOffMaxSocValid)
    {
        _reqValue_oneOffMaxSoc = *(ngtp_ChargeSettingMsg->get_maxStateOfCharge()->get_oneOffMaxSoc().get_maxSoc());
        _tcuValue_oneOffMaxSoc = EvtChargeSettings::GetInstance()->maxStateOfCharge_field.oneOffMaxSoc_field;

        LOG_EV("[check_MaxStateOfCharge_Dup]oneOff NGTP[%d] TCU[%d]", _reqValue_oneOffMaxSoc, _tcuValue_oneOffMaxSoc);
    }
    else if(bOneOffClear)
    {
        LOG_EV("[check_MaxStateOfCharge_Dup]oneOff Clear");
    }
    else if(bOneOffNoChange)
    {
        LOG_EV("[check_MaxStateOfCharge_Dup]oneOff No Change");
    }
    else
    {
        LOGE("[check_MaxStateOfCharge_Dup]oneOff is false");
    }

    if( (bPermMaxSocValid && (_reqValue_permanentMaxSoc == _tcuValue_permanentMaxSoc))
        || (bOneOffMaxSocValid && (_reqValue_oneOffMaxSoc == _tcuValue_oneOffMaxSoc)))
    {
        return E_OK;
    }
    else if((bPermClear && EvtChargeSettings::GetInstance()->getPermanentMaxSocStatus() == 2)
        || (bOneOffClear && EvtChargeSettings::GetInstance()->getOneOffMaxSocStatus() == 2))
    {
        return E_OK;
    }
    else
    {
        latestErrorCode += svc::ErrorCode::executionFailure_chosen;
        return svc::ErrorCode::executionFailure_chosen;
    }
}

error_t EvChargeApp::check_ChargingModeChoice_Dup(svc::ChargeSettings* ngtp_ChargeSettingMsg)
{
    const int _reqValue = *(ngtp_ChargeSettingMsg->get_chargingModeChoice());
    const int _tcuValue = EvtChargeSettings::GetInstance()->chargingModeChoice_field;
    LOG_EV("[check_ChargingModeChoice_Dup] NGTP[%d] TCU[%d]",_reqValue, _tcuValue);

    if(_reqValue == _tcuValue)
    {
        return E_OK;
    }
    latestErrorCode += svc::ErrorCode::executionFailure_chosen;
    return svc::ErrorCode::executionFailure_chosen;
}

//////////////////////////////////////////////////////////// CAN Trigger EVENT - NO State ////////////////////////////////////////////////////////

///////////////// 3.6 Vehicle Notification (Push Message) ///////////////////////
//SRD_ReqEVC0040_v1 : Figure EVC8,EVC9
void EvChargeApp::rcv_HVBatteryUsableSOCDisp(Buffer& buf)          // Check CAN signal Rx
{
    if(bEVC_EVI_TIMER_ALIVE)
    {
        LOGV("[rcv_HVBatteryUsableSOCDisp]EVC_EVI_PUSH_TIMER has not expired yet - not sending ChargingStatusUpdate");
    }
    else
    {
        LOGV("[rcv_HVBatteryUsableSOCDisp]EVC_EVI_PUSH_TIMER has already expired - try to send ChargingStatusUpdate");
        sendChargingStatusByCondition();
    }
}

void EvChargeApp::rcv_WlChargingStatusDisp()
{
    if(EvConfigData::GetInstance()->configData["AppModeEVC"] != ENABLED)
    {
        LOGE("[rcv_WlChargingStatusDisp] EVC Disabled. Not Sending CSUpdate");
        return;
    }
    // send NGTP Message SRD_ReqEVC0040_v1
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE, E_OK);
    // Start EVC_EVI_PUSH_TIMER
    start_EVC_EVI_PUSH_TIMER();
}

void EvChargeApp::rcv_ChargingStatusDisp()
{
    if(EvConfigData::GetInstance()->configData["AppModeEVC"] != ENABLED)
    {
        LOGE("[rcv_ChargingStatusDisp] EVC Disabled. Not Sending CSUpdate");
        return;
    }
    // send NGTP Message SRD_ReqEVC0040_v1
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE, E_OK);
    // Start EVC_EVI_PUSH_TIMER
    start_EVC_EVI_PUSH_TIMER();
}

void EvChargeApp::rcv_ChargePwrInterruptDisp(Buffer& buf)
{
    uint8_t *canBlock = buf.data();
    EvtElectricVehicleInformation *data = EvtElectricVehicleInformation::GetInstance();
    EvtNgtpMsgManager* ngtpMgr = EvtNgtpMsgManager::GetInstance();
    if(canBlock[0] == 1 && data->chargingStatus_field == 0) //SRD_ReqEVC0049_v1:
    {
        ngtpMgr->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE_ALERT, E_OK);
    }
    LOGV("ChargePwInterruptDisp [%X]", canBlock[0]);
}

inline uint16_t getAlertDataForEVC(int32_t sigId);
bool EvChargeApp::triggerUpdateAlertCondition(int32_t sigId)
{
    uint16_t _alertData = getAlertDataForEVC(sigId);
    if(_alertData == svc::AlertData::unused_chosen)
    {
        LOGV("Not Sending CSAlert : %d", _alertData);
    }
    else
    {
        LOGV("Sending CSAlert : %d", _alertData);
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE_ALERT, _alertData);
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////

void EvChargeApp::send_ChargingStatusUpdate()           // Check NGTP Tx
{
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGING_STATUS_UPDATE, E_OK);
}

void EvChargeApp::send_ChargingSettingsUpdate()
{
    EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_UPDATE, E_OK);
}

void EvChargeApp::rcv_CAN_Frame(EvCanFrame *pEvCanFrame)
{
    LOG_EV("[Frame] rcv_CAN_Frame : %d", pEvCanFrame->mFrameID);
    m_mapFrameFunc[pEvCanFrame->mFrameID](pEvCanFrame);
}

bool EvChargeApp::DBG_ChargeNowSetting(uint8_t value)             // DBG code.
{
    if(getState() != EVC_IDLE)
    {
        LOGE("[DBG] EVC is not 'EVC_IDLE' status! (current status = [%d]).", getState());
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen);
        return false;
    }

    LOGV("[DBG] value = %d",value);
    switch(value)
    {
        case 0:
        {
            LOGV("[DBG] Send ChargeNowSetting Signal as force_off");
            sendCanSignal(Signal_ChargeNowReqExtUpdte2_TX, (uint8_t)2);
            //_chargeNowState = EVC::STOP;        // TODO: change using flag code. 20160405
            break;
        }
        case 1:
        {
            LOGV("[DBG] Send ChargeNowSetting Signal as force_on");
            sendCanSignal(Signal_ChargeNowReqExtUpdte2_TX, (uint8_t)1);
            //_chargeNowState = EVC::START;       // TODO: change using flag code. 20160405
            break;
        }
        default:
        {
            LOGE("[DBG][ERROR] Unknown DBG ChargeNowSetting Command");
            return false;
        }
    }

    // Evc Status change.
    setState(EVC_WAIT_IMC_IGNR_STATE);

    // start Timer.
    setTimer(EVC_IMC_IGNR_TIMER, getCfgData("EVC_IMC_IGNR_TIME"), true);
    LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!", getCfgData("EVC_IMC_IGNR_TIME"));

    EvtElectricVehicleInformation::GetInstance()->printData();

    return true;
}

bool EvChargeApp::DBG_ChargeModeChoice(uint8_t value)
{
    if(getState() != EVC_IDLE)
    {
        LOGE("[DBG] EVC is not 'EVC_IDLE' status! (current status = [%d]).", getState());
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen);
        return false;
    }

    LOGV("[DBG] value = %d",value);
    switch(value)
    {
        case 0:
        {
            LOGV("[DBG] Send ChargeModeChoice Signal as timedChargingNotActive");
            sendCanSignal(Signal_ChrgModeChoiceExtUpdte_TX, (uint8_t)0);
            break;
        }
        case 1:
        {
            LOGV("[DBG] Send ChargeModeChoice Signal as timedChargingActive");
            sendCanSignal(Signal_ChrgModeChoiceExtUpdte_TX, (uint8_t)1);
            break;
        }
        default:
        {
            LOGE("[DBG][ERROR] Unknown DBG ChargeModeChoice Command");
            return false;
        }
    }
    // Evc Status change.
    setState(EVC_WAIT_IMC_IGNR_STATE);

    // start Timer.
    setTimer(EVC_IMC_IGNR_TIMER, getCfgData("EVC_IMC_IGNR_TIME"), true);
    LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!", getCfgData("EVC_IMC_IGNR_TIME"));

    EvtElectricVehicleInformation::GetInstance()->printData();

    return true;
}

bool EvChargeApp::DBG_MaxStateOfCharge(uint32_t permValue, uint32_t oneOffValue)
{
    if(getState() != EVC_IDLE)
    {
        LOGE("[DBG] EVC is not 'EVC_IDLE' status! (current status = [%d]).", getState());
        EvtNgtpMsgManager::GetInstance()->send_NgtpMessage(EvtNgtpMsgManager::CHARGE_SETTING_RESP, svc::ErrorCode::parameterAlreadyInRequestedState_chosen);
        return false;
    }
    // Send CAN Signal
    sendDbgMaxSOC(1, permValue);
    sendDbgMaxSOC(2, oneOffValue);

    // Evc Status change.
    setState(EVC_WAIT_IMC_IGNR_STATE);

    // start Timer.
    setTimer(EVC_IMC_IGNR_TIMER, getCfgData("EVC_IMC_IGNR_TIME"), true);
    LOGV("EVC_IMC_IGNR_TIMER[%u]ms start!", getCfgData("EVC_IMC_IGNR_TIME"));

    EvtElectricVehicleInformation::GetInstance()->printData();

    return true;
}

bool EvChargeApp::sendDbgMaxSOC(uint8_t mode, uint32_t value)
{
    // Mode 1: permanentMaxSoc, mode 2 : oneOffMaxSoc
    value = (value == 103)? -1 : value*10;
    LOGE("sendDbgMaxSOC starting mode 1: %d value : %d", mode, value);
    if(value > 1000)
    {
        LOGE("[sendDbgMaxSOC] value out of range : %d", value);
        return false;
    }

    switch(mode)
    {
        case 1:
        {
            LOGV("[DBG] Send BulkSoCSetExtUpdate Signal as %d", value);
            uint8_t tmpdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
            tmpdata[1] = value & 0xFF;
            tmpdata[0] = (value & 0xFF00) >> 8;
            sendCanArraySignal(Signal_BulkSoCSetExtUpdate_TX, tmpdata);
            break;
        }
        case 2:
        {
            LOGV("[DBG] Send BulkSoCSingleUseExtUpd Signal as %d", value);
            uint8_t tmpdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
            tmpdata[1] = value & 0xFF;
            tmpdata[0] = (value & 0xFF00) >> 8;
            sendCanArraySignal(Signal_BulkSoCSingleUseExtUpd_TX, tmpdata);
            break;
        }
        default:
        {
            LOGE("[ERROR] Please Choose Right Mode");
            break;
        }
    }
    return true;
}

//////////////////// Private Method //////////////////////////////

void EvChargeApp::start_EVC_EVI_PUSH_TIMER()
{
    if(bEVC_EVI_TIMER_ALIVE)
    {
        // If timer is already started, cancel legacy timer and restart
        cancelTimer(EVC_EVI_PUSH_TIMER);
        setTimer(EVC_EVI_PUSH_TIMER, getCfgData("EVC_EVI_PUSH_TIME"));  // Re-trigger Timer --> EVC_EVI_PUSH_TIMER
        LOGV("EVC_EVI_PUSH_TIMER[%u]s Cancel and Re-start!", getCfgData("EVC_EVI_PUSH_TIME"));
    }
    else
    {
        bEVC_EVI_TIMER_ALIVE = true;
        setTimer(EVC_EVI_PUSH_TIMER, getCfgData("EVC_EVI_PUSH_TIME"));  // Trigger Timer --> EVC_EVI_PUSH_TIMER
        LOGV("EVC_EVI_PUSH_TIMER[%u]s start!", getCfgData("EVC_EVI_PUSH_TIME"));
    }
}

// EVC Timer.
void EvChargeApp::setTimer(uint8_t timerName, uint16_t expireTime, bool isMsTime)
{
    // Timer Setting.
    if(expireTime <= 0)
    {
        LOGE("[ERROR] Timer[%d] expireTime is invalid", timerName);
        return;
    }

    EvcTimer* t_timer = new EvcTimer(*this);
    EVC_Timers[timerName] = new Timer( t_timer, timerName );

    if(isMsTime == true)
    {
        EVC_Timers[timerName]->setDurationMs(expireTime, 0);
    }
    else
    {
        EVC_Timers[timerName]->setDuration(expireTime, 0);
    }
    EVC_Timers[timerName]->start();
}

void EvChargeApp::resetTimer(uint8_t timerName)
{
    EVC_Timers[timerName] = NULL;
}

uint32_t EvChargeApp::getRemainTimerTime(uint8_t timerName)
{
    if(EVC_Timers[timerName] == NULL)
    {
        return 0;
    }

    double _remainTime = EVC_Timers[timerName]->getRemainingTimeMs();
    LOGI("remainingRuntimeSecond = [%lf]", _remainTime);
    return _remainTime;
}
void EvChargeApp::cancelTimer(uint8_t timerName)
{
    if(getRemainTimerTime(timerName) != 0)
    {
        LOG_EV("[%d]Timer has canceled!", timerName);
        EVC_Timers[timerName]->stop();
    }
    resetTimer(timerName);
}
void EvChargeApp::cancelAllTimer()
{
    LOG_EV("Cancel All Timer!");
    for(int i=0 ; i<EVC_END_TIMER ; i++)
    {
        cancelTimer(i);
    }
}

int32_t EvChargeApp::getCfgData(const char* name)
{
    EvConfigData* _EvConfigData = EvConfigData::GetInstance();
    auto it = _EvConfigData->configData.find(name);
    if(it != _EvConfigData->configData.end())
    {
        return _EvConfigData->configData[name];
    }
    else
    {
        return getDefaultCfgValue(name);
    }
}

// GetDefault value to return default LCF value if property was not defined in LCF
int32_t EvChargeApp::getDefaultCfgValue(const char* name)
{
    if(strcmp(name, "EVC_GWM_BUSOPEN_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_GWM_BUSOPEN_TIME is setting as default value [60]s");
        return 60;
    }
    if(strcmp(name, "EVC_IMC_IGNR_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_IMC_IGNR_TIME is setting as default value [500]ms");
        return 500;
    }
    if(strcmp(name, "EVC_CHRG_PLUG_RMND_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_CHRG_PLUG_RMND_TIME is setting as default value [600]s");
        return 600;
    }
    if(strcmp(name, "EVC_DEAD_BAND_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_DEAD_BAND_TIME is setting as default value [1]s");
        return 1;
    }
    if(strcmp(name, "EVC_EVI_PUSH_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_EVI_PUSH_TIME is setting as default value [45]s");
        return 45;
    }
    if(strcmp(name, "EVC_EVI_PUSH_PERC") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_EVI_PUSH_PERC is setting as default value [1]percent");
        return 1;
    }
    if(strcmp(name, "MAX_NUMBER_DEPT_TIMERS") == 0)
    {
        LOGV("[getDefaultCfgValue] MAX_NUMBER_DEPT_TIMERS is setting as default value [50]");
        return 50;
    }
    if(strcmp(name, "EVC_IMC_WKUP_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_IMC_WKUP_TIME is setting as default value [10]s");
        return 10;
    }
    if(strcmp(name, "EVC_IMC_TF2_TIME") == 0)
    {
        LOGV("[getDefaultCfgValue] EVC_IMC_TF2_TIME(EVC_IMC_FT_TIME) is setting as default value [40]s");
        return 40;
    }
    if(strcmp(name, "EVR_EVI_LOC_PUSH_EN") == 0)
    {
        LOGV("[getDefaultCfgValue] EVR_EVI_LOC_PUSH_EN is setting as default value [1]");
        return 1;
    }
    LOGE("[ERROR] <%s> is not defined in Configure Data!! ", name);
    return -1;
}

error_t EvChargeApp::sendCanSignal(int sigID, const uint8_t data)
{
    return EvtVifManager::GetInstance()->sendCanSignal(sigID, data);
}

error_t EvChargeApp::sendCanArraySignal(int sigID, const uint8_t data[])
{
    return EvtVifManager::GetInstance()->sendCanArraySignal(sigID, data);
}

// void EvChargeApp::pushSingleData(sp<vifCANContainer>& sigdata, int sigID, uint8_t data)
// {
//     EvtVifManager::GetInstance()->pushSingleData(sigdata, sigID, data);
// }

void EvChargeApp::toStringState(unsigned char state, unsigned char newState)
{
    std::string EvAppState[EVC_END_ENUM];
    EvAppState[EVC_OFF]                     = "EVC_OFF";
    EvAppState[EVC_IDLE]                    = "EVC_IDLE";
    EvAppState[EVC_WAIT_WAKEUP_STATE]       = "EVC_WAIT_WAKEUP_STATE";
    EvAppState[EVC_WAIT_IMC_IGNR_STATE]     = "EVC_WAIT_IMC_IGNR_STATE";
    EvAppState[EVC_WAIT_CAN_SIGNAL_STATE]   = "EVC_WAIT_CAN_SIGNAL_STATE";
    EvAppState[EVC_FIRST_TIME_STATE]        = "EVC_FIRST_TIME_STATE";

    if(state < 0 && state >= EVC_END_ENUM)
    {
        return ;
    }

    if(newState < 0 && newState >= EVC_END_ENUM)
    {
        return ;
    }

    LOGE("EvChargeApp state is changed : [%s] => [%s]", EvAppState[state].c_str(), EvAppState[newState].c_str());
}

uint32_t EvChargeApp::getValueWithLimit(uint32_t value, uint32_t minLimit, uint32_t maxLimit, uint32_t defaultValue)
{
    uint32_t val;
    if((value > maxLimit) || (minLimit > value))
    {
        LOGE("[getValueWithLimit] out of range val[%d] min[%d] max[%d] defau[%d]", value, minLimit, maxLimit, defaultValue);
        val = defaultValue;
    }
    else
    {
        val = value;
    }
    return val;
}

// SRD_ReqEVC0613_v1
bool EvChargeApp::isConflictWithOnBoardChange()
{
    uint32_t curTimeStamp = UTIL::get_NGTP_time();
    uint32_t timeDiff = curTimeStamp - EvtChargeSettings::GetInstance()->getDeadBandTimeStamp();

    bool result = (timeDiff > EvConfigData::GetInstance()->getEvCfgData("EVC_DEAD_BAND_TIME")) ? false : true;

    LOGV("[isConflictWithOnBoardChange]result[%s]DEAD_BAND_TIME[%d]dif[%d]cur[%u]last[%u]"
    , result ? "T" : "F"
    , EvConfigData::GetInstance()->getEvCfgData("EVC_DEAD_BAND_TIME")
    , timeDiff
    , curTimeStamp
    , EvtChargeSettings::GetInstance()->getDeadBandTimeStamp());
    return result;
}

void EvChargeApp::triggerECWstate()
{
    try{
        if(EvVehicleStatus::GetInstance()->getMdmVariant() != VARI_MY18_HIGH)
        {
            throw "Board Varient is not HIGH variant";
        }

        if(EvConfigData::GetInstance()->getConfigData("ECW_CONFIG_ENABLE") != 1)
        {
            //SRD_ReqECW0002_v1
            throw "[SRD_ReqECW0002_v1]ECW_CONFIG_ENABLE is not enabled!";
        }

        if(EvVehicleStatus::GetInstance()->getCurPowerMode() >= 7)
        {
            //SRD_ReqECW0003_v1
            throw "[SRD_ReqECW0003_v1]PowerMode >= 7";
        }
    }
    catch(const char* e)
    {
        LOG_EV("Fail to trigger ECW: %s", e);
        return;
    }

    bool b_IsCharging = false;
    if( EvtVifManager::GetInstance()->queryCanData(Signal_ChargingStatusDisp_RX) == 3 ||
       EvtVifManager::GetInstance()->queryCanData(Signal_WlChargingStatusDisp_RX) ==3 )
    {
        b_IsCharging = true;
    }

    bool b_eCallStatus = EvVehicleStatus::GetInstance()->IsEmergencyActivated();
    bool b_bCallStatus = EvVehicleStatus::GetInstance()->IsBCallActivated();
    bool b_SVTstatus = EvVehicleStatus::GetInstance()->IsSVTActivated();
    LOG_EV("ECW status = [%d][%d][%d][%d]",b_IsCharging,b_eCallStatus,b_bCallStatus,b_SVTstatus);
    if( b_IsCharging == true &&
       b_eCallStatus == false &&
       b_bCallStatus == false &&
       b_SVTstatus == false)
    {
        trigger_setEcwState(ECW_ACTIVE);
    }
    else
    {
        trigger_setEcwState(ECW_DEACTIVE);
    }
}

// Active/Deactive ECW.
static uint8_t ECWstatus = ECW_DEACTIVE;
void EvChargeApp::trigger_setEcwState(uint8_t cmd)
{
    if(ECWstatus == cmd)
    {
        LOG_EV("current ECWstatus is %d",cmd);
        return;
    }

    switch(cmd)
    {
        case ECW_ACTIVE:
            LOG_EV("Req active ECW");
            EvtNgtpMsgManager::GetInstance()->activeECW();
            ECWstatus = ECW_ACTIVE;
            break;
        case ECW_DEACTIVE:
            LOG_EV("Req deactive ECW");
            EvtNgtpMsgManager::GetInstance()->deactiveECW();
            ECWstatus = ECW_DEACTIVE;
            break;
        default:
            LOGE("[%s] received Unknown command[%d] ", __func__, cmd );
            break;
    }
}

bool EvChargeApp::isTimeToIgnoreChange()
{
    if(getState() == EVC_WAIT_IMC_IGNR_STATE)
    {
        LOGE("IgnoreChange From IMC");
        return true;
    }
    else
    {

        return false;
    }
    return (getState() == EVC_WAIT_IMC_IGNR_STATE) ?  true : false;
}

void EvChargeApp::setEVCState(unsigned char newState)
{
    setState(newState);
}

void EvChargeApp::initializeCompFlags()
{
    bool bCompChargeNow = false;
    bool bCompChargingMode = false;
    bool bCompMaxSOC = false;
    bool bCompTariff = false;
    bool bCompDeptTimer = false;
}

void EvChargeApp::restartTimer(uint8_t timerName, uint16_t expireTime, bool isMsTime)
{
    cancelTimer(timerName);
    setTimer(timerName, expireTime, isMsTime);
}
#include "EvChargeApp.inl"
