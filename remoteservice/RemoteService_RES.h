/**
* \file    RemoteService_RES.h
* \brief     Declaration of RemoteService_RES Class.
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

#ifndef _REMOTE_SERVICE_RES_H_
#define _REMOTE_SERVICE_RES_H_

#include "RemoteServiceBase.h"
#include "RemoteServiceApp.h"

class RemoteService_RES : public RemoteServiceBase
{

public:
    RemoteServiceApp &m_RemoteServiceApp;

    RemoteService_RES(RemoteServiceApp& app);
    ~RemoteService_RES();
    bool RES_getSVTstatus(RS_ServiceType serviceType);
    bool RES_getPowerMode(RS_ServiceType serviceType);
    bool RES_getRESstatus(RS_ServiceType serviceType);
    bool RES_getCrashStatus(RS_ServiceType serviceType);
    bool RES_getFuelLevel(RS_ServiceType serviceType);
    bool RES_getRemainingRuntime(RS_ServiceType serviceType);
    bool RES_getFFHOperatingStatus(RS_ServiceType serviceType);
    bool RemoteService_PrereqCheck(RS_ServiceType remoteServiceType);

    error_t RES_reqEngineStart();
    error_t RES_reqEngineStop();
    error_t RES_reqFFHactivation();
    error_t RES_reqEngineHeating();
    error_t RES_reqFFHstop();
    error_t RES_delayForReqBCMauth(RS_ServiceType serviceType);

    virtual void requestHandler(RS_ServiceType remoteServiceType);
    virtual void responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size);
    virtual bool responseStatus(RS_ServiceType remoteServiceType);

private:
};
#endif /**_REMOTE_SERVICE_RES_H_*/
