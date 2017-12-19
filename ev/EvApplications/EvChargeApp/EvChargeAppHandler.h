// hojunes.lee@lge.com
#ifndef _EV_CHARGE_HANDLER_H_
#define _EV_CHARGE_HANDLER_H_

// #include "Handler.h"
// #include "Message.h"
// #include "SLLooper.h"
#include "EvChargeApp.h"

#define EMPTY   200
#define WORKING -1

class EvChargeAppHandler : public Handler
{
    public:
        EvChargeAppHandler(sp<sl::SLLooper>& looper, EvChargeApp& app)
            :Handler(looper), mEvChargeApp(app) {}
        virtual ~EvChargeAppHandler() {}
        virtual void handleMessage(const sp<sl::Message>& msg);

    private:
        EvChargeApp& mEvChargeApp;

        //Custom Code for EV Charge
        // Handler Called.
        void sendChargeNowSettingToCAN(svc::ChargeSettings* data);
        void sendMaxStateOfChargeToCAN(svc::ChargeSettings* data);
        void sendChargingModeChoiceToCAN(svc::ChargeSettings* data);
        void deleteDepartureTimerToCAN(svc::ChargeSettings* data);
        void sendDepartureTimerToCAN(svc::ChargeSettings* data);
        void sendTariffToCAN(svc::ChargeSettings* data);

        error_t send_ElectricityPeakTimesExtUpdate1(svc::Tariff* pTariff);
        error_t send_ElectricityPeakTimesExtUpdate2(svc::Tariff* pTariff);
        error_t send_ElectricityPeakTimesExtUpdate3(svc::Tariff* pTariff);
        error_t send_ElectricityPeakTimesExtUpdate4(svc::Tariff* pTariff);

        void print_TariffSchedule(svc::Tariff* pTariff);
        void print_TariffZone(svc::Tariff* pTariff, uint8_t tariffZoneNum);
        uint32_t convertEndTime(svc::TariffZone* tariffZone);

        void trigger_Start_FT_Status();
        void trigger_FirstTime_EVC_IMC_WKUP_TIME();
        void trigger_FirstTime_EVC_IGNR_TIME();
        void trigger_FirstTime_EVC_IMC_FT_TIME();

    public:
        enum EvcHandlerEnum{
            NGTP_chargeNowSetting_is_present = 0,
            NGTP_maxStateOfCharge_is_present,
            NGTP_chargingModeChoice_is_present,
            NGTP_departureTimers_is_present,
            NGTP_tariffs_is_present,
            NGTP_CHARGESETTING_REQ = 10,
            Start_FT_Status,
            FirstTime_EVC_IMC_WKUP_TIME,
            FirstTime_EVC_IGNR_TIME,
            FirstTime_EVC_IMC_FT_TIME,
        };
};

enum ECW_CMD{
    ECW_ACTIVE = 0,
    ECW_DEACTIVE
};

#endif // _EV_CHARGE_HANDLER_H_