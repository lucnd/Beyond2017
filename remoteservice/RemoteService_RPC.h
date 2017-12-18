/**
* \file    RemoteService_RPC.h
* \brief     Declaration of RemoteService_RPC Class.
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

#ifndef _REMOTE_SERVICE_RPC_H_
#define _REMOTE_SERVICE_RPC_H_

#include "RemoteServiceBase.h"
#include "RemoteServiceApp.h"

class RemoteService_RPC : public RemoteServiceBase
{

public:
    RemoteServiceApp &m_RemoteServiceApp;

    RemoteService_RPC(RemoteServiceApp& app);
    ~RemoteService_RPC();

    bool requestHandler_TCU(RS_ServiceType remoteServiceType);
    void requestHandler_HVAC(RS_ServiceType remoteServiceType);

    bool RPC_getParkClimateSystemPresent(RS_ServiceType serviceType);
    bool RPC_getParkClimateRemoteEnabled(RS_ServiceType serviceType);
    bool RPC_getPowerMode(RS_ServiceType serviceType);
    bool RPC_getFFHOperatingStatus(RS_ServiceType serviceType);
    bool RPC_getparkClimateModeTCU(RS_ServiceType serviceType);
    bool RemoteService_PrereqCheck(RS_ServiceType remoteServiceType);
    bool RemoteService_PrereqCheck_TCU(RS_ServiceType remoteServiceType);
    bool RemoteService_PrereqCheck_HVAC(RS_ServiceType remoteServiceType);
    void reqParkVenting();
    void reqParkHeating();
    void reqParkAuto();
    void reqParkStop();

    virtual void requestHandler(RS_ServiceType remoteServiceType);
    virtual void responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size);
    virtual bool responseStatus(RS_ServiceType remoteServiceType);
};
#endif /**_REMOTE_SERVICE_RPC_H_*/
