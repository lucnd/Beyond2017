/**
* \file    RemoteService_RCC.h
* \brief     Declaration of RemoteService_RCC Class.
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

#ifndef _REMOTE_SERVICE_RCC_H_
#define _REMOTE_SERVICE_RCC_H_

#include "RemoteServiceBase.h"
#include "RemoteServiceApp.h"

class RemoteService_RCC : public RemoteServiceBase
{

public:
    RemoteServiceApp &m_RemoteServiceApp;

    RemoteService_RCC(RemoteServiceApp& app);
    ~RemoteService_RCC();
    bool RCC_getFrontSystemOnCmd();
    uint8_t RCC_getTempTargetTCU();
    void RCC_reqTempTargetTCU();
    void RCC_reqClimateDisable();
    void RCC_reqHeatedSeatFLMode();
    void RCC_reqHeatedSeatFRMode();
    void RCC_reqHeatedSeatRLMode();
    void RCC_reqHeatedSeatRRMode();
    void RCC_reqHeatedSeatFL();
    void RCC_reqHeatedSeatFR();
    void RCC_reqHeatedSeatRL();
    void RCC_reqHeatedSeatRR();
    void RCC_reqMaxDefrost();
    bool RemoteService_PrereqCheck(RS_ServiceType remoteServiceType);

    virtual void requestHandler(RS_ServiceType remoteServiceType);
    virtual void responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size);
    virtual bool responseStatus(RS_ServiceType remoteServiceType);
};
#endif /**_REMOTE_SERVICE_RCC_H_*/
