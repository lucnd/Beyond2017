#include "EvVehicleStatus.h"
#include "EvConfigData.h"
#include "EvtUtil.h"
#include "EvtVifManager.h"
#include "EvHBDApp.h"
#include "services/PowerManagerService/JLR_State/JLRPowerMode.h"
#include "SystemManagerService/SystemManagerService.h"
#include "EvChargeApp.h"

#define LOG_TAG "[EVT]VehicleStatus"
#include "Log.h"

#define RES_SVT_ALERT_STATUS   2
#define RES_SVT_ALARM_STATUS   3

EvVehicleStatus::EvVehicleStatus()
{
    cur_PM = PM_8_NOT_USED;
    cur_PowerState = STATE_UNKNOWN;
    evKeepPower = new KeepPower(APP_ID_EV);
    cnt_KeepPower = 0;
    bSVTstatus = false;

    mdmVariant = SET_SERVICE(SystemManagerService)->getMDMvariant();
}
EvVehicleStatus::~EvVehicleStatus()
{
    if(evKeepPower != NULL)
    {
        delete evKeepPower;
    }
}

int32_t EvVehicleStatus::getMdmVariant()
{
    return mdmVariant;
}

void EvVehicleStatus::setVehicleSVTstatus(uint8_t state)
{
    if((state == RES_SVT_ALERT_STATUS) || (state == RES_SVT_ALARM_STATUS))
    {
        bSVTstatus = true;
    }
    else
    {
        bSVTstatus = false;
    }
    LOGI("SVT status update (%d)", bSVTstatus);

    //ECW check.
    EvChargeApp::GetInstance()->triggerECWstate();
}

bool EvVehicleStatus::IsSVTActivated()      // Negative Function.
{
    return bSVTstatus;
}

