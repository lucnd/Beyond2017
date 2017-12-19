#include "EvtHvBatteryPack.h"
#include "EvServiceManager.h"
#include "NGTPManagerService/NGTPUtil.h"
#include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"

#define LOG_TAG "[EVT]HVBP"
#include "Log.h"

EvtHvBatteryPack::EvtHvBatteryPack()
{

};

void EvtHvBatteryPack::sampleDataFromCAN()
{
    LOGV("[sampleDataFromCAN] start");
    // TODO encode batteryPackDataHeader
    // TODO Should Get VIN Value
    _batteryPackDataHeader.set_vehicleType(convertNgtpFromCAN_U(0, 1, 0, 0, 31, 0));
    _batteryPackDataHeader.set_vehicleRegion(convertNgtpFromCAN_U(0, 1, 0, 0, 255, 0));
    //TODO Implement to Set BECMVersion
    _batteryPackDataHeader.omit_becmSoftwareVersion();

    svc::HighVoltageDataRecord _highVoltageDataRecord;

    // set HighVoltageDataRecord - BatteryCellMeasurements
    setBatteryCellMeasurements(_highVoltageDataRecord);

    // set HighVoltageDataRecord - BatteryFaultMonitoring
    setbatteryFaultMonitoring(_highVoltageDataRecord);

    // set HighVoltageDataRecord - BatteryHealthMonitoring
    setBatteryHealthMonitoring(_highVoltageDataRecord);

    // set HighVoltageDataRecord - ThermalManagement
    setThermalManagement(_highVoltageDataRecord);

    // set HighVoltageDataRecord - CellBalancing
    setCellBalancing(_highVoltageDataRecord);

    // set HighVoltageDataRecord - BMSElectricalModuleMonitoring
    setBmsElectricalModuleMonitoring(_highVoltageDataRecord);

    // set HighVoltageDataRecord - HVBattOther
    setParkingData(_highVoltageDataRecord);

    // set HighVoltageDataRecord - HVBattParkingData
    setOther(_highVoltageDataRecord);

    _hvDataRecordVec.push_back(_highVoltageDataRecord);
    LOGV("[sampleDataFromCAN] HVDataRecord Added [%d]", _hvDataRecordVec.size());
}

void EvtHvBatteryPack::setBatteryCellMeasurements(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::batteryCellMeasurements _batteryCellMeasurements;

    _batteryCellMeasurements.set_hvBattVoltageExt(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattVoltageExt_RX), 1, 0, 0, 1023, 0));
    _batteryCellMeasurements.set_hvBattCurrentExtX1000(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattCurrentExt_RX), 25, -209715175, -209715175, 209715200, 0));
    _batteryCellMeasurements.set_hvBattCellVoltageMaxX1000(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattCellVoltageMax_RX), 1, 0, 0, 8191, 0));
    _batteryCellMeasurements.set_hvBattVoltMaxCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattVoltMaxCellID_RX), 1, 0, 1, 255, 1));
    _batteryCellMeasurements.set_hvBattCellVoltageMinX1000(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattCellVoltageMin_RX), 1, 0, 0, 8191, 0));
    _batteryCellMeasurements.set_hvBattVoltMinCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattVoltMinCellID_RX), 1, 0, 1, 255, 1));
    _batteryCellMeasurements.set_hvBattCellTempHottestX10(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattCellTempHottest_RX), 5, -400, -400, 875, -400));
    _batteryCellMeasurements.set_hvBattTempHotCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattTempHottCellID_RX), 1, 0, 1, 255, 1));
    _batteryCellMeasurements.set_hvBattCellTempCoolestX10(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattCellTempColdest_RX), 5, -400, -400, 875, -400));
    _batteryCellMeasurements.set_hvBattCellTempColdCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattTempColdCellID_RX), 1, 0, 1, 255, 1));

    _hvDataRecord.set_batteryCellMeasurements(_batteryCellMeasurements);
}

void EvtHvBatteryPack::setbatteryFaultMonitoring(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::batteryFaultMonitoring _batteryFaultMonitoring;

    //TODO rangeCheck function for char
    _batteryFaultMonitoring.set_hvBatStatuCat4Derate(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStatusCAT4Derate_RX));
    _batteryFaultMonitoring.set_hvBatStatusCat6DlyBpo(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStatusCAT6DlyBPO_RX));
    _batteryFaultMonitoring.set_hvBatStatusCat7NowBpo(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStatusCAT7NowBPO_RX));

    _hvDataRecord.set_batteryFaultMonitoring(_batteryFaultMonitoring);
}

