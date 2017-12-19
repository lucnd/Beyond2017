#include "EvtNgtpMsgManager.h"
#include "EvtChargeSettings.h"
#include "EvtEVPreconditionStatus.h"
#include "EvtEVPreconditionStartConfiguration.h"
#include "EvtNextDepartureTimer.h"
#include "EvtHvBatteryPack.h"
#include "EvChargeApp.h"
#include "EvClimateApp.h"
#include "EvtUtil/EvtUtil.h"

typedef svc::ChargeSettings::departureTimers EvDepartureTimers;
typedef svc::ChargeSettings::departureTimers::timers EvDepartureTimerList;
typedef svc::ChargeSettings::departureTimers::timers::component EvDepartureTimer;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition EvDepartureTimerDefinition;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerType EvDepartureTimerType;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::departureTime EvDepartureTime;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerTarget EvTimerTarget;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerTarget::singleDay EvSingleDay;
typedef svc::ChargeSettings::departureTimers::timers::component::departureTimerDefinition::timerTarget::repeatSchedule EvDepartureRepeatSchedule;

#define LOG_TAG "[EVT]Ngtp"
#include "Log.h"

EvtNgtpMsgManager::EvtNgtpMsgManager()
{
    ngtp_send_type = NGTP_UDP;
    memset(evt_ngtp_sms_number, 0, sizeof(evt_ngtp_sms_number));
    strncpy(evt_ngtp_sms_number, "310000202", sizeof(evt_ngtp_sms_number));
    bLoggingNgtpDecoder = false;
}

void EvtNgtpMsgManager::send_NgtpMessage(uint8_t ngtpType, uint8_t ngtpResult)
{
    switch(ngtpType)
    {
        //EV Charge
        case CHARGE_SETTING_RESP:
        {
            sendNgtp_ChargeSettingResp(ngtpType, ngtpResult);
            break;
        }
        case CHARGE_SETTING_UPDATE:
        case CHARGING_STATUS_UPDATE:
        {
            // SRD_ReqEVC0019 - In Cas of EVC Disable, EVC will not do anything
            if(EvConfigData::GetInstance()->configData["AppModeEVC"] == ENABLED)
            {
                sendNgtp_ElectricVehicleInformation(ngtpType, ngtpResult);
            }
            else
            {
                LOGE("EVC Disabled. Not Sending CSUpdate type[%d]", ngtpType);
            }
            break;
        }
        case CHARGING_STATUS_UPDATE_ALERT:
        {
	    	// SRD_ReqEVC0019 - In Cas of EVC Disable, EVC will not do anything
            if(EvConfigData::GetInstance()->configData["AppModeEVC"] == ENABLED)
            {
                sendNGTP_Alerts(ngtpType, ngtpResult);
            }
            else
            {
                LOGE("EVC Disabled. Not Sending CSUpdate Alert type[%d]", ngtpType);
            }
            break;
        }
        //EV Climate (Precondition)
        case PRECONDITION_SETTING_RESP:
        {
            sendNgtp_PreconditionSettingResp(ngtpType, ngtpResult);
            break;
        }

        case PRECONDITION_START_RESP:
        case PRECONDITION_STOP_RESP:
        {
            sendNgtp_PreconditionStartStopResp(ngtpType, ngtpResult);
            break;
        }
        case PRECONDITON_UPDATE_ALERT:
        {
            sendNgtp_PreconditionUpdateAlerts(ngtpType, ngtpResult);
            break;
        }
        case PRECONDITION_SETTING_UPDATE:
        {
            sendNgtp_PreconditionSettingUpdate(ngtpType);
            break;
        }
        case BATTERY_INFORMATION_UPDATE:
        {
            sendNgtp_BatteryInformationUpdate(ngtpType);
        }
        default:
        {
            LOGE("[ERROR] Unknown Ngtp request type [%d]", ngtpType);
            break;
        }
    }
}

void EvtNgtpMsgManager::rcv_NgtpMessage(Buffer& buf)
{
    //InternalNGTPData *pNgtp = static_cast<InternalNGTPData*>(buf.data());
    InternalNGTPData *pNgtp = (InternalNGTPData*)buf.data();

    svc::NGTPServiceData* pServiceData;

////////////////////////// QA4 Test Code /////////////////////////////
    uint32_t _msgCreationTime = pNgtp->messageCreationTime_seconds;
/*
    if(IsValidNgtpMsg(pNgtp, pServiceData) == false)
    {
        LOGE("[ERROR] Received invalid NGTP message.");
        return ;
    }


    LOGV("Before get_serviceDataCoreMessage()!");
    if(pServiceData == NULL)
    {
        LOGE("E1");
    }
    if(pServiceData->get_serviceDataCoreMessage() == NULL)
    {
        LOGE("E2");
    }
*/
    if(pNgtp->encodedSize <= 0)
    {
        LOGE("[ERROR] NGTP validation has failed. Encoded Message size is invalid.");
        return ;
    }

    tsdp_svc_Control ctl;
    svc::NGTPServiceData_PDU  outPDU;
    ctl.setDecodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);

    //decode for validation
    PEREncodedBuffer encodedSvcData;
    encodedSvcData.set_buffer(pNgtp->encodedSize, (char*)pNgtp->encodedSvcData);
    outPDU.decode ( ctl, encodedSvcData);

    pServiceData = outPDU.get_data();
    if(pServiceData == NULL)
    {
        LOGE("ServiceData is NULL, Decoding FAILED");
        return ;
    }

    mMessageType = pServiceData->get_messageType();
    if(pServiceData->appAck_is_present())
    {
        mAppAck = *(pServiceData->get_appAck());
        LOGV("appAck = %d", mAppAck);
    }
    // LOGV("messagetype = %d", mMessageType);

    if(!pServiceData->serviceDataCoreMessage_is_present())
    {
        LOGE("[ERROR] This NGTP has no 'serviceDataCoreMessage' : serviceDataCoreMessage_is_present()==false");
        return ;
    }
/////////////////////////////////////////////////////////////////////
    svc::NGTPDownlinkCore *pDownlink = pServiceData->get_serviceDataCoreMessage()->get_downlink();
    // LOGV("After get_serviceDataCoreMessage()!");

    EvChargeApp* mEvc = EvChargeApp::GetInstance();

    // Message Type : ChargeSettings Request.
    if(pDownlink->chargeSettings_is_present())
    {
        LOGI("Receive ChargeSettings NGTP Msg.");
        //set Event Creation Time.
        setEventCreationTime(CHARGE_SETTINGS, pNgtp);

        svc::ChargeSettings* mChargeSettings = pDownlink->get_chargeSettings();
        // It should be operated after error check finishing
        //EvtElectricVehicleInformation::GetInstance()->setChargeSetting(*mChargeSettings);

        //decodeMsg_ChargeSettings(mChargeSettings, _msgCreationTime);
        EvChargeApp* mEvc = EvChargeApp::GetInstance();
        mEvc->rcv_NgtpMessage(mChargeSettings, _msgCreationTime);
    }

    // Message Type : PreconditionStart Request.
    if(pDownlink->preconditionStart_is_present())
    {
        LOGI("Receive PreconditionStart NGTP Msg.");

        //set Event Creation Time.
        setEventCreationTime(PRECND_START, pNgtp);

        svc::EVPreconditionStartConfiguration *mPreconditionStart = pDownlink->get_preconditionStart();

        //SRD_ReqECC0040
        uint32_t _targetTemperatureCelsiusX10 = 0;
        if(mPreconditionStart->targetTemperatureCelsiusX10_is_present())
        {
            _targetTemperatureCelsiusX10 = *(mPreconditionStart->get_targetTemperatureCelsiusX10());
        }
        EvClimateApp::GetInstance()->rcv_PreconditionStartReq(_targetTemperatureCelsiusX10);

    }

    // Message Type : PreconditionStop Request.
    if(pDownlink->preconditionStop_is_present())
    {
        LOGI("Receive PreconditionStop NGTP Msg.");
        //set Event Creation Time.
        setEventCreationTime(PRECND_STOP, pNgtp);

        svc::EVPreconditionStopConfiguration *mPreconditionStop = pDownlink->get_preconditionStop();
        EvClimateApp* mEcc = EvClimateApp::GetInstance();
        mEcc->rcv_PreconditionStopReq();
    }

    // Message Type : PreconditionSetting Requset.
    if(pDownlink->preconditionSettings_is_present())
    {
        LOGI("Receive PreconditionSettings NGTP Msg.");

        //set Event Creation Time.
        setEventCreationTime(PRECND_SETTING, pNgtp);

        svc::EVPreconditionSettings *mPreconditionSettings = pDownlink->get_preconditionSettings();

        int8_t _fuelFiredHeaterSetting = -1;
        int8_t _prioritySetting = -1;

        if(mPreconditionSettings->fuelFiredHeaterSetting_is_present())
        {
            if(EvVehicleStatus::GetInstance()->getVehicleType() != EvVehicleStatus::PHEV)
            {
                LOGE("[ERROR] WRONG PreconditionSetting NGTP Reqeust. This Vehicle is not PHEV. And It is not able to handle fuelFiredHeaterSetting reqeust.");
                send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, svc::ErrorCode::invalidData_chosen);
                return;
            }
            _fuelFiredHeaterSetting = *(mPreconditionSettings->get_fuelFiredHeaterSetting());
        }

        if(mPreconditionSettings->prioritySetting_is_present())
        {
            _prioritySetting = *(mPreconditionSettings->get_prioritySetting());
        }

        if(!mPreconditionSettings->fuelFiredHeaterSetting_is_present() && !mPreconditionSettings->prioritySetting_is_present())
        {
            LOGE("[ERROR] WRONG PreconditionSetting NGTP Reqeust. No Data!");
            send_NgtpMessage(EvtNgtpMsgManager::PRECONDITION_SETTING_RESP, svc::ErrorCode::invalidData_chosen);
            return;
        }

        //decodeMsg_PreconditionSettings(_fuelFiredHeaterSetting, _prioritySetting, _msgCreationTime); --> 20160905 delayed decoding.
        EvClimateApp::GetInstance()->rcv_PreconditionSettingReq(_fuelFiredHeaterSetting, _prioritySetting, _msgCreationTime);
    }
}

void EvtNgtpMsgManager::setLoggingNgtpDecoder(bool bState)
{
    if(bState == ENABLED)
    {
        LOGI("LoggingNgtpDecoder is ENABLED!");
        bLoggingNgtpDecoder = true;
    }
    else
    {
        LOGI("LoggingNgtpDecoder is DISABLED!");
        bLoggingNgtpDecoder = false;
    }
}

//////////////////////// Private Method //////////////////////////////
void EvtNgtpMsgManager::setEventCreationTime(uint8_t msgType, InternalNGTPData *pNgtp)
{
    if((msgType < EVT_MSG_ENUM_END && msgType > UNKNOWN_MSG) == false)
    {
        LOGE("[WARN] unknown msgType! - Unavilalbe to store ECT!");
        return ;
    }

    EventCreationTime _ect;
    _ect["second"]      = pNgtp->creationTime_seconds;
    _ect["milisecond"]  = pNgtp->creationTime_millis;
    mArrayECT[msgType].push_back(_ect);

    LOG_EV("Push NGTP ECT: type[%d] ECT[%d:%d] Index[%d] ", msgType,
                                                        _ect["second"], _ect["milisecond"],
                                                        mArrayECT[msgType].size());
}

EventCreationTime EvtNgtpMsgManager::getEventCreationTime(uint8_t msgType)
{
    EventCreationTime _ect;
    if(mArrayECT[msgType].empty())
    {
        LOG_EV("Set ECT as current Time");
        _ect["second"]      = UTIL::get_NGTP_time();

        struct timeval tm;
        uint32_t msec;
        gettimeofday(&tm, NULL);
        msec = tm.tv_usec/1000;
        _ect["milisecond"]  = msec;
        return _ect;
    }

    _ect = mArrayECT[msgType].back();
    mArrayECT[msgType].pop_back();
    LOG_EV("Pop NGTP ECT: type[%d] ECT[%d:%d] Index[%d] ", msgType,
                                                        _ect["second"], _ect["milisecond"],
                                                        mArrayECT[msgType].size());
    return _ect;
}

