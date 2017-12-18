/**
* \file    RemoteServiceBase.h
* \brief     Declaration of RemoteServiceBase
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       dongjin1.seo
* \date    2015.06.25
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#ifndef _REMOTE_SERVICE_BASE_H_
#define _REMOTE_SERVICE_BASE_H_

#include "RS_TYPE.h"
#include "include/Typedef.h"

class RemoteServiceBase
{
public:
    RemoteServiceBase() {}
    virtual ~RemoteServiceBase() {}

    virtual void requestHandler(RS_ServiceType remoteServiceType) = 0;
    virtual void responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size) = 0;
    virtual bool responseStatus(RS_ServiceType remoteServiceType) = 0;
};
#endif /**_REMOTE_SERVICE_BASE_H_*/
