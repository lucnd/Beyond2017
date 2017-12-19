#include <string.h>
#include "EvtElectricVehicleInformation.h"
#include "NGTPManagerService/NGTPUtil.cpp"
// #include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"
#include "EvtChargeSettings.h"
using namespace svc;

#define LOG_TAG "[EVT]EvtElectricVehicleInformation"
#include "Log.h"


void EvtElectricVehicleInformation::resetAll()
{
    bChargingStatus_is_present = false;
    chargingStatus_field = 0;

    bWlChargingStatus_is_present = false;
    wlChargingStatus_field = 0;

    bBatteryStatus_is_present = false;
    memset(&batteryStatus_field, 0, sizeof(_ElectricVehicleBatteryInformation));
    rangeOnBatteryKm_is_updated = false;
    rangeOnBatteryMiles_is_updated = false;
    rangeCombinedKm_is_updated = false;
    rangeCombinedMiles_is_updated = false;
    stateOfCharge_is_updated = false;

    bChargingInformation_is_present = false;
    memset(&chargingInformation_field, 0, sizeof(_ElectricVehicleChargingInformation));
    minutesToFullyCharged_is_updated = false;
    minutesToBulkCharged_is_updated = false;
    chargingRateSocPerHour_is_updated = false;
    chargingRateMilesPerHour_is_updated = false;
    chargingRateKmPerHour_is_updated = false;

    bEnergyConsumedLastChargekWh_is_preset = false;
    energyConsumedLastChargekWh_field = 0;

    bNextDepartureTimer_is_present = false;

    //pChargeSetting_field = EvtChargeSettings::GetInstance();
    bChargeSetting_field_is_updated = false;

    //EVRangeMapInformation
    bEvRangeMapInformation_is_present = false;
    bVSCInitialHVBattEnergy_field_is_updated = false;
    bVSCRevisedHVBattEnergy_field_is_updated = false;
    bVSCRegenEnergyAvailable_field_is_updated = false;
    bVSCHVEnergyAscent_field_is_updated = false;
    bVSCHVEnergyDescent_field_is_updated = false;
    bVSCHVEnergyTimePen_field_is_updated = false;
    bVSCRegenEnergyFactor_field_is_updated = false;
    bVSCVehAccelFactor_field_is_updated = false;
    bVSCRangeMapRefactrComf_field_is_updated = false;
    bVSCRangeMapRefactrEco_field_is_updated = false;
    bVSCRangeMapRefactrGMH_field_is_updated = false;
    bVSCHVBattConsumpSpd1_field_is_updated = false;
    bVSCHVBattConsumpSpd2_field_is_updated = false;
    bVSCHVBattConsumpSpd3_field_is_updated = false;
    bVSCHVBattConsumpSpd4_field_is_updated = false;
    bVSCHVBattConsumpSpd5_field_is_updated = false;
    bVSCHVBattConsumpSpd6_field_is_updated = false;
    bVSCHVBattConsumpSpd7_field_is_updated = false;
    bEvcEviPushtimeElapsed = true;
    evRangeComfort_field_is_updated = false;
    evRangeECO_field_is_updated = false;
    evRangeGetMeHome_field_is_updated = false;
    evRangePredicStatus_field_is_updated = false;
    setCreationTimeStamp(0);
};

void EvtElectricVehicleInformation::printData()
{
    LOGV("EvtElectricVehicleInformation : \n\t chargingStatus[%X], batteryStatus(rangeOnBatteryKm[%X], rangeOnBatteryMiles[%X], rangeCombinedKm[%X], rangeCombinedMiles[%X], stateOfCharge[%X]),\n\t chargingInformation(minutesToFullyCharged[%X], minutesToBulkCharged[%X], chargingRateSocPerHour[%X], chargingRateMilesPerHour[%X], chargingRateKmPerHour[%X]),\n\t energyConsumedLastChargekWh[%X]", chargingStatus_field, batteryStatus_field.rangeOnBatteryKm_field, batteryStatus_field.rangeOnBatteryMiles_field, batteryStatus_field.rangeCombinedKm_field, batteryStatus_field.rangeCombinedMiles_field, batteryStatus_field.stateOfCharge_field, chargingInformation_field.minutesToFullyCharged_field, chargingInformation_field.minutesToBulkCharged_field, chargingInformation_field.chargingRateSocPerHour_field, chargingInformation_field.chargingRateMilesPerHour_field, chargingInformation_field.chargingRateKmPerHour_field, energyConsumedLastChargekWh_field);
    //EvtChargeSettings::GetInstance()->printData();
};