inline void initializeNgtpUplinkData(svc::NGTPUplinkCore &uplink);
inline void setInternalNGTPData(InternalNGTPData &internalNGTPData, PEREncodedBuffer &encodedSvcData, uint8_t ngtp_send_type = EvtNgtpMsgManager::NGTP_UDP, char* evt_ngtp_sms_number = NULL);
inline uint8_t getNgtpDispatcherServiceType(uint8_t typeUplinkMsg);
inline void setNgtpUplinkData_errorCodes(svc::NGTPUplinkCore &uplink, uint8_t ngtpResult);
inline void setEvErrorCodes(OssIndex &tErrorIdx, svc::NGTPUplinkCore::errorCodes &tErrCodes, uint8_t ngtpResult);
inline void setNgtpUplinkData_ElectricVehicleInformation(svc::NGTPUplinkCore &uplink);

void EvtNgtpMsgManager::sendNgtp_ChargeSettingResp(uint8_t typeUplinkMsg, uint8_t ngtpResult)
{
    // ChargeSettingResponse Errorcodes are not triggered by IMC. So It shouldn't be ignored on IMC_IGNR_STATE.
    if(EvChargeApp::GetInstance()->isTimeToIgnoreChange() && ngtpResult == E_OK)
    {
        LOGE("[sendNgtp_ChargeSettingResp] IMC_IGNR_STATE and No ErrorCode - Not Encoding Message");
        return;
    }

    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);

    PEREncodedBuffer encodedSvcData;
    bool result;

    setNgtpUplinkData_ElectricVehicleInformation(uplink);                       // Message dependancy.
    if(ngtpResult != E_OK)
    {
        setNgtpUplinkData_errorCodes(uplink, ngtpResult);                           //   Message dependancy.
        result = encodeNgtpServiceData(encodedSvcData, uplink, svc::acknowledge, svc::nack, false); // Message dependancy.
    }
    else
    {
        result = encodeNgtpServiceData(encodedSvcData, uplink, svc::acknowledge, svc::ack, false); // Message dependancy.
    }

    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = _serviceType;                            // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.

    EventCreationTime _ect = getEventCreationTime(CHARGE_SETTINGS);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;
    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = false;

    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
}

inline void setNgtpUplinkData_preconditionStatus(svc::NGTPUplinkCore &uplink, uint8_t ngtpResult);
void EvtNgtpMsgManager::sendNgtp_PreconditionSettingResp(uint8_t typeUplinkMsg, uint8_t ngtpResult)
{
    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);

    PEREncodedBuffer encodedSvcData;
    bool result;

    ////////////////////////////// EVPreconditionStatus ASN Spec /////////////////////////////////////
    // EVPreconditionStatus ::= SEQUENCE
    // {
    //     -- Always include whether the preconditioning is off, timed or immediate.
    //     preconditioningMode          EVPreconditionMode,
    //
    //     -- Indicating the operating status of the EV powered HVAC operation
    //     preconditionOperatingStatus  EVPreconditionOperatingStatus,
    //
    //     -- The number of minutes that remains of the runtime of the EV powered HVAC operation
    //     -- Only included in for immediate preconditon. -1=unknown
    //     remainingRuntimeMinutes      INTEGER (-1..254) OPTIONAL,
    //
    //     -- Always include whether the HV Battery is preconditioning
    //     batteryPreconditioningStatus EVBatteryPreconditioningStatus,
    //
    //     -- Included when settings were changed from vehicle side or when TSP requests vehicle status
    //     preconditionSettings         EVPreconditionSettings OPTIONAL
    // }
    //////////////////////////////////////////////////////////////////////////////////////////////////

    setNgtpUplinkData_preconditionStatus(uplink, ngtpResult);
    if(ngtpResult != E_OK)
    {
        setNgtpUplinkData_errorCodes(uplink, ngtpResult);                           //   Message dependancy.
        result = encodeNgtpServiceData(encodedSvcData, uplink, svc::acknowledge, svc::nack, false); // Message dependancy.
    }
    else // Success Case
    {
        result = encodeNgtpServiceData(encodedSvcData, uplink, svc::acknowledge, svc::ack, false); // Message dependancy.
    }

    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = _serviceType;                            // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.

    EventCreationTime _ect = getEventCreationTime(PRECND_SETTING);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;

    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = false;

    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
}


inline void encode_energyConsumedLastChargekWh(svc::ElectricVehicleInformation &evInfo);
inline void encode_chargingInformation(svc::ElectricVehicleInformation &evInfo);
inline void encode_batteryStatus(svc::ElectricVehicleInformation &evInfo);
inline void encode_chargingStatus(svc::ElectricVehicleInformation &evInfo);
inline void encode_wlChargingStatus(svc::ElectricVehicleInformation &evInfo);
inline void encode_chargeSetting(svc::ElectricVehicleInformation &evInfo);
inline void encode_evRangeMapInformation(svc::ElectricVehicleInformation &evInfo);
inline void encode_maxStateOfCharge(svc::ChargeSettings &evChargeSettings);
inline void encode_PermanentMaxSoc(svc::MaxStateOfCharge& evMaxSoc );
inline void encode_OneOffMaxSoc(svc::MaxStateOfCharge& evMaxSoc);
inline void encode_Tariffs(svc::ChargeSettings &evChargeSettings);
inline void encode_Tariff(svc::Tariffs::tariffs& ngtp_TariffList);
inline void encode_TariffDefinition(svc::Tariff& ngtp_Tariff, _Tariff& evt_tariff);
inline void encode_RepeatSchedule(svc::TariffDefinition& evTariffDefinition, _TariffDefinition& _tariffDefinition);
inline void encode_TariffZone(svc::TariffDefinition& evTariffDefinition, _TariffDefinition& _tariffDefinition);
inline void encode_TariffEndTime(svc::TariffZone& evTariffZone, _TariffZone& _tariffZone);
inline void encode_DepartureTimer(EvDepartureTimerList& evDepartureTimerList);
inline void encode_DepartureTimers(svc::ChargeSettings &evChargeSettings);
inline void encode_DepartureTimerDefinition(EvDepartureTimer& evDepartureTimer, _DepartureTimer& _departureTimer);
inline void encode_timerTarget(EvDepartureTimerDefinition& evDepartureTimerDefinition, _DepartureTimerDefinition& _departureTimerDefinition);
inline void encode_nextDepartureTimer(svc::ElectricVehicleInformation &evInfo);
inline void encode_evRangePredicStatus(svc::ElectricVehicleInformation &evInfo);

void EvtNgtpMsgManager::sendNgtp_ElectricVehicleInformation(uint8_t typeUplinkMsg, uint8_t ngtpResult)
{
    if(EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    {
        LOGE("[sendNgtp_ElectricVehicleInformation] IMC_IGNR_STATE - Not Encoding Message");
        return;
    }

    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);
    PEREncodedBuffer encodedSvcData;

    setNgtpUplinkData_ElectricVehicleInformation(uplink);                       // Message dependancy.
    bool _bOmitAppAck = ((typeUplinkMsg == CHARGE_SETTING_UPDATE) || (typeUplinkMsg== CHARGING_STATUS_UPDATE)) ? true : false;
    LOGV("sendNgtp_EVI omitAppAck[%s]", _bOmitAppAck ? "T" : "F");

    bool result = encodeNgtpServiceData(encodedSvcData, uplink, svc::update, svc::ack, _bOmitAppAck); // Message dependancy.
    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;

    //Set ECT Time.
    EventCreationTime _ect = getEventCreationTime(UNKNOWN_MSG);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;
    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = true; // true

    internalNGTPData.serviceType = _serviceType;               // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.
    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));

}

//Alert
inline void setNgtpUplinkData_Alerts(svc::NGTPUplinkCore &uplink, uint8_t alertType);
void EvtNgtpMsgManager::sendNGTP_Alerts(uint8_t typeUplinkMsg, uint8_t ngtpResult)
{
    if(EvChargeApp::GetInstance()->isTimeToIgnoreChange())
    {
        LOGE("[sendNGTP_Alerts] IMC_IGNR_STATE - Not Encoding Message");
        return;
    }

    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);
    PEREncodedBuffer encodedSvcData;

    setNgtpUplinkData_Alerts(uplink, ngtpResult);
    setNgtpUplinkData_ElectricVehicleInformation(uplink);                       // Message dependancy.
    bool result = encodeNgtpServiceData(encodedSvcData, uplink, svc::update, svc::nack, true); // Message dependancy.
    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    //Set ECT Time.
    EventCreationTime _ect = getEventCreationTime(UNKNOWN_MSG);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;
    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = true; //true

    internalNGTPData.serviceType = _serviceType;               // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.
    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));

}

void EvtNgtpMsgManager::setNgtpUplinkData_HighVoltageBatteryPack(svc::NGTPUplinkCore &uplink)
{
    svc::HighVoltageBatteryPack _hvbp;

    encode_BatteryPackDataHeader(_hvbp);
    encode_HighVoltageDataRecord(_hvbp);

    uplink.set_highVoltageBatteryPack(_hvbp);
}

inline void setNgtpUplinkData_vehicleStatus(svc::NGTPUplinkCore &uplink);
inline void setNgtpUplinkData_climateStatus(svc::NGTPUplinkCore &uplink);

void EvtNgtpMsgManager::sendNgtp_PreconditionStartStopResp(uint8_t typeUplinkMsg, uint8_t ngtpResult)
{
    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);

    PEREncodedBuffer encodedSvcData;
    bool result;
    setNgtpUplinkData_vehicleStatus(uplink);
    setNgtpUplinkData_climateStatus(uplink);
    setNgtpUplinkData_preconditionStatus(uplink, 0xFF);

    if(ngtpResult != E_OK)
    {
        setNgtpUplinkData_errorCodes(uplink, ngtpResult);                       // Message dependancy.
        result = encodeNgtpServiceData(encodedSvcData, uplink, svc::acknowledge, svc::nack, false); // Message dependancy.
    }
    else
    {
        result = encodeNgtpServiceData(encodedSvcData, uplink, svc::acknowledge, svc::ack, false); // Message dependancy.
    }

    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = _serviceType;                            // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.

    EventCreationTime _ect = getEventCreationTime((typeUplinkMsg == PRECONDITION_START_RESP) ? PRECND_START : PRECND_STOP);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;

    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = false;

    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
}

void EvtNgtpMsgManager::sendNgtp_PreconditionSettingUpdate(uint8_t typeUplinkMsg)
{
    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);
    PEREncodedBuffer encodedSvcData;
    setNgtpUplinkData_preconditionStatus(uplink, 0xFF);

    bool result = encodeNgtpServiceData(encodedSvcData, uplink, svc::update, svc::nack, true); // Message dependancy.

    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = _serviceType;               // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.

    EventCreationTime _ect = getEventCreationTime(UNKNOWN_MSG);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;

    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = true; // true

    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
}

void EvtNgtpMsgManager::sendNgtp_PreconditionUpdateAlerts(uint8_t typeUplinkMsg, uint8_t ngtpResult)
{
    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);
    PEREncodedBuffer encodedSvcData;

    setNgtpUplinkData_Alerts(uplink, ngtpResult);
    setNgtpUplinkData_vehicleStatus(uplink);
    setNgtpUplinkData_climateStatus(uplink);
    setNgtpUplinkData_preconditionStatus(uplink, 0xFF);

    bool result = encodeNgtpServiceData(encodedSvcData, uplink, svc::update, svc::nack, true); // Message dependancy.

    if(result == false)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = _serviceType;               // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.

    EventCreationTime _ect = getEventCreationTime(UNKNOWN_MSG);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;

    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = true; // true

    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
}