void EvtHvBatteryPack::setBatteryHealthMonitoring(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::batteryHealthMonitoring _batteryHealthMonitoring;

    _batteryHealthMonitoring.set_hvBatStateOfHealthX10(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStateofHealth_RX), 1, 0, 0, 1000, 0));
    _batteryHealthMonitoring.set_hvBatStateOfHealthMinX10(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStateofHealthMin_RX), 1, 0, 0, 1000, 0));
    _batteryHealthMonitoring.set_hvBatStateOfHealthMaxX10(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStateofHealthMax_RX), 1, 0, 0, 1000, 0));
    _batteryHealthMonitoring.set_hvBattStateOfHealthPwrX10(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattStateofHealthPwr_RX), 1, 0, 0, 1000, 0));
    _batteryHealthMonitoring.set_hvBatSocHcMinCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattSoHcMinCellID_RX), 1, 1, 0, 108, 0));
    _batteryHealthMonitoring.set_hvBatSocHcMaxCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattSoHcMaxCellID_RX), 1, 1, 0, 108, 0));
    _batteryHealthMonitoring.set_hvBattFastChgCounter(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattFastChgCounter_RX), 1, 0, 0, 4095, 0));

    _hvDataRecord.set_batteryHealthMonitoring(_batteryHealthMonitoring);
}

void EvtHvBatteryPack::setThermalManagement(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::thermalManagement _thermalManagement;

    _thermalManagement.set_hvBattInletCoolantTempX10(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattInletCoolantTemp_RX), 5, -400, -400, 875, 870));
    _thermalManagement.set_hvBattOutletCoolantTempX10(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattOutletCoolantTmp_RX), 5, -400, -400, 875, 870));
    _thermalManagement.set_hvBattCoolingEnergyUsdX100(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattCoolingEnrgyUsd_RX), 5, 0, 0, 10235, 0));
    _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::HVBattThrmlMngrMode_initialValue);

    int32_t _hvThrmlMgrMode = EvtVifManager::GetInstance()->queryCanData(Signal_HVBattThrmlMngrMode_RX);

    switch(_hvThrmlMgrMode)
    {
        case 0:
            break;
        case 1:
             _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::idle);
            break;
        case 2:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::HVBattThrmlMngrMode_cooling);
            break;
        case 3:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::HVBattThrmlMngrMode_heating);
            break;
        case 4:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::activeCooling);
            break;
        case 5:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::activeHeating);
            break;
        case 6:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::passingCooling);
            break;
        case 7:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::passiveHeating);
            break;
        case 8:
            _thermalManagement.set_hvBattThrmlMngrMode(svc::HVBattThrmlMngrMode::thermalBalancing);
            break;
        default:
            break;
    }
    _hvDataRecord.set_thermalManagement(_thermalManagement);
}

void EvtHvBatteryPack::setCellBalancing(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::cellBalancing _cellBalancing;

    _cellBalancing.set_hvBattBalancingStatus(svc::HVBattBalancingStatus::HVBattBalancingStatus_initialValue);
    int32_t _cellBalStatus = EvtVifManager::GetInstance()->queryCanData(Signal_HVBattBalancingStatus_RX);
    switch(_cellBalStatus)
    {
        case 0:
            break;
        case 1:
            _cellBalancing.set_hvBattBalancingStatus(svc::HVBattBalancingStatus::noBalancing);
            break;
        case 2:
            _cellBalancing.set_hvBattBalancingStatus(svc::HVBattBalancingStatus::activeBalancing);
            break;
        case 3:
            _cellBalancing.set_hvBattBalancingStatus(svc::HVBattBalancingStatus::passiveBalancing);
            break;
        default:
            break;
    }

    _cellBalancing.set_hvBattBlncngTrgCellId(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattBlncngTrgrCellID_RX), 1, 0, 0, 108, 0));

    _hvDataRecord.set_cellBalancing(_cellBalancing);
}

