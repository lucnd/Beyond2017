/**
* \file    RemoteService_RVI.h
* \brief     Declaration of RemoteService_RVI Class.
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

#ifndef _REMOTE_SERVICE_RVI_H_
#define _REMOTE_SERVICE_RVI_H_

#include "RemoteServiceBase.h"
#include "RemoteServiceApp.h"

class RemoteService_RVI : public RemoteServiceBase
{

public:
    RemoteServiceApp &m_RemoteServiceApp;

    RemoteService_RVI(RemoteServiceApp& app);
    ~RemoteService_RVI();
    error_t RVI_reqAlarmReset();
    error_t RVI_reqLocationCheck();
    error_t RVI_reqVehicleSecure();
    error_t RVI_reqVehicleUnlock();
    error_t RVI_delayForReqBCMauth(RS_ServiceType serviceType);
    bool RemoteService_PrereqCheck(RS_ServiceType remoteServiceType);

    virtual void requestHandler(RS_ServiceType remoteServiceType);
    virtual void responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size);
    virtual bool responseStatus(RS_ServiceType remoteServiceType);

private:
};
#endif /**_REMOTE_SERVICE_RVI_H_*/