void EvtNgtpMsgManager::sendNgtp_BatteryInformationUpdate(uint8_t typeUplinkMsg)
{
    LOGV("[sendNgtp_BatteryInformationUpdate]");
    uint8_t _serviceType = getNgtpDispatcherServiceType(typeUplinkMsg);
    svc::NGTPUplinkCore uplink;
    initializeNgtpUplinkData(uplink);
    PEREncodedBuffer encodedSvcData;

    setNgtpUplinkData_HighVoltageBatteryPack(uplink);
    bool result = encodeNgtpServiceData(encodedSvcData, uplink, svc::update, svc::ack, false); // Message dependancy.

    if(!result)
    {
        LOGE("[ERROR] Sent NGTP failure.");
        return ;
    }

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = _serviceType;               // Message dependancy.
    internalNGTPData.messageType = dspt::upServiceData;                     // Message dependancy.

    EventCreationTime _ect = getEventCreationTime(UNKNOWN_MSG);
    internalNGTPData.creationTime_seconds = _ect["second"];    // ECT second;
    internalNGTPData.creationTime_millis = _ect["milisecond"];     // ECT millisec;

    //Set stateless NGTP.
    internalNGTPData.statelessNGTPmessage = true; // true

    setInternalNGTPData(internalNGTPData, encodedSvcData, ngtp_send_type, evt_ngtp_sms_number);
    m_ngtpMgrService->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
}

bool EvtNgtpMsgManager::IsValidNgtpMsg(InternalNGTPData *pNgtp, svc::NGTPServiceData* pServiceData)
{
    if(pNgtp->encodedSize <= 0)
    {
        LOGE("[ERROR] NGTP validation has failed. Encoded Message size is invalid.");
        return false;
    }

    tsdp_svc_Control ctl;
    svc::NGTPServiceData_PDU  outPDU;
    ctl.setDecodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);

    //decode for validation
    PEREncodedBuffer encodedSvcData;
    encodedSvcData.set_buffer(pNgtp->encodedSize, (char*)pNgtp->encodedSvcData);
    outPDU.decode ( ctl, encodedSvcData);

    pServiceData = outPDU.get_data();
    if(pServiceData == NULL)
    {
        LOGE("ServiceData is NULL, Decoding FAILED");
        return false;
    }

    mMessageType = pServiceData->get_messageType();
    if(pServiceData->appAck_is_present())
    {
        mAppAck = *(pServiceData->get_appAck());
        LOGV("appAck = %d", mAppAck);
    }
    LOGV("messagetype = %d", mMessageType);

    if(!pServiceData->serviceDataCoreMessage_is_present())
    {
        LOGE("[ERROR] This NGTP has no 'serviceDataCoreMessage' : serviceDataCoreMessage_is_present()==false");
        return false;
    }
    return true;
}

void EvtNgtpMsgManager::decodeMsg_PreconditionSettings(int8_t _fuelFiredHeaterSetting, int8_t _prioritySetting, uint32_t _msgCreationTime)
{
    EvtEVPreconditionSettings *newMsg       = EvtEVPreconditionSettings::GetInstance();
    newMsg->fuelFiredHeaterSetting_field    = _fuelFiredHeaterSetting;
    newMsg->prioritySetting_field           = _prioritySetting;
    newMsg->CreationTimeStamp               = _msgCreationTime;
}

void EvtNgtpMsgManager::decodeMsg_PreconditionStart(uint32_t _targetTemperatureCelsiusX10, uint32_t _msgCreationTime)
{
    EvtEVPreconditionStartConfiguration *newMsg = EvtEVPreconditionStartConfiguration::GetInstance();
    newMsg->targetTemperatureCelsiusX10_field    = _targetTemperatureCelsiusX10;
    newMsg->CreationTimeStamp                   = _msgCreationTime;
}

void EvtNgtpMsgManager::activeECW()
{
    m_ngtpMgrService->startWifiTopsMode();
}
void EvtNgtpMsgManager::deactiveECW()
{
    m_ngtpMgrService->stopWifiTopsMode();
}

inline void initializeNgtpUplinkData(svc::NGTPUplinkCore &uplink)
{
    uplink.omit_positionExtension();
    uplink.omit_dashboardStatus();
    uplink.omit_climateStatus();                // Ev App.
    uplink.omit_lastGPSTimeStamp();
    uplink.omit_vin();
    uplink.omit_navigationInformation();
    uplink.omit_memoryStatus();
    uplink.omit_vehiclePropulsionType();
    uplink.omit_remoteOperationResponse();
    uplink.omit_genericText();
    uplink.omit_genericParameters();
    uplink.omit_alerts();
    uplink.omit_journeys();
    uplink.omit_msdInfo();
    uplink.omit_debugInfo();
    uplink.omit_errorCodes();                   // Ev App.
    uplink.omit_rismAcknowledge();
    uplink.omit_rismResponse();
    uplink.omit_rismUpdate();
    uplink.omit_electricVehicleInformation();   // Ev App.
    uplink.omit_preconditionStatus();           // Ev App.
    uplink.omit_highVoltageBatteryPack();        // Ev App.

    // NGTP v3.9 deleted. 20160824
    // uplink.omit_diagJobRequestAck();
    // uplink.omit_diagJobDescRequest();
    // uplink.omit_diagJobResult();
    // uplink.omit_diagJobEnd();
}

inline void print_ServiceDatatoASN1(svc::tsdp_svc_Control &ctl, PEREncodedBuffer &encodedSvcData)
{
    svc::NGTPServiceData_PDU  outPDU;
    outPDU.decode ( ctl, encodedSvcData);

    unsigned long decodeLength = outPDU.toStringLength(ctl);
    char* buf = new char[decodeLength+1];

    outPDU.toString ( ctl, buf, decodeLength+1);
    int decodeBufLen = strlen(buf);
    int curPos = 0;
    LOGV("Decode Buffer Length : %d", decodeBufLen);
    LOGV("======== Debug decode NGTP Data =======");
    while(decodeBufLen >= curPos)
    {
        LOGV ("%s", buf+curPos );
        curPos += 1023;
    }
    LOGV("=======================================");

    delete buf;
}

bool EvtNgtpMsgManager::encodeNgtpServiceData(PEREncodedBuffer &encodedSvcData, svc::NGTPUplinkCore &uplink, svc::ServiceMessageType svcMsgType, svc::Acknowledge _appAck, bool bOmitAppAck)
{

    svc::NGTPServiceData::serviceDataCoreMessage data_core;
    data_core.set_uplink(uplink);

    svc::NGTPServiceData serviceData;
    if(bOmitAppAck)
    {
        serviceData.omit_appAck();
        LOGV("Omit AppAck");
    }
    else
    {
        serviceData.set_appAck(_appAck);
    }
    // Parameter set
    serviceData.set_appAckRequired(FALSE);
    serviceData.set_messageType (svcMsgType);   // Parameter set  --> (svc::acknowledge);
    serviceData.set_testFlag (FALSE);
    serviceData.set_serviceDataCoreMessage(data_core);

    svc::NGTPServiceData_PDU svcDataPDU;
    svcDataPDU.set_data (serviceData);

    svc::tsdp_svc_Control ctl;
    ctl.setEncodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setDecodingFlags(NOCONSTRAIN | RELAXBER | RELAXPER);
    ctl.setEncodingRules(OSS_PER_UNALIGNED);

    try{
        if ( svcDataPDU.encode(ctl, encodedSvcData) != E_OK /*0*/ ){
            LOGE("NGTPServiceData_PDU encode fails  :%s", ctl.getErrorMsg());
            return false;
        }
    }catch (ASN1RuntimeException e)
    {
        LOGE("[ERROR][ASN1RuntimeException] ErrorCode (%d) : %s ",e.get_code(), e.describe_error());
        return false;
    }

    if(bLoggingNgtpDecoder == true)
    {
        print_ServiceDatatoASN1(ctl, encodedSvcData);
    }
    LOGV(" encoded byte:%lu", encodedSvcData.get_data_size() );
    return true;
}

inline void setInternalNGTPData(InternalNGTPData &internalNGTPData, PEREncodedBuffer &encodedSvcData, uint8_t ngtp_send_type = EvtNgtpMsgManager::NGTP_UDP, char* evt_ngtp_sms_number = NULL)
{
    internalNGTPData.tspErrorCode = 0;

    internalNGTPData.dsptAckRequired = true;

    if(internalNGTPData.creationTime_seconds == 0)
    {
        internalNGTPData.creationTime_seconds = UTIL::get_NGTP_time();
        internalNGTPData.creationTime_millis = 0;
    }
    LOGE("ECT-(%u)", internalNGTPData.creationTime_seconds);

    internalNGTPData.crqmRequest = false;
    internalNGTPData.transport = ngtp_send_type;

    internalNGTPData.retryNum = 0;//bCall_req_ngtp_retry_count;
    internalNGTPData.retryDelay = 0;//bCall_req_ngtp_retry_delay;

    if(ngtp_send_type==EvtNgtpMsgManager::NGTP_SMS && evt_ngtp_sms_number != NULL ){
        strncpy(internalNGTPData.callNumber, evt_ngtp_sms_number, sizeof(internalNGTPData.callNumber));
        LOGV("internalNGTPData.callNumber :%s \n" ,internalNGTPData.callNumber);
    }
    internalNGTPData.encodedSize  = encodedSvcData.get_data_size();
    memcpy ( internalNGTPData.encodedSvcData, encodedSvcData.get_data(), encodedSvcData.get_data_size());
}

inline uint8_t getNgtpDispatcherServiceType(uint8_t typeUplinkMsg)
{
    switch(typeUplinkMsg)
    {
        case EvtNgtpMsgManager::CHARGE_SETTING_UPDATE:
        {
            LOGV("===== Encode NGTP ChargeSettingUpdate =====");
            return EvtNgtpMsgManager::EVT_CHARGE_PROFILE;
        }
        case EvtNgtpMsgManager::CHARGING_STATUS_UPDATE_ALERT:
        {
            LOGV("===== Encode NGTP Charge Status Update Alert =====");
            return EvtNgtpMsgManager::EVT_REMOTE_VEHICLE_STATUS;
        }
        case EvtNgtpMsgManager::CHARGING_STATUS_UPDATE:
        {
            LOGV("===== Encode NGTP Charge Status Update =====");
            return EvtNgtpMsgManager::EVT_REMOTE_VEHICLE_STATUS;
        }
        case EvtNgtpMsgManager::CHARGE_SETTING_RESP:
        {
            LOGV("===== Encode NGTP ChargeSettingResponse =====");
            return EvtNgtpMsgManager::EVT_CHARGE_PROFILE;
        }
        case EvtNgtpMsgManager::PRECONDITION_START_RESP:
        {
            LOGV("===== Encode NGTP PreconditionStartResponse =====");
            return EvtNgtpMsgManager::EVT_PRECONDITIONING;
        }
        case EvtNgtpMsgManager::PRECONDITION_STOP_RESP:
        {
            LOGV("===== Encode NGTP PreconditionStopResponse =====");
            return EvtNgtpMsgManager::EVT_PRECONDITIONING;
        }
        case EvtNgtpMsgManager::PRECONDITION_SETTING_RESP:
        {
            LOGV("===== Encode NGTP PreconditionSettingResponse =====");
            return EvtNgtpMsgManager::EVT_PRECONDITIONING;
        }
        case EvtNgtpMsgManager::PRECONDITION_SETTING_UPDATE:
        {
            LOGV("===== Encode NGTP PreconditionSettingUpdate =====");
            return EvtNgtpMsgManager::EVT_PRECONDITIONING;
        }
        case EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT:
        {
            LOGV("===== Encode NGTP PreconditionUpdateAlert =====");
            return EvtNgtpMsgManager::EVT_REMOTE_VEHICLE_STATUS;
        }
        case EvtNgtpMsgManager::BATTERY_INFORMATION_UPDATE:
        {
            LOGV("===== Encode NGTP BatteryInfoUpdate =====");
            return EvtNgtpMsgManager::EVT_REMOTE_BATTERY_STATUS;
        }

        default:
        {
            LOGE("Unknown Ngtp request type ");
            return EvtNgtpMsgManager::EVT_UNKNOWN;
        }
    }
}

inline void setNgtpUplinkData_errorCodes(svc::NGTPUplinkCore &uplink, uint8_t ngtpResult)
{
    OssIndex tErrorIdx = NULL;
    svc::NGTPUplinkCore::errorCodes tErrCodes;
    setEvErrorCodes(tErrorIdx, tErrCodes, ngtpResult);
    uplink.set_errorCodes(tErrCodes);
}

