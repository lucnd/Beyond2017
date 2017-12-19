#ifndef _EV_VEHICLE_STATUS_
#define _EV_VEHICLE_STATUS_
#include <string>
#include "EvtSingleton.h"
#include "ApplicationTable.h"
#include "KeepPower.h"

class EvVehicleStatus : public EvtSingleton<EvVehicleStatus>
{
public:
    EvVehicleStatus();
    ~EvVehicleStatus();

    uint8_t getVehicleType();
    bool IsSVTActivated();
    bool IsCrashEventActivated();
    bool IsAlarmActivated();
    bool IsEmergencyActivated();
    bool IsBCallActivated();
    bool IsFuelLevelNotSufficientForRES();
    bool IsMaxExecutionTimeOfRESTimedOut();
    void evKeepCANbus(std::string _who);
    void evReleaseCANbus(std::string _who);
    void setVehicleSVTstatus(uint8_t state);
    bool isVehicleCharging();
    bool IsRemoteStartStatus();

    //PowerMode
    uint16_t getCurPowerMode();
    void setCurPowerMode(uint16_t _PM);

    // PowerState
    uint16_t getCurPowerState();
    void setCurPowerState(uint16_t _PS);

    // Vehicle Speed
    uint32_t getSpeedx100();

    // OdorMeter
    uint32_t getOdometerMaster();

    // Variant of TCU3
    int32_t getMdmVariant();

    enum{
        UNKNOWN = 0,
        PHEV = 1,
        BEV = 2
    };
private:
    KeepPower *evKeepPower;
    uint8_t cnt_KeepPower;
    uint16_t cur_PM;
    uint16_t cur_PowerState;
    bool bSVTstatus;
    int32_t mdmVariant;
};

#define VARI_MY18_TELE_ONLY      0
#define VARI_MY18_LOW             1
#define VARI_MY18_HIGH            2

#endif //_EV_VEHICLE_STATUS_