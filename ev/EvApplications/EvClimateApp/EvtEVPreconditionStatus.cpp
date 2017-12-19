#include "EvtEVPreconditionStatus.h"
#include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"
#include "NGTPManagerService/NGTPUtil.h"
#define LOG_TAG "[EVT]PreconditionStatus"
#include "Log.h"


void EvtEVPreconditionStatus::resetAll()
{
    setCreationTimeStamp(0);
    setPreconditioningModeField(0);
    setPreconditionOperatingStatusField(0);
    setRemainingRuntimeMinutesField(0);
    setBatteryPreconditioningStatusField(0);
}

uint32_t EvtEVPreconditionStatus::getCreationTimeStamp()
{
    return CreationTimeStamp;
}

void EvtEVPreconditionStatus::setCreationTimeStamp(uint32_t time)
{
    CreationTimeStamp = time;
}

uint8_t EvtEVPreconditionStatus::getBatteryPreconditioningStatusField()
{
    return batteryPreconditioningStatus_field;
}

void EvtEVPreconditionStatus::setBatteryPreconditioningStatusField(
        uint8_t batteryPreconditioningStatusField) {
    batteryPreconditioningStatus_field = batteryPreconditioningStatusField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("EVPreconditionStatus::setBatteryPreconditioningStatus[%d], Time[%u]",batteryPreconditioningStatusField,getCreationTimeStamp());
}

uint8_t EvtEVPreconditionStatus::getPreconditioningModeField()
{
    return preconditioningMode_field;
}

void EvtEVPreconditionStatus::setPreconditioningModeField(uint8_t preconditioningModeField) {
    switch(preconditioningModeField)
    {
        case 0: //PreClim OFF
        {
            preconditioningMode_field = svc::EVPreconditionMode::inactive;
            break;
        }
        case 1: //PreClimTimed
        {
            preconditioningMode_field = svc::EVPreconditionMode::timed;
            break;
        }
        case 2: //PreClimDirectReq
        {
            preconditioningMode_field = svc::EVPreconditionMode::immediate;
            break;
        }
        case 3: //PreClimVent
        case 4: //Engine Heat
        default: //Undefined
        {
            preconditioningMode_field = svc::EVPreconditionMode::EVPreconditionMode_unknown;
            break;
        }

    }
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("EVPreconditionStatus::setPreconditioningMode[%d], Time[%u]",preconditioningModeField, getCreationTimeStamp());
}

uint8_t EvtEVPreconditionStatus::getPreconditionOperatingStatusField()
{
    return preconditionOperatingStatus_field;
}

void EvtEVPreconditionStatus::setPreconditionOperatingStatusField(
        uint8_t preconditionOperatingStatusField) {
    switch(preconditionOperatingStatusField)
    {
        case 0: // PreClim OFF
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::EVPreconditionOperatingStatus_off;
            break;
        case 1: // StartUp
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::startUp;
            break;
        case 2: // PreClim with Engine Running
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimWithEngineRunning;
            break;
        case 3: // PreClim Vent
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimVent;
            break;
        case 4: // PreClim Heat
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimHeat;
            break;
        case 5: // PreClim Cool
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimCool;
            break;
        case 6: // PreClim
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclim;
            break;
        case 7: // PreClim Vent & Batt Cool
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimVentAndBatteryCooling;
            break;
        case 8: // PreClim & Batt Cool
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimBatteryCooling;
            break;
        case 9: // Engine Heat
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::engineHeat;
            break;
        case 10: // PreClim Finished
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimFinished;
            break;
        case 11: // PreClim Inhibit HV power
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimInhibitedHvPower;
            break;
        case 12: // PreClim Inhibited system fault
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimInhibitedSystemFault;
            break;
        case 13: // PreClim Finished Partial Precon
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimFinishedPartialPrecon;
            break;
        case 14: // PreClim Requires Engine
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimRequiresEngine;
            break;
        case 15: //PreClim Grace Period
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::preclimGracePeriod;
            break;
        default:
            preconditionOperatingStatus_field = svc::EVPreconditionOperatingStatus::EVPreconditionOperatingStatus_unknown;
            break;
    }
    //preconditionOperatingStatus_field = preconditionOperatingStatusField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("EVPreconditionStatus::setPreconditionOperatingStatus[%d], Time[%u]",preconditionOperatingStatusField,getCreationTimeStamp());
}

uint32_t EvtEVPreconditionStatus::getRemainingRuntimeMinutesField()
{
    return remainingRuntimeMinutes_field;
}

void EvtEVPreconditionStatus::setRemainingRuntimeMinutesField(
        uint32_t remainingRuntimeMinutesField) {
    remainingRuntimeMinutes_field = remainingRuntimeMinutesField;
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("EVPreconditionStatus::setRemainingRuntimeMinutes[%d], Time[%u]",remainingRuntimeMinutesField,getCreationTimeStamp());
}