inline void setEvErrorCodes(OssIndex &tErrorIdx, svc::NGTPUplinkCore::errorCodes &tErrCodes, uint8_t ngtpResult)
{
    svc::ErrorCode errCode;
    switch(ngtpResult)
    {
        /////////// ChargeSettingsResponse ///////////////
        case svc::ErrorCode::parameterAlreadyInRequestedState_chosen:       // Charge Now already in requested state.
        {
            errCode.set_parameterAlreadyInRequestedState(0);
            LOGV("NGTP set ErrorCode [parameterAlreadyInRequestedState]");
            break;
        }
        case svc::ErrorCode::conflictWithOnboardChange_chosen:              // Setting was changed by the user in the car before this change could be applied.
        {
            errCode.set_conflictWithOnboardChange(0);
            LOGV("NGTP set ErrorCode [conflictWithOnboardChange]");
            break;
        }
        case svc::ErrorCode::parameterOutOfRange_chosen:                    // Max state of charge value requested is out of range
        {
            errCode.set_parameterOutOfRange(0);
            LOGV("NGTP set ErrorCode [parameterOutOfRange_chosen]");
            break;
        }
        /////////// PreconditionStartResponse ///////////////
        case svc::ErrorCode::serviceAlreadyRunning_chosen:
        {
            errCode.set_serviceAlreadyRunning(0);
            LOGV("NGTP set ErrorCode [serviceAlreadyRunning_chosen]");
            break;
        }
        case svc::ErrorCode::hvacSystemFailure_chosen:
        {
            errCode.set_hvacSystemFailure(0);
            LOGV("NGTP set ErrorCode [hvacSystemFailure_chosen]");
            break;
        }
        case svc::ErrorCode::batteryLevelTooLow_chosen:
        {
            errCode.set_batteryLevelTooLow(0);
            LOGV("NGTP set ErrorCode [batteryLevelTooLow_chosen]");
            break;
        }
        case svc::ErrorCode::theftEvent_chosen:
        {
            errCode.set_theftEvent(0);
            LOGV("NGTP set ErrorCode [theftEvent_chosen]");
            break;
        }
        case svc::ErrorCode::vehiclePowerModeNotCorrect_chosen:
        {
            errCode.set_vehiclePowerModeNotCorrect(0);
            LOGV("NGTP set ErrorCode [vehiclePowerModeNotCorrect_chosen]");
            break;
        }
        case svc::ErrorCode::crashEvent_chosen:
        {
            errCode.set_crashEvent(0);
            LOGV("NGTP set ErrorCode [crashEvent_chosen]");
            break;
        }
        case svc::ErrorCode::alarmActive_chosen:
        {
            errCode.set_alarmActive(0);
            LOGV("NGTP set ErrorCode [alarmActive_chosen]");
            break;
        }
        ////////////// PreconditionStopResponse ///////////////
        case svc::ErrorCode::climateModeNotCorrect_chosen:
        {
            errCode.set_climateModeNotCorrect(0);
            LOGV("NGTP set ErrorCode [climateModeNotCorrect_chosen]");
            break;
        }
        ////////////// Common Error /////////////////
        case svc::ErrorCode::timeout_chosen:                                       // Timeout trying to apply the change.
        {
            errCode.set_timeout(0);
            LOGV("NGTP set ErrorCode [timeout_chosen]");
            break;
        }
        case svc::ErrorCode::invalidData_chosen:                                  // Generic Error
        {
            errCode.set_invalidData(0);
            LOGV("NGTP set ErrorCode [invalidData_chosen]");
            break;
        }
        case svc::ErrorCode::incorrectState_chosen:                                 // TU internal state prevented the message from being processed
        {
            errCode.set_incorrectState(0);
            LOGV("NGTP set ErrorCode [incorrectState_chosen]");
            break;
        }
        case svc::ErrorCode::notAllowed_chosen:                                     // TU is not allowed to process the request
        {
            errCode.set_notAllowed(0);
            LOGV("NGTP set ErrorCode [notAllowed_chosen]");
            break;
        }
        case svc::ErrorCode::conditionsNotCorrect_chosen:
        {
            errCode.set_conditionsNotCorrect(0);
            LOGV("NGTP set ErrorCode [conditionsNotCorrect_chosen]");
            break;
        }
        case svc::ErrorCode::executionFailure_chosen:
        {
            errCode.set_executionFailure(0);
            LOGV("NGTP set ErrorCode [executionFailure_chosen]");
            break;
        }
        case svc::ErrorCode::hvacTimeout_chosen:
        {
            errCode.set_hvacTimeout(0);
            LOGV("NGTP set ErrorCode [hvacTimeout_chosen]");
            break;
        }
        case svc::ErrorCode::resRequired_chosen:
        {
            errCode.set_resRequired(0);
            LOGV("NGTP set ErrorCode [resRequired_chosen]");
            break;
        }
        default:
        {
            errCode.set_conditionsNotCorrect(0);
            LOGV("[ERROR][UNKNOWN ErrorCode Case (%d)] NGTP set ErrorCode [conditionsNotCorrect]", ngtpResult);
            break;
        }
    }
    tErrorIdx = tErrCodes.prepend(errCode);
}

#define SET_ALERT_DATA(Q) svc::AlertData::Q##_chosen:{_alertData.set_##Q(0);break;}
inline void setEvAlertData(svc::AlertData &_alertData, uint8_t alertType)
{
    switch(alertType)
    {
        case SET_ALERT_DATA( evChargeInitiated )
        case SET_ALERT_DATA( evChargeComplete )
        case SET_ALERT_DATA( evChargeFault )
        case SET_ALERT_DATA( evChargeIncomplete )
        case SET_ALERT_DATA( evChargeInterrupted )
        //case SET_ALERT_DATA( evPluginReminder )
        case SET_ALERT_DATA( evBulkChargeComplete )
        case SET_ALERT_DATA( evCannotReachFullCharge )
        case SET_ALERT_DATA( evCannotReachMaxSOC )

        //Unused.
        case SET_ALERT_DATA( lowBatteryVoltx10 )
        case SET_ALERT_DATA( fuelLevel )
        case SET_ALERT_DATA( fuelLevelLtrs )
        case SET_ALERT_DATA( remoteClimate )
        //Unused.
        case SET_ALERT_DATA( preconditioningStarted )
        case SET_ALERT_DATA( preconditioningStopped )
        case SET_ALERT_DATA( preconditioningComfortLevelReached )
        case SET_ALERT_DATA( preconditioningNotOk )
        case SET_ALERT_DATA( preconditioningExtendNotAllowed )
        case SET_ALERT_DATA( preconditioningNotOkSoc )
        case SET_ALERT_DATA( preconditioningNotOkFault )
        case SET_ALERT_DATA( preconditioningNotOkBatteryConditioning )
        case SET_ALERT_DATA( preconditioningBatteryConditioningCompleted )
        case SET_ALERT_DATA( preconditioningPartiallyCompleted )
        case SET_ALERT_DATA( preconditioningNotOkSVTActive )
        case SET_ALERT_DATA( preconditioningNotOkAlarmActive )
        case SET_ALERT_DATA( preconditioningNotOkCrashActive )
        case SET_ALERT_DATA( preconditioningNotOkHVACfailure )
        case SET_ALERT_DATA( preconditioningNotOkUserRejectedRES )
        case SET_ALERT_DATA( preconditioningNotOkPowermode )
        case SET_ALERT_DATA( preconditioningResNotOkSVTActive )
        case SET_ALERT_DATA( preconditioningResNotOkPowerMode )
        case SET_ALERT_DATA( preconditioningResNotOkCrashActive )
        case SET_ALERT_DATA( preconditioningResNotOkAlarmActive )
        case SET_ALERT_DATA( preconditioningResNotOkFuelLevelLow )
        case SET_ALERT_DATA( preconditioningResNotOkHVACfailure )
        case SET_ALERT_DATA( preconditioningResNotOkRemainingTime )
        case SET_ALERT_DATA( preconditioningSocCritical )
        case SET_ALERT_DATA( preconditioningRemoteToNormal )
        default:
        {
            LOGE("[ERROR] [%d]UNKNOWN AlertType!! return:<unused>", alertType);
            _alertData.set_unused(0);
            break;
        }
    }

}
#undef SET_ALERT_DATA

inline void setNgtpUplinkData_Alerts(svc::NGTPUplinkCore &uplink, uint8_t alertType)
{
    svc::AlertType _alert;

    svc::TimeStamp _timeStamp;
    _timeStamp.set_seconds(UTIL::get_NGTP_time());
    LOGI("Current Time : %d", UTIL::get_NGTP_time());
    _alert.set_time(_timeStamp);

    svc::AlertData _alertData;
    setEvAlertData(_alertData, alertType);
    _alert.set_alertData(_alertData);

    // _alert.set_alertActive(true);
    if(alertType >= svc::AlertData::evChargeInitiated_chosen
        && alertType <= svc::AlertData::particulateFilter_chosen) // Type16~24 : SRD_ReqEVC0044_v1, Precondition Alert Type
    {
        _alert.set_alertActive(true);
    }
    else
    {
        _alert.set_alertActive(false);
    }

    svc::NGTPUplinkCore::alerts _alertList;
    OssIndex tAlertsIdx = NULL;
    tAlertsIdx = _alertList.insert_after(tAlertsIdx, _alert);

    uplink.set_alerts(_alertList);
}

inline void setNgtpUplinkData_ElectricVehicleInformation(svc::NGTPUplinkCore &uplink)
{
    svc::ElectricVehicleInformation evInfo;
    evInfo.omit_chargingInformation();
    evInfo.omit_chargeSetting();
    evInfo.omit_evRangeMapInformation();

    encode_chargingStatus(evInfo);
    encode_batteryStatus(evInfo);
    encode_chargingInformation(evInfo);
    encode_energyConsumedLastChargekWh(evInfo);
    encode_chargeSetting(evInfo);
    encode_evRangePredicStatus(evInfo);

    // EVRangeMapInformation, wlChargingStatus is available from NGTP Ver 3.11.
    #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3

      int32_t vehicleType = EvConfigData::GetInstance()->getEvCfgData("VEHICLE_TYPE");
      // EVRangeMapInformation
      if((EvConfigData::GetInstance()->configData["AppModeEVR"] == ENABLED) && (vehicleType == _BEV))
      {
          encode_evRangeMapInformation(evInfo);
      }
      else
      {
          LOGE("EVRMapInformation is Not Encoding AppModeEVR[%d] VehicleType[%d]"
            , EvConfigData::GetInstance()->configData["AppModeEVR"], vehicleType);
      }

      // wlChargingStatus
      evInfo.omit_wirelessChargingStatus();
      encode_wlChargingStatus(evInfo);

      // nextDepartureTimer
      evInfo.omit_nextDepartureTimer();
      encode_nextDepartureTimer(evInfo);
    #endif

    uplink.set_electricVehicleInformation(evInfo);
}