bool EvVehicleStatus::IsCrashEventActivated()      // Negative Function.
{
    int32_t _CrashStatusRCM = EvtVifManager::GetInstance()->queryCanData(Signal_CrashStatusRCM_RX);
    if(_CrashStatusRCM < 1 /*Both driver airbag and passenger airbag not deployed*/)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool EvVehicleStatus::IsAlarmActivated()      // Negative Function.
{
    int32_t _AlarmModeHS = EvtVifManager::GetInstance()->queryCanData(Signal_AlarmModeHS_RX);
    if(_AlarmModeHS == 2 /*Active*/)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EvVehicleStatus::IsEmergencyActivated()      // Negative Function.
{
    int32_t eCallStatus = EvConfigData::GetInstance()->getConfigData("eCallstatus");
    if(eCallStatus == 1)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool EvVehicleStatus::IsBCallActivated()      // Negative Function.
{
    int32_t bCallStatus = EvConfigData::GetInstance()->getConfigData("bCallstatus");
    if(bCallStatus == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EvVehicleStatus::IsFuelLevelNotSufficientForRES()      // Negative Function.
{
    int32_t _FuelLevelIndicatedHS = EvtVifManager::GetInstance()->queryCanData(Signal_FuelLevelIndicatedHS_RX);
    if(_FuelLevelIndicatedHS < 0)
    {
        return true;
    }
    double resFuelLevelMin = 0.1 * EvConfigData::GetInstance()->getConfigData("ClimateControlResFuelLevelStartMin_dl");
    double _tmp_FuelLevelIndicatedHS = (double)_FuelLevelIndicatedHS * 0.2;
    LOGI("Current FuelLevel: %lf, Threshold: %lf", _tmp_FuelLevelIndicatedHS, resFuelLevelMin);
    if(_tmp_FuelLevelIndicatedHS < resFuelLevelMin)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EvVehicleStatus::IsMaxExecutionTimeOfRESTimedOut()      // Negative Function.
{
    uint16_t RVC_RES_RUNMAX = EvConfigData::GetInstance()->getConfigData("RvcResRunTime_s"); //remaining res time, internal value to save actual remaining time.
    uint16_t RES_MIN_REMAINING_TIME = EvConfigData::GetInstance()->getConfigData("ClimateControlResRunTimeMinLimit_s");

    if(RVC_RES_RUNMAX <= RES_MIN_REMAINING_TIME)
    {
        LOGE("[Warnning] Remaining RES time [%d] <= RES_MIN_REMAINING_TIME[%d].", RVC_RES_RUNMAX, RES_MIN_REMAINING_TIME);
        return true;
    }
    else
    {
        LOG_EV("[%s] Remaining RES time [%d] > RES_MIN_REMAINING_TIME[%d]", __func__, RVC_RES_RUNMAX, RES_MIN_REMAINING_TIME);
        return false;
    }
}

uint8_t EvVehicleStatus::getVehicleType()
{
    uint8_t _vehicleType = EvConfigData::GetInstance()->getEvCfgData("VEHICLE_TYPE");
    switch(_vehicleType)
    {
        case 0x06:      // PHEV
        {
            return EvVehicleStatus::PHEV;
        }
        case 0x07:      //BEV
        {
            return EvVehicleStatus::BEV;
        }
        default:
        {
            return EvVehicleStatus::UNKNOWN;
        }
    }
}

void EvVehicleStatus::evKeepCANbus(std::string _who)
{
    evKeepPower->acquire();
    cnt_KeepPower++;
    LOG_EV("[DEBUG] <<%s>> triggered CAN bus hold request. Reference Count:[%d]",_who.c_str(),cnt_KeepPower);
}

void EvVehicleStatus::evReleaseCANbus(std::string _who)
{
    if(cnt_KeepPower == 0)
    {
        return ;
    }

    evKeepPower->release();
    cnt_KeepPower--;
    LOG_EV("[DEBUG] <<%s>> triggered CAN bus release request. Reference Count:[%d]",_who.c_str(),cnt_KeepPower);
}


uint16_t EvVehicleStatus::getCurPowerMode()
{
    return cur_PM;
}

void EvVehicleStatus::setCurPowerMode(uint16_t _PM)
{
    LOGI("PM [%d] ==> [%d]", cur_PM, _PM);
    cur_PM = _PM;
}

uint16_t EvVehicleStatus::getCurPowerState()
{
    return cur_PowerState;
}

void EvVehicleStatus::setCurPowerState(uint16_t _PS)
{
    LOGI("PowerState [%d] ==> [%d]", cur_PowerState, _PS);
    cur_PowerState = _PS;

    //Add Trigger event.
    if((EvHBDApp::GetInstance()->getState() < EvHBDApp::HBDStatus::HBD_END_ENUM)
        && (EvHBDApp::GetInstance()->getState() != EvHBDApp::HBDStatus::HBD_OFF))
    {
        EvHBDApp::GetInstance()->determineHBDOperation(cur_PowerState);
    }
    else
    {
        LOGE("OPSC-Invalid HBD State[%d]", EvHBDApp::GetInstance()->getState());
    }
}

uint32_t EvVehicleStatus::getSpeedx100()
{
    uint32_t _speed = EvtVifManager::GetInstance()->queryCanData(Signal_VehicleSpeed_RX);
    _speed = (_speed > 32000) ?  0 : _speed;
    return _speed;
}

uint32_t EvVehicleStatus::getOdometerMaster()
{
    uint32_t _odometer = EvtVifManager::GetInstance()->queryCanData(Signal_OdometerMasterValue_RX);
    _odometer = (_odometer > 0xFFFFFE) ? 0 : _odometer;
    return _odometer;
}

// SRD_ReqEVC0047_v3 - Check if Vehicle is Charging
bool EvVehicleStatus::isVehicleCharging()
{
    bool _result;
    int32_t _wlcstatus = EvtVifManager::GetInstance()->queryCanData(Signal_WlChargingStatusDisp_RX);
    int32_t _cstatus = EvtVifManager::GetInstance()->queryCanData(Signal_ChargingStatusDisp_RX);

    _result = ((_wlcstatus == 3) || (_cstatus == 3)) ? true : false;
    LOGV("IVC-%s C[%d]WL[%d]", _result?"T":"F",_cstatus, _wlcstatus  );
    return _result;
}

bool EvVehicleStatus::IsRemoteStartStatus()
{
    int32_t _remoteStartStatus = EvtVifManager::GetInstance()->queryCanData(Signal_RemoteStartStatusMS_RX);
    if(_remoteStartStatus == 1) // Remote Mode (Driver Not present)
    {
        return true;
    }
    else
    {
        return false;
    }
}