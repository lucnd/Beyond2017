#ifndef SPECIALMODETYPE_H
#define SPECIALMODETYPE_H

#define SPECIALMODE_CONFIG_FILE "jlr_tcu_data_dictionary_swr_9_3.xml"
#define SPECIALMODE_INCONTROL "telematics2"
#define INCONTROL_TYPE_WEIGHT 7

enum SpecialModeType
{
    E_DEMOMODE = 0,           // run if specialmode has  provision
    E_IN_CONTROL_LIGHT = 1,   // run without provision
    E_UNKNOWN = 2             // undefine state
};

#endif // SPECIALMODETYPE_H
