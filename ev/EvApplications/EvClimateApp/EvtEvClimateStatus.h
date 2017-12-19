//EvClimateAppStatus
#ifndef _EV_CLIMATE_STATUS_H_
#define _EV_CLIMATE_STATUS_H_

#include "EvtSingleton.h"
class EvtEvClimateAppStatus : public EvtSingleton<EvtEvClimateAppStatus>
{
    typedef struct
    {
        uint32_t hour_field;
        uint32_t minute_field;
    } _TimeHM;

    typedef struct
    {
        uint32_t month_field;
        uint32_t day_field;
        _TimeHM time_field;
    } _ClimateTimer;

    typedef struct
    {
        uint8_t selection_mode_field;
        uint8_t operation_mode_field;
    } _HeatedSeatSettings;

    public:
        EvClimateAppStatus();
        ~EvClimateAppStatus();
        uint32_t            CreationTimeStamp;

    uint8_t/*BPPLEXT*/ timerActivationStatus_field;
    _ClimateTimer timer1_field;
    _ClimateTimer timer2_field;
    uint8_t climateOperatingStatus_field;
    uint32_t remainingRunTime_field;
    uint32_t ffhRemainingRunTime_field;
    uint32_t ventingTime_field;
    uint8_t heatedFrontScreenActive_field;
    uint8_t heatedRearWindowActive_field;
    uint8_t heatedSteeringWheelActive_field;
    _HeatedSeatSettings heatedSeatFrontLeft_field;
    _HeatedSeatSettings heatedSeatFrontRight_field;
    _HeatedSeatSettings heatedSeatRearLeft_field;
    _HeatedSeatSettings heatedSeatRearRight_field;
};

#endif // _EV_CLIMATE_STATUS_H_