//1
uint8_t EvtElectricVehicleInformation::getChargingStatusField() const {
    return chargingStatus_field;
}

namespace CS
{
    enum _ChargingStatusDisp{
        NO_MESSAGE = 0,
        BATTERY_FULLY_CHARGED,
        BATTERY_BULK_CHARGED,
        CHARGING,
        WAITING_TO_CHARGE,
        FAULT,
        INITIALISATION,
        PAUSED,
    };
}

void EvtElectricVehicleInformation::setChargingStatusField(
        uint8_t chargingStatusField){
    chargingStatus_field = chargingStatusField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bChargingStatus_is_present = true;
}

#if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
void EvtElectricVehicleInformation::setChargingStatusFromCAN(
        uint8_t ChargingStatusDisp_RX) {
    switch(ChargingStatusDisp_RX)
    {
        case CS::NO_MESSAGE:
        {
            setChargingStatusField(svc::ChargingStatus_notConnected);
            LOGV("Updated ChargingStatus[NO_MESSAGE][%u]",getCreationTimeStamp());
            break;
        }
        case CS::BATTERY_FULLY_CHARGED:
        {
            setChargingStatusField(svc::ChargingStatus_fullyCharged);
            LOGV("Updated ChargingStatus[BATTERY_FULLY_CHARGED][%u]",getCreationTimeStamp());
            break;
        }
        case CS::BATTERY_BULK_CHARGED:
        {
            setChargingStatusField(svc::ChargingStatus_bulkCharged);
            LOGV("Updated ChargingStatus[BATTERY_BULK_CHARGED][%u]",getCreationTimeStamp());
            break;
        }
        case CS::CHARGING:
        {
            setChargingStatusField(svc::ChargingStatus_charging);
            LOGV("Updated ChargingStatus[CHARGING][%u]",getCreationTimeStamp());
            break;
        }
        case CS::WAITING_TO_CHARGE:
        {
            setChargingStatusField(svc::ChargingStatus_waitingToCharge);
            LOGV("Updated ChargingStatus[WAITING_TO_CHARGE][%u]",getCreationTimeStamp());
            break;
        }
        case CS::FAULT:
        {
            setChargingStatusField(svc::ChargingStatus_fault);
            LOGV("Updated ChargingStatus[FAULT][%u]",getCreationTimeStamp());
            break;
        }
        case CS::INITIALISATION:
        {
            setChargingStatusField(svc::ChargingStatus_initialization);
            LOGV("Updated ChargingStatus[INITIALISATION][%u]",getCreationTimeStamp());
            break;
        }
        case CS::PAUSED:
        {
            setChargingStatusField(svc::ChargingStatus_paused);
            LOGV("Updated ChargingStatus[PAUSED][%u]",getCreationTimeStamp());
            break;
        }
        default:
        {
            LOGE("[Error] Receive unknown ChargingStatus from CAN!! [%u]", ChargingStatusDisp_RX);
            break;
        }
    }
}
#else
void EvtElectricVehicleInformation::setChargingStatusFromCAN(
        uint8_t ChargingStatusDisp_RX) {
    switch(ChargingStatusDisp_RX)
    {
        case CS::NO_MESSAGE:
        {
            setChargingStatusField(svc::notConnected);
            LOGV("Updated ChargingStatus[NO_MESSAGE][%u]",getCreationTimeStamp());
            break;
        }
        case CS::BATTERY_FULLY_CHARGED:
        {
            setChargingStatusField(svc::fullyCharged);
            LOGV("Updated ChargingStatus[BATTERY_FULLY_CHARGED][%u]",getCreationTimeStamp());
            break;
        }
        case CS::BATTERY_BULK_CHARGED:
        {
            setChargingStatusField(svc::bulkCharged);
            LOGV("Updated ChargingStatus[BATTERY_BULK_CHARGED][%u]",getCreationTimeStamp());
            break;
        }
        case CS::CHARGING:
        {
            setChargingStatusField(svc::charging);
            LOGV("Updated ChargingStatus[CHARGING][%u]",getCreationTimeStamp());
            break;
        }
        case CS::WAITING_TO_CHARGE:
        {
            setChargingStatusField(svc::waitingToCharge);
            LOGV("Updated ChargingStatus[WAITING_TO_CHARGE][%u]",getCreationTimeStamp());
            break;
        }
        case CS::FAULT:
        {
            setChargingStatusField(svc::fault);
            LOGV("Updated ChargingStatus[FAULT][%u]",getCreationTimeStamp());
            break;
        }
        case CS::INITIALISATION:
        {
            setChargingStatusField(svc::initialization);
            LOGV("Updated ChargingStatus[INITIALISATION][%u]",getCreationTimeStamp());
            break;
        }
        case CS::PAUSED:
        {
            setChargingStatusField(svc::paused);
            LOGV("Updated ChargingStatus[PAUSED][%u]",getCreationTimeStamp());
            break;
        }
        default:
        {
            LOGE("[Error] Receive unknown ChargingStatus from CAN!! [%u]", ChargingStatusDisp_RX);
            break;
        }
    }
}
#endif