void EvtHvBatteryPack::setBmsElectricalModuleMonitoring(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::bmsElectricalModuleMonitoring _bmsElectricalModuleMonitoring;

    _bmsElectricalModuleMonitoring.set_hvBattFuseTemperatureX10(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattFuseTemperature_RX), 5, -400, -400, 875, 870));
    _bmsElectricalModuleMonitoring.set_hvBattMemTemperatureX10(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattBEMTemperature_RX), 5, -400, -400, 875, 870));

    _hvDataRecord.set_bmsElectricalModuleMonitoring(_bmsElectricalModuleMonitoring);
}

void EvtHvBatteryPack::setParkingData(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::parkingData _parkingData;

    _parkingData.set_hVBattAvgSOCOAtEvent(convertNgtpFromCAN_U(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattAvgSOCOAtEvent_RX), 1, 0, 0, 102, 101));
    _parkingData.set_hVBattAvTempX10AtEvent(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_HVBattAvTempAtEvent_RX), 5, -400, -400, 875, 875));

    _hvDataRecord.set_parkingData(_parkingData);
}

void EvtHvBatteryPack::setOther(svc::HighVoltageDataRecord& _hvDataRecord)
{
    svc::HighVoltageDataRecord::other _other;
    svc::TimeStamp _ts;

    _other.set_powerMode(svc::HVBattPowerMode::keyOut);
    uint16_t _curPM = EvVehicleStatus::GetInstance()->getCurPowerMode();
    switch(_curPM)
    {
        case 0:
            break;
        case 1:
            _other.set_powerMode(svc::HVBattPowerMode::keyRecentlyOut);
            break;
        case 2:
            _other.set_powerMode(svc::HVBattPowerMode::keyApproved0);
            break;
        case 3:
            _other.set_powerMode(svc::HVBattPowerMode::postAccessory0);
            break;
        case 4:
            _other.set_powerMode(svc::HVBattPowerMode::accessory1);
            break;
        case 5:
            _other.set_powerMode(svc::HVBattPowerMode::postIgnition1);
            break;
        case 6:
            _other.set_powerMode(svc::HVBattPowerMode::ignitionOn2);
            break;
        case 7:
            _other.set_powerMode(svc::HVBattPowerMode::running2);
            break;
        case 9:
            _other.set_powerMode(svc::HVBattPowerMode::crank3);
            break;
        default:
            break;
    }

    _other.set_vehicleSpeedX100(EvVehicleStatus::GetInstance()->getSpeedx100());
    _other.set_odometerMasterValue(EvVehicleStatus::GetInstance()->getOdometerMaster());

    //TODO set CurTimeStamp
    _ts.set_seconds(convertNgtpFromCAN_U(0, 0, 0, 0, 0, 0));
    _other.set_recordedTime(_ts);
    _other.set_ambientTempX100(convertNgtpFromCAN(EvtVifManager::GetInstance()->queryCanData(Signal_AmbientTemp_RX), 25, -12800, -12800, 12775, -12800));

    _hvDataRecord.set_other(_other);
}

uint32_t EvtHvBatteryPack::convertNgtpFromCAN_U(   int32_t canValue
                                                    , int16_t scaleFactor
                                                    , int16_t offset
                                                    , int32_t minValue
                                                    , int32_t maxValue
                                                    , int32_t defaultValue)
{
    uint32_t val;
    val = (canValue*scaleFactor)+offset;
    val = ((minValue > val) || (val > maxValue)) ? defaultValue : val;
    return val;
}

int32_t EvtHvBatteryPack::convertNgtpFromCAN(   int32_t canValue
                                                    , int16_t scaleFactor
                                                    , int16_t offset
                                                    , int32_t minValue
                                                    , int32_t maxValue
                                                    , int32_t defaultValue)
{
    int32_t val;
    val = (canValue*scaleFactor)+offset;
    val = ((minValue > val) || (val > maxValue)) ? defaultValue : val;
    return val;
}

void EvtHvBatteryPack::initHvDataRecordVec()
{

    //Vector initialization.
    if(!_hvDataRecordVec.empty())
    {
        /**
        for(auto element : _hvDataRecordVec)
        {
            delete element;
        }
      */
        LOGV("Clear hvDataVec");
        _hvDataRecordVec.clear();
    }
}

svc::BatterPackDataHeader& EvtHvBatteryPack::getBatteryPackDataHeader()
{
    return _batteryPackDataHeader;
}

vector<svc::HighVoltageDataRecord>& EvtHvBatteryPack::getHvDataRecordVec()
{
    return _hvDataRecordVec;
}