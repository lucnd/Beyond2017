//ElectricVehicleInformation
#ifndef _EV_ELECTRIC_VEHICLE_INFORMATION_H_
#define _EV_ELECTRIC_VEHICLE_INFORMATION_H_

#include "EvtSingleton.h"
// #include "EvtChargeSettings.h"
#include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"

class EvtElectricVehicleInformation : public EvtSingleton<EvtElectricVehicleInformation>
{
    typedef struct
    {
        uint32_t rangeOnBatteryKm_field;
        uint32_t rangeOnBatteryMiles_field;
        uint32_t rangeCombinedKm_field;
        uint32_t rangeCombinedMiles_field;
        uint32_t stateOfCharge_field;
    } _ElectricVehicleBatteryInformation;

    typedef struct
    {
        uint32_t minutesToFullyCharged_field;
        uint32_t minutesToBulkCharged_field;
        uint32_t chargingRateSocPerHour_field;
        uint32_t chargingRateMilesPerHour_field;
        uint32_t chargingRateKmPerHour_field;
    } _ElectricVehicleChargingInformation;



    public:
        EvtElectricVehicleInformation(){};
        ~EvtElectricVehicleInformation(){};

    // EVRangeMapInformation (Based on SRD_TCU_S09_v1_0_054_EVT.pdf)
    uint32_t VSCInitialHVBattEnergy_field;
    uint32_t VSCRegenEnergyAvailable_field;
    uint32_t VSCRevisedHVBattEnergy_field;
    uint32_t VSCHVEnergyAscent_field;
    uint32_t VSCHVEnergyDescent_field;
    uint32_t VSCHVEnergyTimePen_field;
    uint32_t VSCRegenEnergyFactor_field;
    uint32_t VSCVehAccelFactor_field;
    uint32_t VSCRangeMapRefactrComf_field;
    uint32_t VSCRangeMapRefactrEco_field;
    uint32_t VSCRangeMapRefactrGMH_field;
    uint32_t VSCHVBattConsumpSpd1_field;
    uint32_t VSCHVBattConsumpSpd2_field;
    uint32_t VSCHVBattConsumpSpd3_field;
    uint32_t VSCHVBattConsumpSpd4_field;
    uint32_t VSCHVBattConsumpSpd5_field;
    uint32_t VSCHVBattConsumpSpd6_field;
    uint32_t VSCHVBattConsumpSpd7_field;
    uint32_t evRangeComfort_field;
    uint32_t evRangeECO_field;
    uint32_t evRangeGetMeHome_field;
    uint32_t evRangePredicStatus_field;

    void resetAll();
    void printData();

    //chargingStatus_field
    uint8_t getChargingStatusField() const;
    void setChargingStatusField(uint8_t chargingStatusField);
    void setChargingStatusFromCAN(uint8_t ChargingStatusDisp_RX);

    //wlChargingStatus_field
    uint8_t getWlChargingStatusField() const;
    void setWlChargingStatusField(uint8_t wlChargingStatusField);
    void setWlChargingStatusFromCAN(uint8_t WlChargingStatusDisp_RX);

    //batteryStatus_field
    uint32_t getRangeOnBatteryKmField() const;
    void setRangeOnBatteryKmField(uint32_t rangeOnBatteryKmField);
    void setRangeOnBatteryKmFromCAN(uint32_t EVRangeDisp_RX);
    uint32_t getRangeOnBatteryMilesField() const;
    void setRangeOnBatteryMilesField(uint32_t rangeOnBatteryMilesField);
    void setRangeOnBatteryMilesFromCAN(uint32_t EVRangeMilesDisp_RX);
    uint32_t getRangeCombinedKmField() const;
    void setRangeCombinedKmField(uint32_t rangeCombinedKmField);
    void setRangeCombinedKmFromCAN(uint32_t CombinedRangeDisp_RX);
    uint32_t getRangeCombinedMilesField() const;
    void setRangeCombinedMilesField(uint32_t rangeCombinedMilesField);
    void setRangeCombinedMilesFromCAN(uint32_t CombinedRangeMilesDisp_RX);
    uint32_t getStateOfChargeField() const;
    void setStateOfChargeField(uint32_t stateOfChargeField);
    void setStateOfChargeFromCAN(uint8_t HVBatteryUsableSOCDisp_RX);
    // SRD_ReqEVC0040_v1
    uint32_t getLatestStateOfChargeField() const;
    void setLatestStateOfChargeField(uint32_t stateOfChargeField);