inline void setNgtpUplinkData_vehicleStatus(svc::NGTPUplinkCore &uplink)
{
    /*
    BasicVehicleStatus -- See ASN.1 below Only “state” is required.
    */
    svc::BasicVehicleStatus vehicleStatus;
    vehicleStatus.omit_mainBattery();
    vehicleStatus.omit_theftAlarm();
    vehicleStatus.omit_fuelLevelPerc();
    vehicleStatus.omit_srs();
    vehicleStatus.set_engineBlock(svc::EngineBlockedStatus_unknown);
    vehicleStatus.set_brazilEventMode(svc::isUnknown);

    /*
        ** Condition.
                    VehicleStateType ::= ENUMERATED {
                        engine-on-moving,       -- Powermode=Running and VehicleSpeed>0
                        engine-on-park,         -- Powermode=Running and VehicleSpeed=0
                        key-on-engine-off,      -- Powermode=Ignition or Acessory
                        delayed-accessory,      -- Powermode=PostAcessory ?
                        key-off,                -- Powermode=Key inserted
                        key-removed,            -- Powermode=Key out
                        engine-on-remote-start, -- Engine is started from remote
                        unknown                 -- CAN bus asleep or TU is not connected to CAN
                    }
        ** Value.
                    enum VehicleStateType {
                        engine_on_moving = 0,
                        engine_on_park = 1,
                        key_on_engine_off = 2,
                        delayed_accessory = 3,
                        key_off = 4,
                        key_removed = 5,
                        engine_on_remote_start = 6,
                        VehicleStateType_unknown = 7
                    };
    */
    uint16_t _PM = EvVehicleStatus::GetInstance()->getCurPowerMode();
    uint16_t _state = svc::VehicleStateType_unknown;
    switch(_PM)
    {
        case PM_7_RUNNING:
        {
            int32_t _speed = EvVehicleStatus::GetInstance()->getSpeedx100();
            _state = (_speed > 0) ? svc::engine_on_moving : svc::engine_on_park;  // 0 or 1
            break;
        }

        case PM_6_IGNOTION_ON:
        case PM_4_ACCESSORY_ON:
        {
            _state = svc::key_on_engine_off; //2
            break;
        }

        case PM_3_POST_ACCESSORY:
        {
            _state = svc::delayed_accessory; //3
            break;
        }

        case PM_2_KEY_APPROVED:
        {
            _state = svc::key_off;  // 4
            break;
        }

        case PM_1_KEY_RECENTLYOUT:
        case PM_0_KEY_OUT:
        {
            _state = svc::key_removed; //5
            break;
        }

        default:
        {
            _state = svc::VehicleStateType_unknown;
            break;
        }
    }

    if(EvVehicleStatus::GetInstance()->IsRemoteStartStatus() == true)
    {
        _state = svc::engine_on_remote_start;
    }

    vehicleStatus.set_state(_state);
    uplink.set_vehicleStatus(vehicleStatus);
}

void EvtNgtpMsgManager::setNgtpUplinkData_preconditionStatus(svc::NGTPUplinkCore &uplink, uint8_t ngtpResult)
{
    svc::EVPreconditionStatus preconditionstatus;
    setCurEvPreconditionStatus(preconditionstatus);
    uplink.set_preconditionStatus(preconditionstatus);
}

inline svc::TimeHM setTimeHM(uint32_t _hour, uint32_t _minute)
{
    svc::TimeHM _timeHM;
    _timeHM.set_hour(_hour);
    _timeHM.set_minute(_minute);
    return _timeHM;
}

inline svc::ClimateTimer setClimateTimer(int _month, int _day, svc::TimeHM _timeHM)
{
    svc::ClimateTimer _climateTime;
    if(_month < 0)
    {
        _climateTime.omit_month();
    }
    else
    {
        _climateTime.set_month(_month);
    }

    if(_day < 0)
    {
        _climateTime.omit_day();
    }
    else
    {
        _climateTime.set_day(_day);
    }

    _climateTime.set_time(_timeHM);
    return _climateTime;
}

inline void setNgtpUplinkData_climateStatus(svc::NGTPUplinkCore &uplink)
{
    svc::ClimateStatus climateStatus;

    // TODO: implement.
    //LOGE("[WARNNING] ClimateStatus Encoding -> SRD9 Spec v51 does not define mapping table between CAN signal <-> NGTP Field!! SET DEFAULT VALUE!");

    // svc::ClimateTimer _climateTime;
    // _climateTime.omit_month();
    // _climateTime.omit_day();


    // svc::TimeHM _timeHM;
    // _timeHM.set_hour(0);
    // _timeHM.set_minute(0);

    // _climateTime.set_time(_timeHM);

    climateStatus.set_timer1(setClimateTimer(-1, -1, setTimeHM(0,0)));
    climateStatus.set_timer2(setClimateTimer(-1, -1, setTimeHM(0,0)));
    climateStatus.set_climateOperatingStatus(svc::ClimateOperatingStatus::ClimateOperatingStatus_unknown);
    climateStatus.set_remainingRunTime(0);
    climateStatus.set_ffhRemainingRunTime(0);
    climateStatus.set_ventingTime(0);
    climateStatus.set_heatedFrontScreenActive(svc::BOOLEXT::isUnknown);
    climateStatus.set_heatedRearWindowActive(svc::BOOLEXT::isUnknown);
    climateStatus.set_heatedSteeringWheelActive(svc::BOOLEXT::isUnknown);

    climateStatus.omit_heatedSeatFrontLeft();
    climateStatus.omit_heatedSeatFrontRight();
    climateStatus.omit_heatedSeatRearLeft();
    climateStatus.omit_heatedSeatRearRight();

    uplink.set_climateStatus(climateStatus);
}

inline void encode_PermanentMaxSoc(svc::MaxStateOfCharge& evMaxSoc )
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();
    svc::MaxStateOfChargeSetting evPermanentMaxSoc;

    int8_t _permStatus = rawChargeSettings->getPermanentMaxSocStatus();

    switch(_permStatus)
    {
        case 1:  // NoChange
        {
            LOGV("[encode_PermanentMaxSoc] No Change");
            evPermanentMaxSoc.set_noChange(NULL);
            break;
        }
        case 2:  // Clear
        {
            LOGV("[encode_PermanentMaxSoc] Clear");
            evPermanentMaxSoc.set_clear(NULL);
            break;
        }
        case 3:  // MAX SOC
        {
            int32_t _maxSoc = rawChargeSettings->getPermanentMaxSocField();
            LOGV("[encode_PermanentMaxSoc] MaxSOC : %d", _maxSoc);
            evPermanentMaxSoc.set_maxSoc(_maxSoc);
            break;
        }
        default:
        {
            LOGV("[encode_PermanentMaxSoc] inValid Status : %d", _permStatus);
            evPermanentMaxSoc.set_noChange(NULL);
            break;
        }
    }
    evMaxSoc.set_permanentMaxSoc(evPermanentMaxSoc);
}

inline void encode_OneOffMaxSoc(svc::MaxStateOfCharge& evMaxSoc)
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();
    svc::MaxStateOfChargeSetting evOneOffMaxSoc;

    int8_t _oneOffStatus = rawChargeSettings->getOneOffMaxSocStatus();

    switch(_oneOffStatus)
    {
        case 1:  // NoChange
        {
            LOGV("[encode_OneOffMaxSoc] No Change");
            evOneOffMaxSoc.set_noChange(NULL);
            break;
        }
        case 2:  // Clear
        {
            LOGV("[encode_OneOffMaxSoc] Clear");
            evOneOffMaxSoc.set_clear(NULL);
            break;
        }
        case 3:  // MAX SOC
        {
            int32_t _maxSoc = rawChargeSettings->getOneOffMaxSocField();
            LOGV("[encode_OneOffMaxSoc] MaxSOC : %d", _maxSoc);
            evOneOffMaxSoc.set_maxSoc(_maxSoc);
            break;
        }
        default:
        {
            LOGV("[encode_OneOffMaxSoc] inValid Status : %d", _oneOffStatus);
            evOneOffMaxSoc.set_noChange(NULL);
            break;
        }
    }
    evMaxSoc.set_oneOffMaxSoc(evOneOffMaxSoc);
}

inline void encode_maxStateOfCharge(svc::ChargeSettings &evChargeSetting)
{
    svc::MaxStateOfCharge evMaxSoc;
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();
    svc::MaxStateOfChargeSetting evOneOffMaxSoc;
    svc::MaxStateOfChargeSetting evPermanentMaxSoc;

    if(rawChargeSettings->bPermanentMaxSoc_is_updated == true)
    {
        encode_PermanentMaxSoc(evMaxSoc);
    }
	else
	{
        evPermanentMaxSoc.set_noChange(NULL);
        evMaxSoc.set_permanentMaxSoc(evPermanentMaxSoc);
	    LOGV("[encode_maxStateOfCharge] PermMaxSoc is not updated");
	}

    if(rawChargeSettings->bOneOffMaxSoc_is_updated == true)
    {
        encode_OneOffMaxSoc(evMaxSoc);
    }
	else
	{
        evOneOffMaxSoc.set_noChange(NULL);
        evMaxSoc.set_oneOffMaxSoc(evOneOffMaxSoc);
	    LOGV("[encode_maxStateOfCharge] oneOffMaxSoc is not updated");
	}
    evChargeSetting.set_maxStateOfCharge(evMaxSoc);
}

inline void encode_chargeSetting(svc::ElectricVehicleInformation &evInfo)
{
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();
    svc::ChargeSettings evChargeSettings;

    evChargeSettings.omit_chargeNowSetting();
    evChargeSettings.omit_maxStateOfCharge();
    evChargeSettings.omit_chargingModeChoice();
    evChargeSettings.omit_departureTimers();
    evChargeSettings.omit_tariffs();

    if(rawEvInfo->bChargeSetting_field_is_updated == true)
    {
        // ChargeNowSetting
        if(rawChargeSettings->bChargeNowSetting_is_present == true)
        {
            evChargeSettings.set_chargeNowSetting(rawChargeSettings->getChargeNowSettingField());
            LOGV("[encode_chargeSetting] chargeNowSetting updated");
        }

        // MaxStateOfCharge - PermanentMaxSoc, OneOffMaxSoc
        if(rawChargeSettings->bPermanentMaxSoc_is_updated || rawChargeSettings->bOneOffMaxSoc_is_updated )
        {
            encode_maxStateOfCharge(evChargeSettings);
			LOGV("[encode_chargeSetting] MaxSOC updated");
        }

        //chargingModeChoice
        if(rawChargeSettings->bChargingModeChoice_is_present == true)
        {
            evChargeSettings.set_chargingModeChoice(rawChargeSettings->getChargingModeChoiceField());
            LOGV("[encode_chargeSetting] chargeModeChoice updated");
        }

        //departureTimers
        if(rawChargeSettings->bDepartureTimers_is_present == true)
        {
            encode_DepartureTimers(evChargeSettings);
            LOGV("[encode_chargeSetting] DepartureTimers updated");
        }

        // Tariff
        if(rawChargeSettings->bTariffs_is_present == true)
        {
            encode_Tariffs(evChargeSettings);
        }
        evInfo.set_chargeSetting(evChargeSettings);
    }
    else
    {
        LOGE("bChargeSetting_field_is_updated is false");
        evInfo.omit_chargeSetting();
    }
}

inline void encode_DepartureTimers(svc::ChargeSettings &evChargeSettings)
{
    EvDepartureTimers evDepartureTimers;
    EvDepartureTimerList evDepartureTimerList;

    evChargeSettings.omit_departureTimers();
    encode_DepartureTimer(evDepartureTimerList);

    evDepartureTimers.set_timers(evDepartureTimerList);

    evChargeSettings.set_departureTimers(evDepartureTimers);
}

inline void encode_DepartureTimer(EvDepartureTimerList& evDepartureTimerList)
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();
    for( std::vector<_DepartureTimer>::size_type i ; i < rawChargeSettings->departureTimer_field.size() ; i++)
    {
        EvDepartureTimer evDepartureTimer;
        _DepartureTimer _departureTimer = rawChargeSettings->departureTimer_field[i];

        if(_departureTimer._bDepartureTimer_is_updated)
        {
            LOGV("_bDepartureTimer_is_updated is true Vector : %d", i);
            evDepartureTimer.set_timerIndex(_departureTimer.timerIndex_field);
            // When Departure Timer is Deleted, it is not encoding.
            // Condition of distinguish delete
            // From TSP : _bDepartureTimerDefinition_is_present == false
            // From IMC : PrepDepTimeActive=0
            if(_departureTimer._bDepartureTimerDefinition_is_present)
            {
                LOGV("_bDepartureTimerDefinition_is_present Vector : %d", i);
                encode_DepartureTimerDefinition(evDepartureTimer, _departureTimer);
                evDepartureTimerList.prepend(evDepartureTimer);
            }
            else
            {
                LOGV("_bDepartureTimerDefinition_is_ not present Vector : %d", i);
                evDepartureTimer.omit_departureTimerDefinition();
                evDepartureTimerList.prepend(evDepartureTimer);
            }
        }
    }
}

