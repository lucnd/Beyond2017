/**
* \file    RemoteServiceApp.h
* \brief     Declaration of RemoteServiceApp
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

#ifndef _REMOTE_SERVICE_APPLICATION_BASE_H_
#define _REMOTE_SERVICE_APPLICATION_BASE_H_

#include "RemoteServiceBase.h"
#include "RemoteService_log.h"

#include <application/Application.h>
#include "RS_TYPE.h"
#include "RS_EVENT_LIST.h"

#include "Handler.h"
#include "SLLooper.h"
#include "Timer.h"
#include "Buffer.h"

#include "HMIType.h"

#include "include/Typedef.h"
#include "include/ApplicationTable.h"

#include "services/vifManagerService/vifcansig.h"
#include "services/vifManagerService/LGE_CANSignalFrameID.h"

#include "services/DebugManagerService/DebugManagerService.h"
#include "services/DebugManagerService/IDebugManagerService.h"
#include "services/DebugManagerService/IMgrReceiver.h"

#include "services/vifManagerService/vifManagerService.h"
#include "services/vifManagerService/IvifManagerService.h"
#include "services/vifManagerService/IvifManagerReceiver.h"
#include "services/vifManagerService/vifcansig.h"

#include "services/NGTPManagerService/NGTPManagerService.h"
#include "services/NGTPManagerService/INGTPManagerService.h"
#include "services/NGTPManagerService/INGTPReceiver.h"
#include "services/NGTPManagerService/InternalNGTPData.h"

#include "services/PowerManagerService/PowerManagerService.h"
#include "services/PowerManagerService/IPowerStateReceiver.h"
#include "services/PowerManagerService/ICANBusStateReceiver.h"

#include "services/ApplicationManagerService/ApplicationManagerService.h"
#include "services/ApplicationManagerService/IApplicationManagerService.h"

#include "services/ConfigurationManagerService/ConfigurationManagerService.h"
#include "services/ConfigurationManagerService/IConfigurationManagerService.h"
#include "services/ConfigurationManagerService/IConfigurationManagerReceiver.h"

#include "services/DiagManagerService/DiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerReceiver.h"

#include "services/AlarmManagerService/IAlarmManagerService.h"
#include "services/AlarmManagerService/AlarmManagerService.h"
#include "services/AlarmManagerService/IAlarmExpireListener.h"


class RemoteServiceAppBase
{
public:
    RemoteServiceAppBase() {}
    virtual ~RemoteServiceAppBase() {}

    /* RCC Virtual Function */
    virtual error_t RemoteService_reqSendCANdata(uint8_t nSig, sp<vifCANContainer> &outdata) = 0;
    virtual bool getFrontSystemOnCmd() = 0;
    virtual bool getCurrRemoteStartStatus() = 0;
    virtual RS_PowerMode RemoteService_getPowerMode() = 0;
    virtual uint8_t getUint8fromConfig(const char* name) = 0;
    virtual void setHeatedSeatFLModeReq(uint8_t status) = 0;
    virtual void setHeatedSeatFLRequest(uint8_t status) = 0;
    virtual void setHeatedSeatFRModeReq(uint8_t status) = 0;
    virtual void setHeatedSeatFRRequest(uint8_t status) = 0;
    virtual void setHeatedSeatRLModeReq(uint8_t status) = 0;
    virtual void setHeatedSeatRLRequest(uint8_t status) = 0;
    virtual void setHeatedSeatRRModeReq(uint8_t status) = 0;
    virtual void setHeatedSeatRRRequest(uint8_t status) = 0;
    virtual void setHFSCommand(bool status) = 0;
    virtual void setHRWCommand(bool status) = 0;

    /* RES Virtual Function */
    virtual void reqBCMauthentication(RS_ServiceType service) = 0;
    virtual void createTimer() = 0;
    virtual void deleteTimer() = 0;
    virtual void startTimer(RS_ServiceType serviceType) = 0;
    virtual void stopTimer(RS_ServiceType serviceType) = 0;
    virtual void RemoteService_receiveMessageFromTSP(RS_ServiceType remoteServiceType) = 0;
    virtual void setCurrParkClimateReqTCU(uint8_t reqMode) = 0;
    virtual bool getSVTactive() = 0 ;
    virtual void setRVCproperty(RS_ServiceType serviceType, char* key, RVCproperty prop, const char* value) = 0;
    virtual sp<IConfigurationManagerService>& getConfigManager() = 0;
    virtual uint8_t getRESstopReason() = 0;
    virtual void setCurrState(RS_ServiceType service, RS_ServiceState state) = 0;
    virtual void sendNackMsgToTSP(RS_ServiceType remoteServiceType) = 0;
    virtual bool RemoteService_getRemoteStartStatus() = 0;
    virtual bool getParkClimateSystemPresent() = 0;
    virtual bool getParkClimateRemoteEnabled() = 0;
    virtual uint8_t getFFHOperatingStatus() = 0;
    virtual int8_t getCurrParkClimateModeTCU() = 0;

    virtual bool getDBGcmdActiveStatus() = 0;
    virtual void RemoteService_sendDBGcmdResponse(uint8_t cmd, uint8_t cmd2, sp<Buffer> &buf) = 0;
    virtual void setDBGcmdActiveStatus(bool status) = 0;
    virtual RS_ServiceState getCurrState(RS_ServiceType service) = 0;
    virtual void sendAckMsgToTSP(RS_ServiceType remoteServiceType, svc::ServiceMessageType svcMessageType) = 0;

    virtual void setDoorLatchStatus(uint8_t status) = 0;
    virtual void setDoorOpenStatus(uint8_t status)= 0;
    virtual void setWindowStatus(uint16_t sigId, uint8_t sigVal)= 0;

    virtual void setDistanceToEmpty(uint16_t value)= 0;
    virtual void setOdometerMasterValue(uint32_t value)= 0;
    virtual void setODODisplayedMiles(uint32_t value)= 0;
    virtual void setFuelGaugePosition(uint8_t value)= 0;

    virtual RS_ServiceType getCurrBCMauthService()= 0;
    virtual void setCurrBCMauthService(RS_ServiceType service)= 0;

    virtual bool getCrashStatus(RS_ServiceType serviceType)= 0;
    virtual RS_AlarmMode getCurrAlarmStatus()= 0;
    virtual void setCurrAlarmStatus(RS_AlarmMode alarmStatus)= 0;
    virtual bool getAllDoorLockStatus()= 0;
    virtual bool getSVTstatus(RS_ServiceType serviceType)= 0;

    virtual int8_t updateFromCan_FFHOperatingStatus()= 0;
    virtual int8_t updateFromCan_parkClimateMode()= 0;

    virtual bool updateSeatStatus_L405()= 0;
};
#endif /**_REMOTE_SERVICE_APPLICATION_BASE_H_*/
