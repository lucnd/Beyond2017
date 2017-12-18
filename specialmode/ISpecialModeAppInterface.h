/**
* \file    ISpecialModeAppInterface.h
* \brief     Declaration of ISpecialModeAppInterface
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.13
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#if !defined(EA_C94C7D4D_9802_4de9_981B_16DF0D94545E__INCLUDED_)
#define EA_C94C7D4D_9802_4de9_981B_16DF0D94545E__INCLUDED_

class ISpecialModeAppInterface
{

public:
    ISpecialModeAppInterface(){}
    virtual ~ISpecialModeAppInterface(){}
    virtual void do_SpecialModeHandler(uint32_t what, const sp<sl::Message>& message) = 0;
    virtual char* getPropertyWrap(const char* name) = 0;
    virtual void setPropertyChar(const char* name, const char* value, bool sync_now = false) = 0;
    virtual void setPropertyInt(const char* name, const int32_t i_value, bool sync_now = true) = 0;
};
#endif // !defined(EA_C94C7D4D_9802_4de9_981B_16DF0D94545E__INCLUDED_)