inline void encode_timerTarget(EvDepartureTimerDefinition& evDepartureTimerDefinition, _DepartureTimerDefinition& _departureTimerDefinition)
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();

    EvTimerTarget evTimerTarget;
    EvSingleDay evSingleDay;
    EvDepartureRepeatSchedule evDayofWeekSchedule;

    _TimerTarget timerTarget = _departureTimerDefinition.timerTarget_field;
    uint8_t timeType = timerTarget.timeType_field;

    if(timeType == 0)
    {
        evSingleDay.set_day(timerTarget.singleDay_field.day);
        evSingleDay.set_month(timerTarget.singleDay_field.month);
        evSingleDay.set_year(timerTarget.singleDay_field.year);
        evTimerTarget.set_singleDay(evSingleDay);
    }
    else
    {
        char tmpField;
        _DayOfWeekSchedule dayOfWeek = timerTarget.repeatSchedule_field;
        DayOfWeekSchedule evDayOfWeek;

        tmpField = dayOfWeek.monday_field ? 1 : 0;
        evDayOfWeek.set_monday(tmpField);

        tmpField = dayOfWeek.tuesday_field ? 1 : 0;
        evDayOfWeek.set_tuesday(tmpField);

        tmpField = dayOfWeek.wednesday_field ? 1 : 0;
        evDayOfWeek.set_wednesday(tmpField);

        tmpField = dayOfWeek.thursday_field ? 1 : 0;
        evDayOfWeek.set_thursday(tmpField);

        tmpField = dayOfWeek.friday_field ? 1 : 0;
        evDayOfWeek.set_friday(tmpField);

        tmpField = dayOfWeek.saturday_field ? 1 : 0;
        evDayOfWeek.set_saturday(tmpField);

        tmpField = dayOfWeek.sunday_field ? 1 : 0;
        evDayOfWeek.set_sunday(tmpField);

        evTimerTarget.set_repeatSchedule(evDayOfWeek);
    }
    evDepartureTimerDefinition.set_timerTarget(evTimerTarget);
}

inline void encode_DepartureTimerDefinition(EvDepartureTimer& evDepartureTimer, _DepartureTimer& _departureTimer)
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();
    evDepartureTimer.omit_departureTimerDefinition();

    _DepartureTimerDefinition _departureTimerDefinition = _departureTimer.departureTimerDefinition_field;

    EvDepartureTimerDefinition evDepartureTimerDefinition;
    EvDepartureTime evDepartureTime;

    evDepartureTime.set_hour(_departureTimerDefinition.departureTime_field.hour_field);
    evDepartureTime.set_minute(_departureTimerDefinition.departureTime_field.minute_field);

    evDepartureTimerDefinition.set_timerType(_departureTimerDefinition.timerType_field);
    evDepartureTimerDefinition.set_departureTime(evDepartureTime);
    encode_timerTarget(evDepartureTimerDefinition, _departureTimerDefinition);

    evDepartureTimer.set_departureTimerDefinition(evDepartureTimerDefinition);
}

inline void encode_Tariffs(svc::ChargeSettings &evChargeSettings)
{
    svc::Tariffs evTariffs;
    svc::Tariffs::tariffs evTariffList;

    encode_Tariff(evTariffList);
    evTariffs.set_tariffs(evTariffList);
    evChargeSettings.set_tariffs(evTariffs);
}

inline void encode_Tariff(svc::Tariffs::tariffs& ngtp_TariffList)
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();

    for( auto&& evt_Tariff : rawChargeSettings->tariffs_field)
    {
        if(evt_Tariff._bTariffDefinition_is_present)
        {
            //Procedure 1. make Tariff --> encode_TariffDefinition();
            //Tariff(tariffIndex);
            svc::Tariff ngtp_Tariff(evt_Tariff.tariffIndex_field);
            LOGV("[encode_Tariff] Enable TariffIdx : %d",ngtp_Tariff.get_tariffIndex());
            encode_TariffDefinition(ngtp_Tariff, evt_Tariff);

            //Procedure 2. insert_after(tarriff)
            ngtp_TariffList.prepend(ngtp_Tariff);
        }
        else
        {
            LOGE("[encode_Tariff] Disable TariffIdx");
        }

    }
}

inline void encode_TariffDefinition(svc::Tariff& ngtp_Tariff, _Tariff& evt_tariff)
{
    svc::TariffDefinition ngtp_TariffDefinition;  // include omit_tariffDefinition in constrcture.
    ngtp_Tariff.omit_tariffDefinition();
    if(evt_tariff._bTariffDefinition_is_present
        && evt_tariff.tariffDefinition_field.isEnabled_field)
    {
        ngtp_TariffDefinition.set_isEnabled(1);
        encode_RepeatSchedule(ngtp_TariffDefinition, evt_tariff.tariffDefinition_field);
        encode_TariffZone(ngtp_TariffDefinition, evt_tariff.tariffDefinition_field);
        ngtp_Tariff.set_tariffDefinition(ngtp_TariffDefinition);
    }
}

// #define _TD _tariffDefinition
// #define _TDP(A) _TD._bTariffZone##A##_is_present
// #define _TDZ(A) _TD.tariffZone##A##_field
// #define _setTariffZone(A) [&]()                                                             \
//                         {                                                                   \
//                             bool _present = _TDP(A);                                        \
//                             _TariffZone _zone = _TDZ(A);                                    \
//                             if(_present)                                                    \
//                             {                                                               \
//                                 evTariffZone##A = svc::TariffZone(TimeHM(_zone.endTime_field.hour_field,        \
//                                                                          _zone.endTime_field.minute_field),     \
//                                                                   _zone.bandType_field);    \
//                             }                                                               \
//                             else                                                            \
//                             {                                                               \
//                                 evTariffZone##A = svc::TariffZone();                        \
//                             }                                                               \
//                         }

inline void encode_TariffZone(svc::TariffDefinition& evTariffDefinition, _TariffDefinition& _tariffDefinition)
{
    svc::TariffZone evTariffZoneA;
    svc::TariffZone evTariffZoneB;
    svc::TariffZone evTariffZoneC;
    svc::TariffZone evTariffZoneD;
    svc::TariffZone evTariffZoneE;
    evTariffZoneA.omit_endTime();
    evTariffZoneB.omit_endTime();
    evTariffZoneC.omit_endTime();
    evTariffZoneD.omit_endTime();
    evTariffZoneE.omit_endTime();

    // SET TariffZoneA
    _TariffZone _zoneA = _tariffDefinition.tariffZoneA_field;
    if(_tariffDefinition._bTariffZoneA_is_present && _zoneA.bandType_field < 3 && _zoneA.bandType_field >= 0)
    {
        evTariffZoneA = svc::TariffZone(TimeHM(_zoneA.endTime_field.hour_field, _zoneA.endTime_field.minute_field), _zoneA.bandType_field);
        evTariffDefinition.set_tariffZoneA(evTariffZoneA);
    }
    else
    {
        LOGE("TariffZoneA is not Encoding - Invalid BandType[%d] or not Present", _zoneA.bandType_field);
    }

    // SET TariffZoneB
    _TariffZone _zoneB = _tariffDefinition.tariffZoneB_field;
    if(_tariffDefinition._bTariffZoneB_is_present && _zoneB.bandType_field < 3 && _zoneB.bandType_field >= 0)
    {
        evTariffZoneB = svc::TariffZone(TimeHM(_zoneB.endTime_field.hour_field, _zoneB.endTime_field.minute_field), _zoneB.bandType_field);
        evTariffDefinition.set_tariffZoneB(evTariffZoneB);
    }
    else
    {
        LOGE("TariffZoneB is not Encoding - Invalid BandType[%d] or not Present", _zoneB.bandType_field);
        evTariffDefinition.omit_tariffZoneB();
    }

    // SET TariffZoneC
    _TariffZone _zoneC = _tariffDefinition.tariffZoneC_field;
    if(_tariffDefinition._bTariffZoneC_is_present && _zoneC.bandType_field < 3 && _zoneC.bandType_field >= 0)
    {
        evTariffZoneC = svc::TariffZone(TimeHM(_zoneC.endTime_field.hour_field, _zoneC.endTime_field.minute_field), _zoneC.bandType_field);
        evTariffDefinition.set_tariffZoneC(evTariffZoneC);
    }
    else
    {
        LOGE("TariffZoneC is not Encoding - Invalid BandType[%d] or not Present", _zoneC.bandType_field);
        evTariffDefinition.omit_tariffZoneC();
    }

    // SET TariffZoneD
    _TariffZone _zoneD = _tariffDefinition.tariffZoneD_field;
    if(_tariffDefinition._bTariffZoneD_is_present && _zoneD.bandType_field < 3 && _zoneD.bandType_field >= 0)
    {
        evTariffZoneD = svc::TariffZone(TimeHM(_zoneD.endTime_field.hour_field, _zoneD.endTime_field.minute_field), _zoneD.bandType_field);
        evTariffDefinition.set_tariffZoneD(evTariffZoneD);
    }
    else
    {
        LOGE("TariffZoneD is not Encoding - Invalid BandType[%d] or not Present", _zoneD.bandType_field);
        evTariffDefinition.omit_tariffZoneD();
    }

    // SET TariffZoneE
    _TariffZone _zoneE = _tariffDefinition.tariffZoneE_field;
    if(_tariffDefinition._bTariffZoneE_is_present && _zoneE.bandType_field < 3 && _zoneE.bandType_field >= 0)
    {
        evTariffZoneE.omit_endTime();
        evTariffZoneE.set_bandType(_zoneE.bandType_field);
        evTariffDefinition.set_tariffZoneE(evTariffZoneE);
    }
    else
    {
        LOGE("TariffZoneE is not Encoding - Invalid BandType[%d] or not Present", _zoneE.bandType_field);
        evTariffDefinition.omit_tariffZoneE();
    }
}
// #undef _TD
// #undef _TDP(A)
// #undef _TDZ(A)
// #undef _setTariffZone(A)

inline void encode_TariffEndTime(svc::TariffZone& evTariffZone, _TariffZone& _tariffZone)
{
    TimeHM evTimeHM;
    evTimeHM.set_hour(_tariffZone.endTime_field.hour_field);
    evTimeHM.set_minute(_tariffZone.endTime_field.minute_field);

    evTariffZone.set_endTime(evTimeHM);
}

inline void encode_RepeatSchedule(svc::TariffDefinition& evTariffDefinition, _TariffDefinition& _tariffDefinition)
{
    EvtChargeSettings* rawChargeSettings = EvtChargeSettings::GetInstance();

    char tmpField;
    _DayOfWeekSchedule dayOfWeek = _tariffDefinition.repeatSchedule_field;
    DayOfWeekSchedule evDayOfWeek;

    tmpField = dayOfWeek.monday_field ? 1 : 0;
    evDayOfWeek.set_monday(tmpField);

    tmpField = dayOfWeek.tuesday_field ? 1 : 0;
    evDayOfWeek.set_tuesday(tmpField);

    tmpField = dayOfWeek.wednesday_field ? 1 : 0;
    evDayOfWeek.set_wednesday(tmpField);

    tmpField = dayOfWeek.thursday_field ? 1 : 0;
    evDayOfWeek.set_thursday(tmpField);

    tmpField = dayOfWeek.friday_field ? 1 : 0;
    evDayOfWeek.set_friday(tmpField);

    tmpField = dayOfWeek.saturday_field ? 1 : 0;
    evDayOfWeek.set_saturday(tmpField);

    tmpField = dayOfWeek.sunday_field ? 1 : 0;
    evDayOfWeek.set_sunday(tmpField);

    evTariffDefinition.set_repeatSchedule(evDayOfWeek);
}

inline void encode_energyConsumedLastChargekWh(svc::ElectricVehicleInformation &evInfo)
{
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();
    if(rawEvInfo->bEnergyConsumedLastChargekWh_is_preset){
        evInfo.set_energyConsumedLastChargekWh(rawEvInfo->getEnergyConsumedLastChargekWhField());
        LOGI("set_energyConsumedLastChargekWh(%d)",rawEvInfo->getEnergyConsumedLastChargekWhField());
    }else{
        LOGE("[WARNNING] error in encoding 'ElectricVehicleInformation'. energyConsumedLastChargekWh is unknown status.");
        evInfo.set_energyConsumedLastChargekWh(-1);
    }
}

