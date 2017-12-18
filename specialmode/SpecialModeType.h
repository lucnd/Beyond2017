/**
* \file    SpecialModeType.h
* \brief     Declaration of SpecialModeType
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.15
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#if !defined(EA_67A2FD1C_FAE4_4e52_8CB5_0AC8AB9ADA8B__INCLUDED_)
#define EA_67A2FD1C_FAE4_4e52_8CB5_0AC8AB9ADA8B__INCLUDED_

#define SPECIALMODE_CONFIG_FILE "jlr_tcu_data_dictionary_swr_9_3.xml"
#define SPECIALMODE_INCONTROL "telematics2"
#define INCONTROL_TYPE_WEIGHT 7

enum SpecialModeType
{
    DEMOMODE = 0,
    IN_CONTROL_LIGHT = 1,
    UNKNOWN = 2
};
#endif // !defined(EA_67A2FD1C_FAE4_4e52_8CB5_0AC8AB9ADA8B__INCLUDED_)