    //chargingInformation_field
    uint32_t getMinutesToFullyChargedField() const;
    void setMinutesToFullyChargedField(uint32_t minutesToFullyChargedField);
    void setMinutesToFullyChargedFromCAN(uint8_t _Hours, uint8_t _Minute);

    uint32_t getMinutesToBulkChargedField() const;
    void setMinutesToBulkChargedField(uint32_t minutesToBulkChargedField);
    void setMinutesToBulkChargedFromCAN(uint8_t _Hours, uint8_t _Minute);

    uint32_t getChargingRateSocPerHourField() const;
    void setChargingRateSocPerHourField(uint32_t chargingRateSocPerHourField);
    void setChargingRateSocPerHourFromCAN(uint32_t HVBattChrgeRateSOCDisp_RX);

    uint32_t getChargingRateMilesPerHourField() const;
    void setChargingRateMilesPerHourField(uint32_t chargingRateMilesPerHourField);
    void setChargingRateMilesPerHourFromCAN(uint32_t HVBattChrgRateMileDisp_RX);

    uint32_t getChargingRateKmPerHourField() const;
    void setChargingRateKmPerHourField(uint32_t chargingRateKmPerHourField);
    void setChargingRateKmPerHourFromCAN(uint32_t HVBattChargeRateKmDisp_RX);

    //energyConsumedLastChargekWh_field
    uint32_t getEnergyConsumedLastChargekWhField() const;
    void setEnergyConsumedLastChargekWhField(uint32_t energyConsumedLastChargekWhField);
    void setEnergyConsumedLastChargekWhFromCAN(uint32_t kWhConsumedLastCharge_RX);

    // creationTimeStamp
    uint32_t getCreationTimeStamp() const;
    void setCreationTimeStamp(uint32_t _creationTimeStamp);

    //ChargeSetting_field
    svc::ChargeSettings getChargeSetting() const;
    void setChargeSetting(svc::ChargeSettings &_ChargeSetting);

    //EVRangeMapInformation
    uint32_t getVscInitialHvBattEnergyField() const ;
	void setVscInitialHvBattEnergyField(uint32_t vscInitialHvBattEnergyField) ;
    uint32_t getVscRevisedHvBattEnergyField() const ;
	void setVscRevisedHvBattEnergyField(uint32_t vscRevisedHvBattEnergyField) ;
	uint32_t getVscRegenEnergyAvailableField() const ;
	void setVscRegenEnergyAvailableField(uint32_t vscRegenEnergyAvailableField);
	uint32_t getVschvEnergyAscentField() const ;
	void setVschvEnergyAscentField(uint32_t vschvEnergyAscentField) ;
	uint32_t getVschvEnergyDescentField() const ;
	void setVschvEnergyDescentField(uint32_t vschvEnergyDescentField) ;
	uint32_t getVschvEnergyTimePenField() const ;
	void setVschvEnergyTimePenField(uint32_t vschvEnergyTimePenField) ;
	uint32_t getVscRegenEnergyFactorField() const ;
	void setVscRegenEnergyFactorField(uint32_t vscRegenEnergyFactorField) ;
	uint32_t getVscVehAccelFactorField() const ;
	void setVscVehAccelFactorField(uint32_t vscVehAccelFactorField) ;
	uint32_t getVscRangeMapRefactrComfField() const ;
	void setVscRangeMapRefactrComfField(uint32_t vscRangeMapRefactrComfField) ;
	uint32_t getVscRangeMapRefactrEcoField() const ;
	void setVscRangeMapRefactrEcoField(uint32_t vscRangeMapRefactrEcoField) ;
	uint32_t getVscRangeMapRefactrGmhField() const ;
	void setVscRangeMapRefactrGmhField(uint32_t vscRangeMapRefactrGmhField) ;
	uint32_t getVschvBattConsumpSpd1Field() const ;
	void setVschvBattConsumpSpd1Field(uint32_t vschvBattConsumpSpd1Field) ;
	uint32_t getVschvBattConsumpSpd2Field() const ;
	void setVschvBattConsumpSpd2Field(uint32_t vschvBattConsumpSpd2Field) ;
	uint32_t getVschvBattConsumpSpd3Field() const ;
	void setVschvBattConsumpSpd3Field(uint32_t vschvBattConsumpSpd3Field) ;
	uint32_t getVschvBattConsumpSpd4Field() const ;
	void setVschvBattConsumpSpd4Field(uint32_t vschvBattConsumpSpd4Field) ;
	uint32_t getVschvBattConsumpSpd5Field() const ;
	void setVschvBattConsumpSpd5Field(uint32_t vschvBattConsumpSpd5Field) ;
	uint32_t getVschvBattConsumpSpd6Field() const ;
	void setVschvBattConsumpSpd6Field(uint32_t vschvBattConsumpSpd6Field) ;
	uint32_t getVschvBattConsumpSpd7Field() const ;
	void setVschvBattConsumpSpd7Field(uint32_t vschvBattConsumpSpd7Field) ;

