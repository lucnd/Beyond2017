//EVPreconditionStartConfiguration

#ifndef _EV_PRECONDITION_START_CONFIGURATION_H_
#define _EV_PRECONDITION_START_CONFIGURATION_H_

#include "EvtSingleton.h"

class EvtEVPreconditionStartConfiguration : public EvtSingleton<EvtEVPreconditionStartConfiguration>
{
    public:
        EvtEVPreconditionStartConfiguration(){};
        ~EvtEVPreconditionStartConfiguration(){};
        uint32_t            CreationTimeStamp;

    uint32_t targetTemperatureCelsiusX10_field;
};

#endif // _EV_PRECONDITION_START_CONFIGURATION_H_