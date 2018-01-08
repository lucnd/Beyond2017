#ifndef SPECIALMODETYPE_H
#define SPECIALMODETYPE_H

#define SPECIALMODE_CONFIG_FILE "jlr_tcu_data_dictionary_swr_9_3.xml"
#define SPECIALMODE_INCONTROL "telematics2"
#define INCONTROL_TYPE_WEIGHT 7

enum SpecialModeType
{
    DEMOMODE = 0,
    IN_CONTROL_LIGHT = 1,    // what ?
    UNKNOWN = 2
};

#endif // SPECIALMODETYPE_H