    // Added by PCR036 - EVR Update
    uint32_t getEvRangeComfortField() const ;
    void setEvRangeComfortField(uint32_t _evRangeComfort) ;
    uint32_t getEvRangeECOField() const ;
    void setEvRangeECOField(uint32_t _evRangeECO) ;
    uint32_t getEvRangeGetMeHomeField() const ;
    void setEvRangeGetMeHomeField(uint32_t _evRangeGetMeHome) ;
    uint32_t getEvRangePredicStatusField() const ;
    void setEvRangePredicStatusField(uint32_t _evRangePredicStatus) ;

    int32_t convertWLCSFromCanToNgtp(uint32_t _valueNgtp);
    int32_t convertCSFromCanToNgtp(uint32_t _valueCan);

//////////////Should be private///////////////////////////
    uint32_t creationTimeStamp;

    bool bChargingStatus_is_present;
    uint8_t chargingStatus_field;

    bool bWlChargingStatus_is_present;
    uint8_t wlChargingStatus_field;

    bool bBatteryStatus_is_present;
    _ElectricVehicleBatteryInformation batteryStatus_field;

    // BatteryStatus member updated flag
    bool rangeOnBatteryKm_is_updated;
    bool rangeOnBatteryMiles_is_updated;
    bool rangeCombinedKm_is_updated;
    bool rangeCombinedMiles_is_updated;
    bool stateOfCharge_is_updated;

    bool bChargingInformation_is_present;
    _ElectricVehicleChargingInformation chargingInformation_field;

    // ChargingInformation member updated flag
    bool minutesToFullyCharged_is_updated;
    bool minutesToBulkCharged_is_updated;
    bool chargingRateSocPerHour_is_updated;
    bool chargingRateMilesPerHour_is_updated;
    bool chargingRateKmPerHour_is_updated;

    bool bEnergyConsumedLastChargekWh_is_preset;
    uint32_t energyConsumedLastChargekWh_field;

    bool bNextDepartureTimer_is_present;

    bool bChargeSetting_field_is_updated;
    svc::ChargeSettings ChargeSetting_field;

    //EVRangeMapInformation
    bool bEvRangeMapInformation_is_present;

    bool bVSCInitialHVBattEnergy_field_is_updated;
    bool bVSCRevisedHVBattEnergy_field_is_updated;
    bool bVSCRegenEnergyAvailable_field_is_updated;
    bool bVSCHVEnergyAscent_field_is_updated;
    bool bVSCHVEnergyDescent_field_is_updated;
    bool bVSCHVEnergyTimePen_field_is_updated;
    bool bVSCRegenEnergyFactor_field_is_updated;
    bool bVSCVehAccelFactor_field_is_updated;
    bool bVSCRangeMapRefactrComf_field_is_updated;
    bool bVSCRangeMapRefactrEco_field_is_updated;
    bool bVSCRangeMapRefactrGMH_field_is_updated;
    bool bVSCHVBattConsumpSpd1_field_is_updated;
    bool bVSCHVBattConsumpSpd2_field_is_updated;
    bool bVSCHVBattConsumpSpd3_field_is_updated;
    bool bVSCHVBattConsumpSpd4_field_is_updated;
    bool bVSCHVBattConsumpSpd5_field_is_updated;
    bool bVSCHVBattConsumpSpd6_field_is_updated;
    bool bVSCHVBattConsumpSpd7_field_is_updated;
    bool evRangeComfort_field_is_updated;
    bool evRangeECO_field_is_updated;
    bool evRangeGetMeHome_field_is_updated;
    bool evRangePredicStatus_field_is_updated;

    uint32_t latest_stateOfCharge_field;
    // SRD_ReqEVC0040_v1
    bool bEvcEviPushtimeElapsed;
};
#endif // _EV_ELECTRIC_VEHICLE_INFORMATION_H_