namespace WLCS
{
    enum _WlChargingStatusDisp{
        NO_MESSAGE = 0,
        BATTERY_FULLY_CHARGED,
        BATTERY_BULK_CHARGED,
        CHARGING,
        WAITING_TO_CHARGE,
        FAULT,
        INITIALISATION,
        PAD_AUTH_FAIL,
        FOREIGN_OBJ_DETECT,
        PAUSED,
    };
}

uint8_t EvtElectricVehicleInformation::getWlChargingStatusField() const
{
    return wlChargingStatus_field;
}

void EvtElectricVehicleInformation::setWlChargingStatusField(uint8_t wlChargingStatusField)
{
    wlChargingStatus_field = wlChargingStatusField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bWlChargingStatus_is_present = true;
}

void EvtElectricVehicleInformation::setWlChargingStatusFromCAN(uint8_t WlChargingStatusDisp_RX)
{
    #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
    switch(WlChargingStatusDisp_RX)
    {
        case WLCS::NO_MESSAGE:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_notConnected);
            LOGV("Updated WlChargingStatus[NO_MESSAGE][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::BATTERY_FULLY_CHARGED:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_fullyCharged);
            LOGV("Updated WlChargingStatus[BATTERY_FULLY_CHARGED][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::BATTERY_BULK_CHARGED:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_bulkCharged);
            LOGV("Updated WlChargingStatus[BATTERY_BULK_CHARGED][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::CHARGING:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_charging);
            LOGV("Updated WlChargingStatus[CHARGING][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::WAITING_TO_CHARGE:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_waitingToCharge);
            LOGV("Updated WlChargingStatus[WAITING_TO_CHARGE][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::FAULT:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_fault);
            LOGV("Updated WlChargingStatus[FAULT][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::INITIALISATION:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_initialization);
            LOGV("Updated WlChargingStatus[INITIALISATION][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::PAD_AUTH_FAIL:
        {
            setWlChargingStatusField(svc::padAuthFail);
            LOGV("Updated WlChargingStatus[PAD_AUTH_FAIL][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::FOREIGN_OBJ_DETECT:
        {
            setWlChargingStatusField(svc::foreignObjectDetected);
            LOGV("Updated WlChargingStatus[FOREIGN_OBJ_DETECT][%u]",getCreationTimeStamp());
            break;
        }
        case WLCS::PAUSED:
        {
            setWlChargingStatusField(svc::WirelessChargingStatus_paused);
            LOGV("Updated WlChargingStatus[PAUSED][%u]",getCreationTimeStamp());
            break;
        }
        default:
        {
            LOGE("[Error] Receive unknown WlChargingStatus from CAN!! [%u]", WlChargingStatusDisp_RX);
            break;
        }
    }
    #else
    LOGE("[setWlChargingStatusFromCAN] NGTP ver is lower than 3.11");
    #endif
}

uint32_t EvtElectricVehicleInformation::getRangeOnBatteryKmField() const {
    return batteryStatus_field.rangeOnBatteryKm_field;
}

void EvtElectricVehicleInformation::setRangeOnBatteryKmField(
        uint32_t rangeOnBatteryKmField) {
    batteryStatus_field.rangeOnBatteryKm_field = rangeOnBatteryKmField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bBatteryStatus_is_present = true;
    rangeOnBatteryKm_is_updated = true;
}

void EvtElectricVehicleInformation::setRangeOnBatteryKmFromCAN(uint32_t EVRangeDisp_RX)
{
    setRangeOnBatteryKmField(EVRangeDisp_RX);
    LOGV("Updated RangeOnBatteryKm[%u],[%u]", getRangeOnBatteryKmField(), getCreationTimeStamp());
}

//3
uint32_t EvtElectricVehicleInformation::getRangeOnBatteryMilesField() const {
    return batteryStatus_field.rangeOnBatteryMiles_field;
}

void EvtElectricVehicleInformation::setRangeOnBatteryMilesField(
        uint32_t rangeOnBatteryMilesField) {
    batteryStatus_field.rangeOnBatteryMiles_field = rangeOnBatteryMilesField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bBatteryStatus_is_present = true;
    rangeOnBatteryMiles_is_updated = true;
}

void EvtElectricVehicleInformation::setRangeOnBatteryMilesFromCAN(
        uint32_t EVRangeMilesDisp_RX) {
    setRangeOnBatteryMilesField(EVRangeMilesDisp_RX);
    LOGV("Updated RangeOnBatteryMiles[%u],[%u]", getRangeOnBatteryMilesField(), getCreationTimeStamp());
}

//4
uint32_t EvtElectricVehicleInformation::getRangeCombinedKmField() const {
    return batteryStatus_field.rangeCombinedKm_field;
}

void EvtElectricVehicleInformation::setRangeCombinedKmField(
        uint32_t rangeCombinedKmField) {
    batteryStatus_field.rangeCombinedKm_field = rangeCombinedKmField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bBatteryStatus_is_present = true;
    rangeCombinedKm_is_updated = true;
}

void EvtElectricVehicleInformation::setRangeCombinedKmFromCAN(
        uint32_t CombinedRangeDisp_RX) {
    setRangeCombinedKmField(CombinedRangeDisp_RX);
    LOGV("Updated RangeCombinedKm[%u],[%u]", getRangeCombinedKmField(), getCreationTimeStamp());
}

//5
uint32_t EvtElectricVehicleInformation::getRangeCombinedMilesField() const {
    return batteryStatus_field.rangeCombinedMiles_field;
}

void EvtElectricVehicleInformation::setRangeCombinedMilesField(
        uint32_t rangeCombinedMilesField) {
    batteryStatus_field.rangeCombinedMiles_field = rangeCombinedMilesField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bBatteryStatus_is_present = true;
    rangeCombinedMiles_is_updated = true;
}

void EvtElectricVehicleInformation::setRangeCombinedMilesFromCAN(
        uint32_t CombinedRangeMilesDisp_RX) {
    setRangeCombinedMilesField(CombinedRangeMilesDisp_RX);
    LOGV("Updated RangeCombinedMiles[%u],[%u]", getRangeCombinedMilesField(), getCreationTimeStamp());
}

//6
uint32_t EvtElectricVehicleInformation::getStateOfChargeField() const {
    return batteryStatus_field.stateOfCharge_field;
}

void EvtElectricVehicleInformation::setStateOfChargeField(
        uint32_t stateOfChargeField) {
    batteryStatus_field.stateOfCharge_field = stateOfChargeField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bBatteryStatus_is_present = true;
    stateOfCharge_is_updated = true;
}

void EvtElectricVehicleInformation::setStateOfChargeFromCAN(
        uint8_t HVBatteryUsableSOCDisp_RX) {
    setStateOfChargeField(HVBatteryUsableSOCDisp_RX);
    LOGV("Updated StateOfCharge[%u],[%u]", getStateOfChargeField(), getCreationTimeStamp());
    if(HVBatteryUsableSOCDisp_RX > 100){
        LOGE("Wrong CAN signal. StateOfCharge[%d] is not able to bigger than 100%%",HVBatteryUsableSOCDisp_RX);
    }
}

uint32_t EvtElectricVehicleInformation::getLatestStateOfChargeField() const
{
    return latest_stateOfCharge_field;
}

void EvtElectricVehicleInformation::setLatestStateOfChargeField(uint32_t latestSocField)
{
    latest_stateOfCharge_field = latestSocField;
    LOGV("LatestStateOfChargeField to compare next upcoming value[%u]", latestSocField);
}

//7
uint32_t EvtElectricVehicleInformation::getMinutesToFullyChargedField() const {
    return chargingInformation_field.minutesToFullyCharged_field;
}

void EvtElectricVehicleInformation::setMinutesToFullyChargedField(
        uint32_t minutesToFullyChargedField) {
    chargingInformation_field.minutesToFullyCharged_field = minutesToFullyChargedField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bChargingInformation_is_present = true;
    minutesToFullyCharged_is_updated = true;
}
void EvtElectricVehicleInformation::setMinutesToFullyChargedFromCAN(
        uint8_t _Hours, uint8_t _Minute) {
    setMinutesToFullyChargedField(_Hours*60 + _Minute);
    LOGV("Updated MinutesToFullyCharged[%u],[%u]", getMinutesToFullyChargedField(), getCreationTimeStamp());
}

//8
uint32_t EvtElectricVehicleInformation::getMinutesToBulkChargedField() const {
    return chargingInformation_field.minutesToBulkCharged_field;
}

void EvtElectricVehicleInformation::setMinutesToBulkChargedField(
        uint32_t minutesToBulkChargedField) {
    chargingInformation_field.minutesToBulkCharged_field = minutesToBulkChargedField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bChargingInformation_is_present = true;
    minutesToBulkCharged_is_updated = true;
}
void EvtElectricVehicleInformation::setMinutesToBulkChargedFromCAN(
       uint8_t _Hours, uint8_t _Minute){
    setMinutesToBulkChargedField(_Hours*60 + _Minute);
    LOGV("Updated MinutesToBulkCharged[%u],[%u]", getMinutesToBulkChargedField(), getCreationTimeStamp());
}

//9
uint32_t EvtElectricVehicleInformation::getChargingRateSocPerHourField() const {
    return chargingInformation_field.chargingRateSocPerHour_field;
}

void EvtElectricVehicleInformation::setChargingRateSocPerHourField(
        uint32_t chargingRateSocPerHourField) {
    chargingInformation_field.chargingRateSocPerHour_field = chargingRateSocPerHourField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bChargingInformation_is_present = true;
    chargingRateSocPerHour_is_updated = true;
}

void EvtElectricVehicleInformation::setChargingRateSocPerHourFromCAN(
        uint32_t HVBattChrgeRateSOCDisp_RX) {
    setChargingRateSocPerHourField(HVBattChrgeRateSOCDisp_RX);
    LOGV("Updated ChargingRateSocPerHour[%u],[%u]", getChargingRateSocPerHourField(), getCreationTimeStamp());
}

//10
uint32_t EvtElectricVehicleInformation::getChargingRateMilesPerHourField() const {
    return chargingInformation_field.chargingRateMilesPerHour_field;
}

void EvtElectricVehicleInformation::setChargingRateMilesPerHourField(
        uint32_t chargingRateMilesPerHourField) {
    chargingInformation_field.chargingRateMilesPerHour_field = chargingRateMilesPerHourField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bChargingInformation_is_present = true;
    chargingRateMilesPerHour_is_updated = true;
}

void EvtElectricVehicleInformation::setChargingRateMilesPerHourFromCAN(
        uint32_t HVBattChrgRateMileDisp_RX){
    setChargingRateMilesPerHourField(HVBattChrgRateMileDisp_RX);
    LOGV("Updated ChargingRateMilesPerHour[%u],[%u]", getChargingRateMilesPerHourField(), getCreationTimeStamp());
}

//11
uint32_t EvtElectricVehicleInformation::getChargingRateKmPerHourField() const {
    return chargingInformation_field.chargingRateKmPerHour_field;
}

void EvtElectricVehicleInformation::setChargingRateKmPerHourField(
        uint32_t chargingRateKmPerHourField) {
    chargingInformation_field.chargingRateKmPerHour_field = chargingRateKmPerHourField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bChargingInformation_is_present = true;
    chargingRateKmPerHour_is_updated = true;
}

void EvtElectricVehicleInformation::setChargingRateKmPerHourFromCAN(
        uint32_t HVBattChargeRateKmDisp_RX){
    setChargingRateKmPerHourField(HVBattChargeRateKmDisp_RX);
    LOGV("Updated ChargingRateKmPerHour[%u],[%u]", getChargingRateKmPerHourField(), getCreationTimeStamp());
}

//12
uint32_t EvtElectricVehicleInformation::getEnergyConsumedLastChargekWhField() const {
    return energyConsumedLastChargekWh_field;
}

void EvtElectricVehicleInformation::setEnergyConsumedLastChargekWhField(
        uint32_t energyConsumedLastChargekWhField) {
    energyConsumedLastChargekWh_field = energyConsumedLastChargekWhField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEnergyConsumedLastChargekWh_is_preset = true;
}
void EvtElectricVehicleInformation::setEnergyConsumedLastChargekWhFromCAN(
        uint32_t kWhConsumedLastCharge_RX){
    setEnergyConsumedLastChargekWhField(kWhConsumedLastCharge_RX);
    LOGV("Updated EnergyConsumedLastChargekWh[%u],[%u]", getEnergyConsumedLastChargekWhField(), getCreationTimeStamp());
}

//13
svc::ChargeSettings EvtElectricVehicleInformation::getChargeSetting() const{
    LOGI("bChargeSetting_field_is_updated [%s]", bChargeSetting_field_is_updated == true ? "True" : "False");
    return ChargeSetting_field;
}

void EvtElectricVehicleInformation::setChargeSetting(svc::ChargeSettings &_ChargeSetting)
{
    bChargeSetting_field_is_updated = true;
    EvtChargeSettings::GetInstance()->setChargeSettingsFromNGTP(_ChargeSetting);
}

//14
uint32_t EvtElectricVehicleInformation::getCreationTimeStamp() const {
    return creationTimeStamp;
}

void EvtElectricVehicleInformation::setCreationTimeStamp(
        uint32_t _creationTimeStamp) {
    creationTimeStamp = _creationTimeStamp;
}

//EVRangeMapInformation
uint32_t EvtElectricVehicleInformation::getVscInitialHvBattEnergyField() const {
    return VSCInitialHVBattEnergy_field;
}

void EvtElectricVehicleInformation::setVscInitialHvBattEnergyField(uint32_t vscInitialHvBattEnergyField) {
    VSCInitialHVBattEnergy_field = vscInitialHvBattEnergyField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCInitialHVBattEnergy_field_is_updated = true;
    LOGV("Updated VSCInitialHVBattEnergy[%u],[%u]", getVscInitialHvBattEnergyField(), getCreationTimeStamp());
}
uint32_t EvtElectricVehicleInformation::getVscRevisedHvBattEnergyField() const
{
    return VSCRevisedHVBattEnergy_field;
}

void EvtElectricVehicleInformation::setVscRevisedHvBattEnergyField(uint32_t vscRevisedHvBattEnergyField)
{
    VSCRevisedHVBattEnergy_field = vscRevisedHvBattEnergyField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCRevisedHVBattEnergy_field_is_updated = true;
    LOGV("Updated vscRevisedHvBattEnergyField[%u],[%u]", getVscRevisedHvBattEnergyField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVscRegenEnergyAvailableField() const
{
    return VSCRegenEnergyAvailable_field;
}

void EvtElectricVehicleInformation::setVscRegenEnergyAvailableField(uint32_t vscRegenEnergyAvailableField)
{
    VSCRegenEnergyAvailable_field = vscRegenEnergyAvailableField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCRegenEnergyAvailable_field_is_updated = true;
    LOGV("Updated vscRegenEnergyAvailableField[%u],[%u]", getVscRegenEnergyAvailableField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvEnergyAscentField() const {
	return VSCHVEnergyAscent_field;
}

void EvtElectricVehicleInformation::setVschvEnergyAscentField(uint32_t vschvEnergyAscentField) {
	VSCHVEnergyAscent_field = vschvEnergyAscentField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVEnergyAscent_field_is_updated = true;
    LOGV("Updated vschvEnergyAscentField[%u],[%u]", getVschvEnergyAscentField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvEnergyDescentField() const {
	return VSCHVEnergyDescent_field;
}

void EvtElectricVehicleInformation::setVschvEnergyDescentField(uint32_t vschvEnergyDescentField) {
	VSCHVEnergyDescent_field = vschvEnergyDescentField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVEnergyDescent_field_is_updated = true;
    LOGV("Updated vschvEnergyDescentField[%u],[%u]", getVschvEnergyDescentField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvEnergyTimePenField() const {
	return VSCHVEnergyTimePen_field;
}

void EvtElectricVehicleInformation::setVschvEnergyTimePenField(uint32_t vschvEnergyTimePenField) {
	VSCHVEnergyTimePen_field = vschvEnergyTimePenField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVEnergyTimePen_field_is_updated = true;
    LOGV("Updated vschvEnergyTimePenField[%u],[%u]", getVschvEnergyTimePenField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVscRegenEnergyFactorField() const {
	return VSCRegenEnergyFactor_field;
}

void EvtElectricVehicleInformation::setVscRegenEnergyFactorField(uint32_t vscRegenEnergyFactorField) {
	VSCRegenEnergyFactor_field = vscRegenEnergyFactorField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCRegenEnergyFactor_field_is_updated = true;
    LOGV("Updated evRangeComfortField[%u],[%u]", getVscRegenEnergyFactorField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVscVehAccelFactorField() const {
	return VSCVehAccelFactor_field;
}

void EvtElectricVehicleInformation::setVscVehAccelFactorField(uint32_t vscVehAccelFactorField) {
	VSCVehAccelFactor_field = vscVehAccelFactorField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCVehAccelFactor_field_is_updated = true;
    LOGV("Updated vscVehAccelFactorField[%u],[%u]", getVscVehAccelFactorField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVscRangeMapRefactrComfField() const {
	return VSCRangeMapRefactrComf_field;
}

void EvtElectricVehicleInformation::setVscRangeMapRefactrComfField(uint32_t vscRangeMapRefactrComfField) {
	VSCRangeMapRefactrComf_field = vscRangeMapRefactrComfField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCRangeMapRefactrComf_field_is_updated = true;
    LOGV("Updated vscRangeMapRefactrComfField[%u],[%u]", getVscRangeMapRefactrComfField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVscRangeMapRefactrEcoField() const {
	return VSCRangeMapRefactrEco_field;
}

void EvtElectricVehicleInformation::setVscRangeMapRefactrEcoField(uint32_t vscRangeMapRefactrEcoField) {
	VSCRangeMapRefactrEco_field = vscRangeMapRefactrEcoField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCRangeMapRefactrEco_field_is_updated = true;
    LOGV("Updated vscRangeMapRefactrEcoField[%u],[%u]", getVscRangeMapRefactrEcoField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVscRangeMapRefactrGmhField() const {
	return VSCRangeMapRefactrGMH_field;
}

void EvtElectricVehicleInformation::setVscRangeMapRefactrGmhField(uint32_t vscRangeMapRefactrGmhField) {
	VSCRangeMapRefactrGMH_field = vscRangeMapRefactrGmhField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCRangeMapRefactrGMH_field_is_updated = true;
    LOGV("Updated vscRangeMapRefactrGmhField[%u],[%u]", getVscRangeMapRefactrGmhField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd1Field() const {
	return VSCHVBattConsumpSpd1_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd1Field(uint32_t vschvBattConsumpSpd1Field) {
	VSCHVBattConsumpSpd1_field = vschvBattConsumpSpd1Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd1_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd1Field[%u],[%u]", getVschvBattConsumpSpd1Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd2Field() const {
	return VSCHVBattConsumpSpd2_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd2Field(uint32_t vschvBattConsumpSpd2Field) {
	VSCHVBattConsumpSpd2_field = vschvBattConsumpSpd2Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd2_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd2Field[%u],[%u]", getVschvBattConsumpSpd2Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd3Field() const {
	return VSCHVBattConsumpSpd3_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd3Field(uint32_t vschvBattConsumpSpd3Field) {
	VSCHVBattConsumpSpd3_field = vschvBattConsumpSpd3Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd3_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd3Field[%u],[%u]", getVschvBattConsumpSpd3Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd4Field() const {
	return VSCHVBattConsumpSpd4_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd4Field(uint32_t vschvBattConsumpSpd4Field) {
	VSCHVBattConsumpSpd4_field = vschvBattConsumpSpd4Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd4_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd4Field[%u],[%u]", getVschvBattConsumpSpd4Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd5Field() const {
	return VSCHVBattConsumpSpd5_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd5Field(uint32_t vschvBattConsumpSpd5Field) {
	VSCHVBattConsumpSpd5_field = vschvBattConsumpSpd5Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd5_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd5Field[%u],[%u]", getVschvBattConsumpSpd5Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd6Field() const {
	return VSCHVBattConsumpSpd6_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd6Field(uint32_t vschvBattConsumpSpd6Field) {
	VSCHVBattConsumpSpd6_field = vschvBattConsumpSpd6Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd6_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd6Field[%u],[%u]", getVschvBattConsumpSpd6Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getVschvBattConsumpSpd7Field() const {
	return VSCHVBattConsumpSpd7_field;
}

void EvtElectricVehicleInformation::setVschvBattConsumpSpd7Field(uint32_t vschvBattConsumpSpd7Field) {
	VSCHVBattConsumpSpd7_field = vschvBattConsumpSpd7Field;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    bVSCHVBattConsumpSpd7_field_is_updated = true;
    LOGV("Updated vschvBattConsumpSpd7Field[%u],[%u]", getVschvBattConsumpSpd7Field(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getEvRangeComfortField() const
{
    return evRangeComfort_field;
}

void EvtElectricVehicleInformation::setEvRangeComfortField(uint32_t _evRangeComfort)
{
    evRangeComfort_field = _evRangeComfort;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    evRangeComfort_field_is_updated = true;
    LOGV("Updated evRangeComfort Field[%u],[%u]", getEvRangeComfortField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getEvRangeECOField() const
{
    return evRangeECO_field;
}

void EvtElectricVehicleInformation::setEvRangeECOField(uint32_t _evRangeECO)
{
    evRangeECO_field = _evRangeECO;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    evRangeECO_field_is_updated = true;
    LOGV("Updated evRangeECO Field[%u],[%u]", getEvRangeECOField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getEvRangeGetMeHomeField() const
{
    return evRangeGetMeHome_field;
}

void EvtElectricVehicleInformation::setEvRangeGetMeHomeField(uint32_t _evRangeGetMeHome)
{
    evRangeGetMeHome_field = _evRangeGetMeHome;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    bEvRangeMapInformation_is_present = true;
    evRangeGetMeHome_field_is_updated = true;
    LOGV("Updated evRangeGetMeHome Field[%u],[%u]", getEvRangeGetMeHomeField(), getCreationTimeStamp());
}

uint32_t EvtElectricVehicleInformation::getEvRangePredicStatusField() const
{
    return evRangePredicStatus_field;
}

void EvtElectricVehicleInformation::setEvRangePredicStatusField(uint32_t _evRangePredicStatus)
{
    evRangePredicStatus_field = _evRangePredicStatus;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    evRangePredicStatus_field_is_updated = true;
    LOGV("Updated evRangePredicStatus Field[%u],[%u]", getEvRangePredicStatusField(), getCreationTimeStamp());
}

// Aug 7th 2017: TCU need to convert enum value integer is not mapped between CAN and NGTP
int32_t EvtElectricVehicleInformation::convertWLCSFromCanToNgtp(uint32_t _valueCan)
{
    int32_t valueNgtp;
    switch(_valueCan)
    {
        case WLCS::NO_MESSAGE:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_notConnected;
            LOGV("[convertWLCS] NO_MESSAGE(CAN) notConnected C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::BATTERY_FULLY_CHARGED:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_fullyCharged;
            LOGV("[convertWLCS] fullyCharged C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::BATTERY_BULK_CHARGED:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_bulkCharged;
            LOGV("[convertWLCS] bulkCharged C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::CHARGING:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_charging;
            LOGV("[convertWLCS] charging C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::WAITING_TO_CHARGE:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_waitingToCharge;
            LOGV("[convertWLCS] waitingToCharg C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::FAULT:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_fault;
            LOGV("[convertWLCS] fault C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::INITIALISATION:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_initialization;
            LOGV("[convertWLCS] initialization C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::PAD_AUTH_FAIL:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::padAuthFail;
            LOGV("[convertWLCS] padAuthFail C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::FOREIGN_OBJ_DETECT:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::foreignObjectDetected;
            LOGV("[convertWLCS] foreignObjectDetected C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case WLCS::PAUSED:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_paused;
            LOGV("[convertWLCS] paused C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        default:
        {
            valueNgtp = (int32_t)svc::WirelessChargingStatus::WirelessChargingStatus_unknown;
            LOGE("[convertWLCS] unknown C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
    }
    return valueNgtp;
}

int32_t EvtElectricVehicleInformation::convertCSFromCanToNgtp(uint32_t _valueCan)
{
    int32_t valueNgtp;
    switch(_valueCan)
    {
        case CS::NO_MESSAGE:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_notConnected;
            LOGV("[convertCS] NO_MESSAGE(CAN) notConnected C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::BATTERY_FULLY_CHARGED:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_fullyCharged;
            LOGV("[convertCS] fullyCharged C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::BATTERY_BULK_CHARGED:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_bulkCharged;
            LOGV("[convertCS] bulkCharged C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::CHARGING:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_charging;
            LOGV("[convertCS] charging C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::WAITING_TO_CHARGE:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_waitingToCharge;
            LOGV("[convertCS] waitingToCharge C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::FAULT:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_fault;
            LOGV("[convertCS] fault C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::INITIALISATION:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_initialization;
            LOGV("[convertCS] initialization C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        case CS::PAUSED:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_paused;
            LOGV("[convertCS] paused C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
        default:
        {
            valueNgtp = (int32_t)svc::ChargingStatus::ChargingStatus_unknown;
            LOGE("[convertCS] unknown C[%d] -> N[%d]",_valueCan, valueNgtp);
            break;
        }
    }
    return valueNgtp;
}

