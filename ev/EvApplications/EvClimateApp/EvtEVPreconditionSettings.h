//EvtEVPreconditionSettings
#ifndef _EV_PRECONDITION_SETTINGS_H_
#define _EV_PRECONDITION_SETTINGS_H_

#include "EvtSingleton.h"
class EvtEVPreconditionSettings : public EvtSingleton<EvtEVPreconditionSettings>
{
    public:
        EvtEVPreconditionSettings():cur_Signal_FFHPreClimSetting_value(-1),cur_Signal_PreClimRangeComfort_value(-1){};
        ~EvtEVPreconditionSettings(){};
        uint32_t            CreationTimeStamp;

    int8_t fuelFiredHeaterSetting_field;
    int8_t prioritySetting_field;

    int8_t cur_Signal_FFHPreClimSetting_value;
    int8_t cur_Signal_PreClimRangeComfort_value;

    void setCurPreconditionSettingSignal(int8_t _Signal_FFHPreClimSetting, int8_t _Signal_PreClimRangeComfort)
    {
        cur_Signal_FFHPreClimSetting_value = _Signal_FFHPreClimSetting;
        cur_Signal_PreClimRangeComfort_value = _Signal_PreClimRangeComfort;
    }
};

#endif // _EV_PRECONDITION_SETTINGS_H_