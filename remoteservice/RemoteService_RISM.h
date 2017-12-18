/**
* \file    RemoteService_RISM.h
* \brief     Declaration of RemoteService_RISM Class.
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

#ifndef _REMOTE_SERVICE_RISM_H_
#define _REMOTE_SERVICE_RISM_H_

#include "RemoteServiceBase.h"
#include "RemoteServiceApp.h"

class RemoteService_RISM : public RemoteServiceBase
{

public:
    RemoteServiceApp &m_RemoteServiceApp;

    RemoteService_RISM(RemoteServiceApp& app);
    ~RemoteService_RISM();

    error_t RISM_reqInitSeatMove();
    error_t RISM_reqCompleteSeatMove();

    error_t RISM_reqSeatMove();
    error_t RISM_reqSeatMove_L462();
    error_t RISM_reqSeatMove_L405();
    error_t RISM_delayForReqBCMauth(RS_ServiceType serviceType);
    bool RemoteService_PrereqCheck(RS_ServiceType remoteServiceType);

    virtual void requestHandler(RS_ServiceType remoteServiceType);
    virtual void responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size);
    virtual bool responseStatus(RS_ServiceType remoteServiceType);
};
#endif /**_REMOTE_SERVICE_RISM_H_*/