inline void encode_chargingInformation(svc::ElectricVehicleInformation &evInfo)
{
    svc::ElectricVehicleChargingInformation evChargeInfo;
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();

    int32_t tmpValue;

    if(EvVehicleStatus::GetInstance()->isVehicleCharging()
        && rawEvInfo->bChargingInformation_is_present){
        tmpValue = (rawEvInfo->minutesToFullyCharged_is_updated) ? (int32_t)rawEvInfo->getMinutesToFullyChargedField() : -1;
        evChargeInfo.set_minutesToFullyCharged(tmpValue);

        tmpValue = (rawEvInfo->minutesToBulkCharged_is_updated) ? (int32_t)rawEvInfo->getMinutesToBulkChargedField() : -1;
        evChargeInfo.set_minutesToBulkCharged(tmpValue);

        //SRD_ReqEVC0061_v1
        #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
        tmpValue = (rawEvInfo->chargingRateSocPerHour_is_updated) ? (int32_t)rawEvInfo->getChargingRateSocPerHourField() : -1;
        evChargeInfo.set_chargingRateSocPerHourx10(tmpValue);
        #endif

        // unused field should be -1 - JLRTCU-2486
        evChargeInfo.set_chargingRateSocPerHour(-1);

        tmpValue = (rawEvInfo->chargingRateMilesPerHour_is_updated) ? (int32_t)rawEvInfo->getChargingRateMilesPerHourField() : -1;
        evChargeInfo.set_chargingRateMilesPerHour(tmpValue);

        tmpValue = (rawEvInfo->chargingRateKmPerHour_is_updated) ? (int32_t)rawEvInfo->getChargingRateKmPerHourField() : -1;
        evChargeInfo.set_chargingRateKmPerHour(tmpValue);
        LOGI("ElectricVehicleChargingInformation : set_minutesToFullyCharged(%d), \
                                                    set_minutesToBulkCharged(%d), \
                                                    set_chargingRateSocPerHour(%d), \
                                                    set_chargingRateMilesPerHour(%d), \
                                                    set_chargingRateKmPerHour(%d)", \
                                                    rawEvInfo->getMinutesToFullyChargedField(), \
                                                    rawEvInfo->getMinutesToBulkChargedField(), \
                                                    rawEvInfo->getChargingRateSocPerHourField(), \
                                                    rawEvInfo->getChargingRateMilesPerHourField(), \
                                                    rawEvInfo->getChargingRateKmPerHourField());
        evInfo.set_chargingInformation(evChargeInfo);
    }else{
        evInfo.omit_chargingInformation();
    }
}

inline void encode_batteryStatus(svc::ElectricVehicleInformation &evInfo)
{
    ElectricVehicleBatteryInformation evBatteryInfo;
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();
    int32_t tmpValue;

    if(rawEvInfo->bBatteryStatus_is_present){
        tmpValue = (rawEvInfo->rangeOnBatteryKm_is_updated) ? (int32_t)rawEvInfo->getRangeOnBatteryKmField() : -1;
        evBatteryInfo.set_rangeOnBatteryKm(tmpValue);

        tmpValue = (rawEvInfo->rangeOnBatteryMiles_is_updated) ? (int32_t)rawEvInfo->getRangeOnBatteryMilesField() : -1;
        evBatteryInfo.set_rangeOnBatteryMiles(tmpValue);

        tmpValue = (rawEvInfo->rangeCombinedKm_is_updated) ? (int32_t)rawEvInfo->getRangeCombinedKmField() : -1;
        evBatteryInfo.set_rangeCombinedKm(tmpValue);

        tmpValue = (rawEvInfo->rangeCombinedMiles_is_updated) ? (int32_t)rawEvInfo->getRangeCombinedMilesField() : -1;
        evBatteryInfo.set_rangeCombinedMiles(tmpValue);

        tmpValue = (rawEvInfo->stateOfCharge_is_updated) ? (int32_t)rawEvInfo->getStateOfChargeField() : -1;
        evBatteryInfo.set_stateOfCharge(tmpValue);
        LOGI("ElectricVehicleBatteryInformation : set_rangeOnBatteryKm(%d), \
                                                    set_rangeOnBatteryMiles(%d), \
                                                    set_rangeCombinedKm(%d), \
                                                    set_rangeCombinedMiles(%d), \
                                                    set_stateOfCharge(%d)", \
                                                    rawEvInfo->getRangeOnBatteryKmField(), \
                                                    rawEvInfo->getRangeOnBatteryMilesField(), \
                                                    rawEvInfo->getRangeCombinedKmField(), \
                                                    rawEvInfo->getRangeCombinedMilesField(), \
                                                    rawEvInfo->getStateOfChargeField());
    }else{
        LOGE("[WARNNING] error in encoding 'ElectricVehicleInformation'. electricVehicleInformation is unknown status.");
        evBatteryInfo.set_rangeOnBatteryKm(-1);
        evBatteryInfo.set_rangeOnBatteryMiles(-1);
        evBatteryInfo.set_rangeCombinedKm(-1);
        evBatteryInfo.set_rangeCombinedMiles(-1);
        evBatteryInfo.set_stateOfCharge(-1);
    }
    evInfo.set_batteryStatus(evBatteryInfo);
}

inline void encode_chargingStatus(svc::ElectricVehicleInformation &evInfo)
{
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();

    if(rawEvInfo->bChargingStatus_is_present){
        evInfo.set_chargingStatus((svc::ChargingStatus)rawEvInfo->getChargingStatusField());
    }else{
        LOGE("[WARNNING] chargingStatus is unknown status.");
        evInfo.set_chargingStatus(svc::ChargingStatus_unknown);
    }
}

// SRD_ReqEVC0047_v2
inline void encode_wlChargingStatus(svc::ElectricVehicleInformation &evInfo)
{
    #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();
    if(rawEvInfo->bWlChargingStatus_is_present){
        evInfo.set_wirelessChargingStatus((svc::WirelessChargingStatus)rawEvInfo->getWlChargingStatusField());
    }else{
        LOGV("[encode_wlChargingStatus] wlcs is not incuding in EVI.");
        evInfo.omit_wirelessChargingStatus();
    }
    #else
    LOGE("[encode_wlChargingStatus] wlChargeingStatus is not encoding due to NGTP version is lower than 3.11");
    #endif

}

