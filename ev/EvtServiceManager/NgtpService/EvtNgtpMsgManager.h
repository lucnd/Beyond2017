#ifndef _EVT_NGTPMSGMGR_H_
#define _EVT_NGTPMSGMGR_H_
#include <map>
#include "NGTPManagerService/NGTPManagerService.h"
#include "NGTPManagerService/INGTPManagerService.h"
#include "NGTPManagerService/INGTPReceiver.h"
#include "NGTPManagerService/InternalNGTPData.h"
#include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"
#include "EvtSingleton.h"
#include "EvConfigData.h"


using namespace svc;
typedef std::map<std::string, uint32_t> EventCreationTime; //{second, millisec}

class EvtNgtpMsgManager : public EvtSingleton<EvtNgtpMsgManager>
{
public:
    EvtNgtpMsgManager();
    ~EvtNgtpMsgManager(){};

    void decodeMsg_PreconditionSettings(int8_t _fuelFiredHeaterSetting, int8_t _prioritySetting, uint32_t _msgCreationTime);
    void rcv_NgtpMessage(Buffer& buf);
    void send_NgtpMessage(uint8_t ngtpType, uint8_t ngtpResult);
    void setCurElectricVehicleInfo(svc::ElectricVehicleInformation& evInfo);
    void setLoggingNgtpDecoder(bool bState); // bState = ENABLE/DISABLE

    svc::ServiceMessageType getMessageType() { return mMessageType; };
    svc::Acknowledge getAckowledge() { return mAppAck; };

    void activeECW();
    void deactiveECW();

    void setNGTPService(sp<INGTPManagerService> service)
    {
        m_ngtpMgrService = service;
    };

    uint8_t mEvtMsgType;
    uint8_t ngtp_send_type;
    char evt_ngtp_sms_number[20];

    enum TYPE_NGTP_REQ{
        UNKNOWN_MSG = 0,
        CHARGE_SETTINGS = 1,
        PRECND_START,
        PRECND_STOP,
        PRECND_SETTING,
        EVT_MSG_ENUM_END
    };

    enum TYPE_NGTP_RESP{
        CHARGE_SETTING_RESP = 1,
        CHARGE_SETTING_UPDATE,
        CHARGING_STATUS_UPDATE,
        CHARGING_STATUS_UPDATE_ALERT,
        PRECONDITION_START_RESP,
        PRECONDITION_STOP_RESP,
        PRECONDITION_SETTING_RESP,
        PRECONDITION_SETTING_UPDATE,
        PRECONDITON_UPDATE_ALERT,
        BATTERY_INFORMATION_UPDATE
    };

    enum NGTP_SERVICE_TYPE{ // Service Type - EVT spec defined Enum Naming.
        EVT_UNKNOWN = 0,
        EVT_REMOTE_VEHICLE_STATUS = 11,
        EVT_CHARGE_PROFILE = 31,
        EVT_PRECONDITIONING = 46,
        EVT_REMOTE_BATTERY_STATUS=140
    };

    enum NGTP_SEND_TYPE{ // Service Type - EVT spec defined Enum Naming.
        NGTP_UDP = 0,
        NGTP_SMS,
        NGTP_HTTPS,
        NGTP_HTTP
    };

private:
    sp<INGTPManagerService> m_ngtpMgrService;
    svc::ServiceMessageType mMessageType;
    svc::Acknowledge mAppAck;
    std::vector<EventCreationTime> mArrayECT[EVT_MSG_ENUM_END];
    bool bLoggingNgtpDecoder;

    friend class EvAppBinderService;

    int8_t decodeMessage(uint8_t *buffer, uint32_t size, svc::NGTPDownlinkCore *pDownlink);
    bool encodeMessage(); // TODO implement.

    void decodeMsg_PreconditionStart(uint32_t _targetTemperatureCelsiusX10, uint32_t _msgCreationTime);
    void decodeMsg_ChargeSettings(svc::ChargeSettings* msg, uint32_t _msgCreationTime);
    void sendNgtp_ChargeSettingResp(uint8_t typeUplinkMsg, uint8_t ngtpResult);
    void sendNgtp_PreconditionSettingResp(uint8_t typeUplinkMsg, uint8_t ngtpResult);
    void sendNgtp_ElectricVehicleInformation(uint8_t typeUplinkMsg, uint8_t ngtpResult);
    void sendNgtp_PreconditionStartStopResp(uint8_t typeUplinkMsg, uint8_t ngtpResult);
    void sendNGTP_Alerts(uint8_t typeUplinkMsg, uint8_t ngtpResult);
    void sendNgtp_PreconditionUpdateAlerts(uint8_t typeUplinkMsg, uint8_t ngtpResult);
    void sendNgtp_PreconditionSettingUpdate(uint8_t typeUplinkMsg);
    void sendNgtp_BatteryInformationUpdate(uint8_t typeUplinkMsg);
    bool IsValidNgtpMsg(InternalNGTPData *pNgtp, svc::NGTPServiceData* pServiceData);
    void setEventCreationTime(uint8_t msgType, InternalNGTPData *pNgtp);
    EventCreationTime getEventCreationTime(uint8_t msgType);

    // Internal encode
    void setNgtpUplinkData_preconditionStatus(svc::NGTPUplinkCore &uplink, uint8_t ngtpResult);
    void setCurEvPreconditionSettings(svc::EVPreconditionSettings &_EVPreconditionSettings);
    void setCurEvPreconditionStatus(svc::EVPreconditionStatus &preconditionstatus);

    void setNgtpUplinkData_HighVoltageBatteryPack(svc::NGTPUplinkCore &uplink);
    void encode_BatteryPackDataHeader(svc::HighVoltageBatteryPack& _hvbp);
    void encode_HighVoltageDataRecord(svc::HighVoltageBatteryPack& _hvbp);

    bool encodeNgtpServiceData(PEREncodedBuffer &encodedSvcData, svc::NGTPUplinkCore &uplink, svc::ServiceMessageType svcMsgType, svc::Acknowledge _appAck, bool bOmitAppAck);
};

#endif // _EVT_NGTPMSGMGR_H_
