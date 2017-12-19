//EvtEVPreconditionStatus
#ifndef _EV_PRECONDITION_STATUS_H_
#define _EV_PRECONDITION_STATUS_H_

#include "EvtSingleton.h"
#include "EvtEVPreconditionSettings.h"

class EvtEVPreconditionStatus : public EvtSingleton<EvtEVPreconditionStatus>
{
    public:
        EvtEVPreconditionStatus(){};
        ~EvtEVPreconditionStatus(){};

        void        resetAll();
        uint32_t    getCreationTimeStamp();
        void        setCreationTimeStamp(uint32_t time);
        uint8_t     getPreconditioningModeField();
        void        setPreconditioningModeField(uint8_t preconditioningModeField);
        uint8_t     getPreconditionOperatingStatusField();
        void        setPreconditionOperatingStatusField(uint8_t preconditionOperatingStatusField);
        uint32_t    getRemainingRuntimeMinutesField();
        void        setRemainingRuntimeMinutesField(uint32_t remainingRuntimeMinutesField);
        uint8_t     getBatteryPreconditioningStatusField();
        void        setBatteryPreconditioningStatusField(uint8_t batteryPreconditioningStatusField);

    private:
        uint32_t    CreationTimeStamp;
        uint8_t     preconditioningMode_field;
        uint8_t     preconditionOperatingStatus_field;
        uint32_t    remainingRuntimeMinutes_field;
        uint8_t     batteryPreconditioningStatus_field;
        //EvtEVPreconditionSettings preconditionSettings_field;
};

#endif // _EV_PRECONDITION_STATUS_H_