inline void encode_evRangeMapInformation(svc::ElectricVehicleInformation &evInfo)
{
    EvtElectricVehicleInformation* rawEvInfo = EvtElectricVehicleInformation::GetInstance();

    if(EvConfigData::GetInstance()->configData["AppModeEVR"] == ENABLED)
    {
        LOGV("bEvRangeMapInformation_is_present is true");

        #if SVC_MINOR_VER == 9 && SVC_MAJOR_VER == 3
        LOGV("[EvtNgtpMsgManager] SVC_MINOR_VER = 9");
        EVRangeMapInformation evRangeInfo;
        uint32_t tmpEvrValue;

        //Initialized to 0 because their type is unsigned int
        evRangeInfo.set_vscInitialHVBattEnergy(0);
        evRangeInfo.set_vscRevisedHVBattEnergy(0);
        evRangeInfo.set_vscHVEnergyAscent(0);
        evRangeInfo.set_vscHVEnergyDescent(0);
        evRangeInfo.set_vscHVEnergyTimePen(0);
        evRangeInfo.set_vscRegenEnergyFactor(0);
        evRangeInfo.set_vscVehAccelFactor(0);
        evRangeInfo.set_vscRangeMapRefactrComf(0);
        evRangeInfo.set_vscRangeMapRefactrEco(0);
        evRangeInfo.set_vscRangeMapRefactrGMH(0);
        evRangeInfo.set_vscHVBattConsumpSpd1(0);
        evRangeInfo.set_vscHVBattConsumpSpd2(0);
        evRangeInfo.set_vscHVBattConsumpSpd3(0);
        evRangeInfo.set_vscHVBattConsumpSpd4(0);
        evRangeInfo.set_vscHVBattConsumpSpd5(0);
        evRangeInfo.set_vscHVBattConsumpSpd6(0);
        evRangeInfo.set_vscHVBattConsumpSpd7(0);

        // Set Values to svc::EvRangeMapInforMation
        tmpEvrValue = rawEvInfo->bVSCInitialHVBattEnergy_field_is_updated ? rawEvInfo->getVscInitialHvBattEnergyField() : 0;
        evRangeInfo.set_vscInitialHVBattEnergy(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRevisedHVBattEnergy_field_is_updated ? rawEvInfo->getVscRevisedHvBattEnergyField() : 0;
        evRangeInfo.set_vscRevisedHVBattEnergy(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVEnergyAscent_field_is_updated ? rawEvInfo->getVschvEnergyAscentField() : 0;
        evRangeInfo.set_vscHVEnergyAscent(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVEnergyDescent_field_is_updated ? rawEvInfo->getVschvEnergyDescentField() : 0;
        evRangeInfo.set_vscHVEnergyDescent(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVEnergyTimePen_field_is_updated ? rawEvInfo->getVschvEnergyTimePenField() : 0;
        evRangeInfo.set_vscHVEnergyTimePen(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRegenEnergyFactor_field_is_updated ? rawEvInfo->getVscRegenEnergyFactorField() : 0;
        evRangeInfo.set_vscRegenEnergyFactor(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCVehAccelFactor_field_is_updated ? rawEvInfo->getVscVehAccelFactorField() : 0;
        evRangeInfo.set_vscVehAccelFactor(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRangeMapRefactrComf_field_is_updated ? rawEvInfo->getVscRangeMapRefactrComfField() : 0;
        evRangeInfo.set_vscRangeMapRefactrComf(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRangeMapRefactrEco_field_is_updated ? rawEvInfo->getVscRangeMapRefactrEcoField() : 0;
        evRangeInfo.set_vscRangeMapRefactrEco(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRangeMapRefactrGMH_field_is_updated ? rawEvInfo->getVscRangeMapRefactrGmhField() : 0;
        evRangeInfo.set_vscRangeMapRefactrGMH(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd1_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd1Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd1(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd2_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd2Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd2(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd3_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd3Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd3(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd4_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd4Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd4(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd5_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd5Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd5(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd6_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd6Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd6(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd7_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd7Field() : 0;
        evRangeInfo.set_vscHVBattConsumpSpd7(tmpEvrValue);

        // Set svc::EvRangeMapInforMation to svc::ElectricVehicleInformation
        evInfo.set_evRangeMapInformation(evRangeInfo);
    #elif SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
        LOGV("[EvtNgtpMsgManager] SVC_MINOR_VER > 11");
        EVRangeMapInformation evRangeInfo;
        uint32_t tmpEvrValue;

        //Initialized to 0 because their type is unsigned int
        evRangeInfo.set_vscInitialHVBattEnergyx100(0);
        evRangeInfo.set_vscRevisedHVBattEnergyx100(0);
        evRangeInfo.set_vscHVEnergyAscentx10(0);
        evRangeInfo.set_vscHVEnergyDescentx10(0);
        evRangeInfo.set_vscHVEnergyTimePenx100(0);
        evRangeInfo.set_vscRegenEnergyFactor(0);
        evRangeInfo.set_vscVehAccelFactor(100);
        evRangeInfo.set_vscRangeMapRefactrComf(0);
        evRangeInfo.set_vscRangeMapRefactrEco(0);
        evRangeInfo.set_vscRangeMapRefactrGMH(0);
        evRangeInfo.set_vscHVBattConsumpSpd1(100);
        evRangeInfo.set_vscHVBattConsumpSpd2(100);
        evRangeInfo.set_vscHVBattConsumpSpd3(100);
        evRangeInfo.set_vscHVBattConsumpSpd4(100);
        evRangeInfo.set_vscHVBattConsumpSpd5(100);
        evRangeInfo.set_vscHVBattConsumpSpd6(100);
        evRangeInfo.set_vscHVBattConsumpSpd7(100);
        evRangeInfo.set_vscRegenEnergyAvailablex100(0);

        // Set Values to svc::EvRangeMapInforMation
        tmpEvrValue = rawEvInfo->bVSCInitialHVBattEnergy_field_is_updated ? rawEvInfo->getVscInitialHvBattEnergyField() : 0;
        evRangeInfo.set_vscInitialHVBattEnergyx100(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRevisedHVBattEnergy_field_is_updated ? rawEvInfo->getVscRevisedHvBattEnergyField() : 0;
        evRangeInfo.set_vscRevisedHVBattEnergyx100(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVEnergyAscent_field_is_updated ? rawEvInfo->getVschvEnergyAscentField() : 0;
        evRangeInfo.set_vscHVEnergyAscentx10(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVEnergyDescent_field_is_updated ? rawEvInfo->getVschvEnergyDescentField() : 0;
        evRangeInfo.set_vscHVEnergyDescentx10(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVEnergyTimePen_field_is_updated ? rawEvInfo->getVschvEnergyTimePenField() : 0;
        evRangeInfo.set_vscHVEnergyTimePenx100(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRegenEnergyFactor_field_is_updated ? rawEvInfo->getVscRegenEnergyFactorField() : 0;
        evRangeInfo.set_vscRegenEnergyFactor(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCVehAccelFactor_field_is_updated ? rawEvInfo->getVscVehAccelFactorField() : 100;
        evRangeInfo.set_vscVehAccelFactor(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRangeMapRefactrComf_field_is_updated ? rawEvInfo->getVscRangeMapRefactrComfField() : 0;
        evRangeInfo.set_vscRangeMapRefactrComf(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRangeMapRefactrEco_field_is_updated ? rawEvInfo->getVscRangeMapRefactrEcoField() : 0;
        evRangeInfo.set_vscRangeMapRefactrEco(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRangeMapRefactrGMH_field_is_updated ? rawEvInfo->getVscRangeMapRefactrGmhField() : 0;
        evRangeInfo.set_vscRangeMapRefactrGMH(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd1_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd1Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd1(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd2_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd2Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd2(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd3_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd3Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd3(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd4_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd4Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd4(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd5_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd5Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd5(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd6_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd6Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd6(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCHVBattConsumpSpd7_field_is_updated ? rawEvInfo->getVschvBattConsumpSpd7Field() : 100;
        evRangeInfo.set_vscHVBattConsumpSpd7(tmpEvrValue);

        tmpEvrValue = rawEvInfo->bVSCRegenEnergyAvailable_field_is_updated ? rawEvInfo->getVscRegenEnergyAvailableField() : 0;
        evRangeInfo.set_vscRegenEnergyAvailablex100(tmpEvrValue);

        #if SVC_MINOR_VER >= 16
        tmpEvrValue = rawEvInfo->evRangeComfort_field_is_updated ? rawEvInfo->getEvRangeComfortField() : 0;
        evRangeInfo.set_evRangeComfortx10(tmpEvrValue);

        tmpEvrValue = rawEvInfo->evRangeECO_field_is_updated ? rawEvInfo->getEvRangeECOField() : 0;
        evRangeInfo.set_evRangeECOx10(tmpEvrValue);

        tmpEvrValue = rawEvInfo->evRangeGetMeHome_field_is_updated ? rawEvInfo->getEvRangeGetMeHomeField() : 0;
        evRangeInfo.set_evRangeGetMeHomex10(tmpEvrValue);
        #else
        LOGE("[encode_evRangeMapInformation] NGTP Version is lower than 3.16");
        #endif

        // Set svc::EvRangeMapInforMation to svc::ElectricVehicleInformation
        evInfo.set_evRangeMapInformation(evRangeInfo);
    #else

    #endif
    }
    else
    {
      LOGE("bEvRangeMapInformation_is_present is false");
    }
}

inline void encode_nextDepartureTimer(svc::ElectricVehicleInformation &evInfo)
{
    #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
    EvtNextDepartureTimer* rawNextDepartureTimer = EvtNextDepartureTimer::GetInstance();
    svc::NextDepartureTimer evNextDepartureTimer;
    if(EvtElectricVehicleInformation::GetInstance()->bNextDepartureTimer_is_present)
    {
        LOGV("[encode_nextDepartureTimer] nextDepartureTimer updated");
        svc::NextDepartureTimer::nextDepartureTimerDate evNextDepartureTimerDate(  rawNextDepartureTimer->getDateDayField()
                                                                                 , rawNextDepartureTimer->getDateMonthField()
                                                                                 , rawNextDepartureTimer->getDateYearField());
        svc::NextDepartureTimer::nextDepartureTimerTime evNextDepartureTimerTime( rawNextDepartureTimer->getDateHourField()
                                                                                , rawNextDepartureTimer->getDateMinuteField());
        evNextDepartureTimer.set_nextDepartureTimerIsSet(1);
        evNextDepartureTimer.set_nextDepartureTimerDate(evNextDepartureTimerDate);
        evNextDepartureTimer.set_nextDepartureTimerTime(evNextDepartureTimerTime);
        evInfo.set_nextDepartureTimer(evNextDepartureTimer);
    }
    else
    {
        LOGE("[encode_nextDepartureTimer] nextDepartureTimer hasn't updated");
    }
    #else
    LOGE("[encode_nextDepartureTimer] NGTP Version is lower than 3.11");
    #endif
}

inline void encode_evRangePredicStatus(svc::ElectricVehicleInformation &evInfo)
{
    #if SVC_MINOR_VER >= 16 && SVC_MAJOR_VER == 3
    if(EvtElectricVehicleInformation::GetInstance()->evRangePredicStatus_field_is_updated)
    {
        LOGV("[encode_evRangePredicStatus] evRangePredicStatus updated");
        evInfo.set_evRangePredictStatus(EvtElectricVehicleInformation::GetInstance()->getEvRangePredicStatusField());
    }
    else
    {
        LOGE("[encode_evRangePredicStatus] evRangePredicStatus hasn't updated");
        evInfo.omit_evRangePredictStatus();
    }
    #else
    LOGE("[encode_evRangePredicStatus] NGTP Version is lower than 3.16");
    #endif
}

void EvtNgtpMsgManager::encode_BatteryPackDataHeader(svc::HighVoltageBatteryPack& _hvbp)
{
    svc::HighVoltageBatteryPack::batteryPackDataHeader ngtpBpdh(EvtHvBatteryPack::GetInstance()->getBatteryPackDataHeader());
    _hvbp.set_batteryPackDataHeader(ngtpBpdh);
}

void EvtNgtpMsgManager::encode_HighVoltageDataRecord(svc::HighVoltageBatteryPack& _hvbp)
{
    svc::HighVoltageBatteryPack::hvBattRecordList ngtpHvBattRecordList;
    std::vector<svc::HighVoltageDataRecord> _hvdrVec = EvtHvBatteryPack::GetInstance()->getHvDataRecordVec();
    OssIndex _ossIdx = OSS_NOINDEX;

    for( std::vector<svc::HighVoltageDataRecord>::size_type i ; i < _hvdrVec.size() ; i++)
    {
        svc::HighVoltageDataRecord ngtpHvdr(_hvdrVec[i]);
        _ossIdx = ngtpHvBattRecordList.insert_after(_ossIdx, ngtpHvdr);
        LOGV("Encode HVDR - %d", i);
    }
    _hvbp.set_hvBattRecordList(ngtpHvBattRecordList);
}

void EvtNgtpMsgManager::setCurEvPreconditionSettings(svc::EVPreconditionSettings &_EVPreconditionSettings)
{
    if(EvtEVPreconditionSettings::GetInstance()->cur_Signal_FFHPreClimSetting_value == -1)
    {
        _EVPreconditionSettings.omit_fuelFiredHeaterSetting();
    }
    else
    {
        int8_t fuelFiredHeaterSetting = (EvtEVPreconditionSettings::GetInstance()->cur_Signal_FFHPreClimSetting_value == 0 /*FFH disabled*/) ?
                                    svc::EVPreconditionSettings::fuelFiredHeaterSetting::disabled :
                                    svc::EVPreconditionSettings::fuelFiredHeaterSetting::enabled ;
        _EVPreconditionSettings.set_fuelFiredHeaterSetting(fuelFiredHeaterSetting);
    }

    if(EvtEVPreconditionSettings::GetInstance()->cur_Signal_PreClimRangeComfort_value == -1)
    {
        _EVPreconditionSettings.omit_prioritySetting();
    }
    else
    {
        int8_t prioritySetting        = (EvtEVPreconditionSettings::GetInstance()->cur_Signal_PreClimRangeComfort_value == 0 /*Range Prioritised*/) ?
                                    svc::EVPreconditionSettings::prioritySetting::prioritize_range :
                                    svc::EVPreconditionSettings::prioritySetting::prioritize_comfort ;
        _EVPreconditionSettings.set_prioritySetting(prioritySetting);
    }
}

void EvtNgtpMsgManager::setCurEvPreconditionStatus(svc::EVPreconditionStatus &preconditionstatus)
{
    EvtEVPreconditionStatus *_PreconditionStatus = EvtEVPreconditionStatus::GetInstance();

    //set preconditioining Mode.
    preconditionstatus.set_preconditioningMode(_PreconditionStatus->getPreconditioningModeField());
    //set preconditioning Opereeating status.
    preconditionstatus.set_preconditionOperatingStatus(_PreconditionStatus->getPreconditionOperatingStatusField());
    //set preconditioning remaining Reun Time minutes.
    int32_t _remainingRuntime = (EvClimateApp::GetInstance()->getRemainTimerTime(EvClimateApp::ECC_DIRECT_PRECONDITION_TIMER) / SEC_TO_MSEC_CONVERTING_CONSTANT ) / 60 ;
    _remainingRuntime = _remainingRuntime < 255 ? _remainingRuntime : -1;
    preconditionstatus.omit_remainingRuntimeMinutes();
    preconditionstatus.set_remainingRuntimeMinutes(_remainingRuntime);
    //set preconditioning battery status
    preconditionstatus.set_batteryPreconditioningStatus(static_cast<svc::EVPreconditionStatus::batteryPreconditioningStatus>(_PreconditionStatus->getBatteryPreconditioningStatusField()));
    //set preconditioning settings
    EVPreconditionSettings _EVPreconditionSettings;
    setCurEvPreconditionSettings(_EVPreconditionSettings);
    preconditionstatus.omit_preconditionSettings();
    preconditionstatus.set_preconditionSettings(_EVPreconditionSettings);
}

void EvtNgtpMsgManager::setCurElectricVehicleInfo(svc::ElectricVehicleInformation& evInfo)
{
    LOGV("[setCurElectricVehicleInfo] start");
    evInfo.omit_chargingInformation();
    evInfo.omit_chargeSetting();

    encode_chargingStatus(evInfo);
    encode_batteryStatus(evInfo);
    encode_chargingInformation(evInfo);
    encode_energyConsumedLastChargekWh(evInfo);
    encode_chargeSetting(evInfo);

    // TODO: Refectoring as serialize of ElectricVehicleInformation!!!! 20170818
    //Temp code for prevent RVM crush!
    if(evInfo.chargeSetting_is_present())
    {
        LOGV("[setCurElectricVehicleInfo] ChargeSetting exists");
        evInfo.get_chargeSetting()->omit_departureTimers();
        evInfo.get_chargeSetting()->omit_tariffs();
    }
    else
    {
        LOGV("[setCurElectricVehicleInfo] ChargeSetting is omitted");
    }


    // EVRangeMapInformation, wlChargingStatus is available from NGTP Ver 3.11.
    #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
        // EVRangeMapInformation
        if(EvConfigData::GetInstance()->configData["AppModeEVR"] == ENABLED)
        {
        evInfo.omit_evRangeMapInformation();
        encode_evRangeMapInformation(evInfo);
        LOGE("AppModeEVR is ENABLED");
        }
        else
        {
        LOGE("AppModeEVR is DISABLED");
        }

        // wlChargingStatus
        evInfo.omit_wirelessChargingStatus();
        encode_wlChargingStatus(evInfo);

        // nextDepartureTimer
        evInfo.omit_nextDepartureTimer();
        encode_nextDepartureTimer(evInfo);
    #endif
}