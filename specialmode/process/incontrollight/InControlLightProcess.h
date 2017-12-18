/**
* \file    InControlLightProcess.h
* \brief     Declaration of InControlLightProcess
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

#if !defined(EA_B7C52035_7E7E_4cc9_9396_94A02446410F__INCLUDED_)
#define EA_B7C52035_7E7E_4cc9_9396_94A02446410F__INCLUDED_

#include "SpecialModeBaseProcess.h"

class InControlLightProcess : public SpecialModeBaseProcess
{

public:
	InControlLightProcess();
	virtual ~InControlLightProcess();
    virtual void do_SpecialModeHandler(int32_t what, const sp<sl::Message>& message);
	virtual void handleTimerEvent(int timerId);
	virtual void handleEvent(uint32_t ev);

protected:
	virtual void initializeProcess();

};
#endif // !defined(EA_B7C52035_7E7E_4cc9_9396_94A02446410F__INCLUDED_)
