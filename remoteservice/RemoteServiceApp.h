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

#ifndef _REMOTE_SERVICE_APPLICATION_H_
#define _REMOTE_SERVICE_APPLICATION_H_


#include "RemoteServiceAppBase.h"
#include "services/TelephonyManagerService/base/TelephonyManager.h"

#define KEEP_POWER
#ifdef KEEP_POWER
#include "KeepPower.h"
#endif

class RemoteServiceApp : public Application, public RemoteServiceAppBase
{

public:
    RemoteServiceApp();
    ~RemoteServiceApp();
    void onCreate();
    void onDestroy();
//    RS_ServiceState RemoteService_getServiceState();
    bool RemoteService_getRemoteStartStatus();
    RS_PowerMode RemoteService_getPowerMode();
    int8_t RemoteService_getCANBusState();
    bool RemoteService_IsCanBusWaking();
    void RemoteService_receiveMessageFromMCU(uint16_t sigid, uint8_t* buf, uint32_t size);
    void RemoteService_receiveBCMauthStatus(RS_ServiceType reqType, uint8_t* buf, uint32_t size);
    void RemoteService_receiveBCMauthStatus_ok(RS_ServiceType reqType, uint8_t* buf, uint32_t size);
    void RemoteService_receiveBCMauthStatus_ng(RS_ServiceType reqType, uint8_t* buf, uint32_t size);
    void RemoteService_receiveMessageFromTSP(RS_ServiceType remoteServiceType);
    void RemoteService_receiveMessageFromTSP_ScheduledWakeup(RS_ServiceType remoteServiceType);
    void RemoteService_receiveMessageFromDM(int32_t id, uint8_t cmd1, uint8_t cmd2);
    void RemoteService_receiveMessageFromPM(int8_t newMode);
    void RemoteService_receiveMessageFromDiag(uint32_t id, uint8_t cmd1, uint8_t cmd2, sp<Buffer>& buf);
    void RemoteService_receiveMessageFromConfig(const sp<sl::Message> &msg);
    void RemoteService_receiveMessageFromTelephony(int32_t what, int32_t arg1);
    void RemoteService_receiveMessageFromCallbackAlarmExpireListener(int32_t what, int32_t arg1);
    void RemoteService_receiveMessageFromCanBusStateReceiver(int32_t what, int32_t arg1);
    void RemoteService_receiveMessageFromMCUFrame(int32_t what, int32_t arg1);
    void RemoteService_DBGcmdFailResponse(RS_ServiceType ServiceType);
    void RemoteService_sendDBGcmdResponse(uint8_t cmd, uint8_t cmd2, sp<Buffer> &buf);
    error_t RemoteService_reqSendCANdata(uint8_t nSig, sp<vifCANContainer> &outdata);
    void RemoteService_askUpdateMessageToRVM(request_to_rvm_type msgType, int8_t param1 = 0);
    void RemoteService_requestHandler(RS_RemoteService rs_service, RS_ServiceType serviceType);
    void rcvMCU_action_ParkClimateModeRX(uint8_t sigVal);
    void rcvMCU_action_FFHOperatingStatusRX(uint8_t sigVal);

private:
    void onPostReceived_slddRism(const sp<Post>& post);
    bool mcuSignal_Valiable(uint16_t sigid);
    bool mcuSignal_response(uint16_t sigid);
    bool mcuSignal_hvac(uint16_t sigid);
    void rcvMCU_variale(uint16_t sigid, uint8_t* buf, uint32_t size);
    void rcvMCU_response(uint16_t sigid, uint8_t* buf, uint32_t size);
    void rcvMCU_hvac(uint16_t sigid, uint8_t *buf, uint32_t size);
    void rcvMCU_ParkClimateModeRX(uint16_t sigid, uint8_t *buf, uint32_t size);
    void rcvMCU_action_ParkClimateModeRX_RES_whilstEngineHeating(uint8_t sigVal);
    void rcvMCU_action_ParkClimateModeRX_RES_priorEngineHeating(uint8_t sigVal);
    void rcvMCU_action_ParkClimateModeRX_RPC(uint8_t sigVal);
    void rcvMCU_FFHOperatingStatusRX(uint16_t sigid, uint8_t *buf, uint32_t size);
    void rcvMCU_DoorLatchStatusRX(uint16_t sigid, uint8_t* buf, uint32_t size);
    void rcvMCU_windowPosition(uint16_t sigid, uint8_t* buf, uint32_t size);
//    void rcvMCU_seatInhib(uint16_t sigid, uint8_t* buf, uint32_t size);
//    void rcvMCU_seatPosition(uint16_t sigid, uint8_t* buf, uint32_t size);
    void rcvMCU_remoteStartStatusMSRX(uint16_t sigid, uint8_t* buf, uint32_t size);
    void rcvMCU_warmupVariable(uint16_t sigid, uint8_t* buf, uint32_t size);
    void rcvMCUFrame_fuel();
    void rcvMCUFrame_resRelevantValiables();
    void rcvMCUFrame_seat(int32_t arg1);
    void rcvTSP_RVI(RS_ServiceType serviceType);
    void rcvTSP_RES(RS_ServiceType serviceType);
    void rcvTSP_RCC(RS_ServiceType serviceType);
    void rcvTSP_RPC(RS_ServiceType serviceType);
    void rcvTSP_RISM(RS_ServiceType serviceType);

    void RemoteService_responseHandler(RS_RemoteService rs_service, RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size);

    void resetRVC();

public:
    void createTimer();
    void deleteTimer();
    void startTimer(RS_ServiceType serviceType);
    void stopTimer(RS_ServiceType serviceType);
    void registerReceiver();
    void unregisterReceiver();
    void registerSVTpostReceiver();
    uint32_t get_NGTP_time();
    void initRISM();
//    void getSeatInhibStatus();
//    void getSeatPosStatus();
//    void seatStatusChanged();
    void resetSeatMoveReq();
    void resetSeatMoveReq_L462();
    void resetSeatMoveReq_L405();

    void stopRESrunTimer();
    void reqDefaultParkClimate();
    bool getParkClimateSystemPresent();
    bool getParkClimateRemoteEnabled();
    bool IsVehicleWithFFH();
    svc::ClimateOperatingStatus getClimateOperatingStatus();
    void getServiceDataClimateStatus();
    void getServiceDataTuStatus();
    uint32_t getRESremainingMinute();

    void onNotifyFromMCU(uint16_t Sigid, sp<Buffer>& buf);
    void onNotifyFromMCUFrame(uint16_t FrameId, sp<vifCANFrameSigData>& FrameSigData);
    void onNotifyFromTSP(InternalNGTPData *data, uint32_t size);
    void onNotifyFromPowerStateMgr(int32_t newState, int32_t reason);
    void onNotifyFromPowerModeMgr(int8_t newMode);
    void onNotifyFromCANBusStateMgr(int8_t newState);
    void onNotifyFromDebugMgr(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);
    void onNotifyFromDiagMgr(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);
    void onNotifyFromConfiguration(sp<Buffer>& buf);
    void onPsimLockStateChanged(bool locked);
    void onTsimLockStateChanged(bool locked);
    void onActiveSimStateChanged(int32_t activeSim);
    void onAlarmExpired(const int32_t param);

    virtual void onHMIReceived(const uint32_t type, const uint32_t action);
    virtual void onPostReceived(const sp<Post>& post);

    virtual uint32_t getAppID() { return APP_ID_REMOTESERVICE; }
    virtual sp<IvifManagerReceiver>& getVifReceiver() { return m_vifReceiver; }
    virtual sp<IConfigurationManagerService>& getConfigManager() { return m_configManager; }

    RS_ServiceState getCurrState(RS_ServiceType service) { return mCurrentState[service]; }
    void setCurrState(RS_ServiceType service, RS_ServiceState state);
    void setAllServiceState(RS_ServiceState state);
//    void initRISMServiceState();
//    void setRISMServiceState(RS_ServiceState state);
    void initSLDDstatusProperty(RS_ServiceType serviceType);
    void initSLDDstatusProperty_RH();
    void setRVCproperty(RS_ServiceType serviceType, char* key, RVCproperty prop, const char* value);
    void startRequestService(int32_t ngtpServiceType, uint32_t seconds, uint32_t millis);
    bool checkDuplicateReqFromTSP(int32_t ngtpServiceType, uint32_t seconds, uint32_t millis);
    bool checkServiceState(int32_t ngtpServiceType);

    virtual RS_ServiceType getCurrBCMauthService();
    virtual void setCurrBCMauthService(RS_ServiceType service);


    bool getCurrRemoteStartStatus();
    void setCurrRemoteStartStatus(bool status);

    bool getDBGcmdActiveStatus();
    void setDBGcmdActiveStatus(bool status);

    int32_t getCurrDBGcmdId();
    void setCurrDBGcmdId(int32_t cmdId);

    bool getSLDDcmdActiveStatus();
    void setSLDDcmdActiveStatus(bool status);

    int8_t getCurrPowerMode();
    void setCurrPowerMode(int8_t newMode);

    void initRESremainingTime();

    uint8_t getRESstopReason();
    void setRESstopReason(RES_StopReason reason);

    uint16_t getFuelLevelIndicated();
    void setFuelLevelIndicated(uint16_t level);

//    uint8_t getFuelLevelIndicatedQF();
    void setFuelLevelIndicatedQF(uint8_t accuracy);

    bool getFuelLevelLow();
    void setFuelLevelLow(bool status);

    bool getSVTactive();
    void setSVTactive(bool status);

    bool getSubscriptSVT();
    void setSubscriptSVT(bool status);

    bool getRESTransmissionStatus();
    void setRESTransmissionStatus(bool status);
    bool getRESEngineRunningStatus();
    void setRESEngineRunningStatus(bool status);
    bool getRESVehStationaryStatus();
    void setRESVehStationaryStatus(bool status);

    bool getEngOBDMil();
    void setEngOBDMil(bool status);

    int8_t getPrevParkClimateModeTCU();
    void setPrevParkClimateModeTCU(int8_t mode);

    int8_t getCurrParkClimateModeTCU();
    void setCurrParkClimateModeTCU(int8_t mode);

    int8_t getParkClimateModeWhenReqEngineHeating();
    void setParkClimateModeWhenReqEngineHeating(int8_t mode);
    uint8_t getCurrParkClimateReqTCU();
    void setCurrParkClimateReqTCU(uint8_t reqMode);

    uint8_t getParkHeatVentTime();
    void setParkHeatVentTime(uint8_t minutes);

    bool getRPCtimerActiveStatus();
    void setRPCtimerActiveStatus(bool status);

    uint8_t getFFHOperatingStatus();
    void setFFHOperatingStatus(uint8_t status);

    uint32_t getTimer1Time();
    uint32_t getTimer1Month();
    uint32_t getTimer1Day();
    uint32_t getTimer1Hour();
    uint32_t getTimer1Minute();
    uint32_t getTimer2Time();
    uint32_t getTimer2Month();
    uint32_t getTimer2Day();
    uint32_t getTimer2Hour();
    uint32_t getTimer2Minute();
    void setTimer1Time(uint32_t time);
    void setTimer2Time(uint32_t time);

    bool getFrontSystemOnCmd();
    void setFrontSystemOnCmd(bool status);
    void setHFSCommand(bool status);
    void setHRWCommand(bool status);
    void setHeatedSeatFLModeReq(uint8_t status);
    void setHeatedSeatFLRequest(uint8_t status);
    void setHeatedSeatFRModeReq(uint8_t status);
    void setHeatedSeatFRRequest(uint8_t status);
    void setHeatedSeatRLModeReq(uint8_t status);
    void setHeatedSeatRLRequest(uint8_t status);
    void setHeatedSeatRRModeReq(uint8_t status);
    void setHeatedSeatRRRequest(uint8_t status);

    void setSeatPositionStatus(uint8_t seat, uint8_t position);
    uint8_t getSeatPositionStatus(uint8_t seat);
    void setSeatInhibitStatus(uint8_t seat, uint8_t status);
    uint8_t getSeatInhibitStatus(uint8_t seat);

    bool getCrashStatus(RS_ServiceType serviceType);
    RS_AlarmMode getCurrAlarmStatus();
    void setCurrAlarmStatus(RS_AlarmMode alarmStatus);
    bool getAllDoorLockStatus();
    uint8_t getDoorStatus();
    bool getSVTstatus(RS_ServiceType serviceType);
    uint8_t getSunroofOpenStatus();
    void setSunroofOpenStatus(uint8_t status);
    uint8_t getRoofPositionStatus();
    void setRoofPositionStatus(uint8_t status);
    uint8_t getRESSequenceStatus();
    void setRESSequenceStatus(uint8_t status);

    void reqBCMauthentication(RS_ServiceType service);

    //void sendDSPTackMsgToTSP(RS_ServiceType remoteServiceType);
    void sendAckMsgToTSP(RS_ServiceType remoteServiceType, svc::ServiceMessageType svcMessageType);
    void sendNackMsgToTSP(RS_ServiceType remoteServiceType);
    void sendAckMsgToTSP_RISM(RS_ServiceType remoteServiceType, svc::ServiceMessageType svcMessageType, bool appAck, uint8_t errorCode = 0);
    void sendNackMsgToTSP_RISM(RS_ServiceType remoteServiceType, uint8_t errorCode);
    void sendRISMUpdateMsgToTSP();

    void TcuCheckForUserInterruptionDuringEngineHeat();
    bool TcuCheckForUserInterruptionDuringEngineHeat_FFHstopByUser();
    void stopRESuponLowFuel();

private:
    void assembleMsgToTSP(svc::NGTPServiceData::serviceDataCoreMessage::uplink *uplink);
    void assembleMsgToTSP_vehicleStatus(svc::NGTPUplinkCore::vehicleStatus *vSts);
    void assembleMsgToTSP_extendedVehicleStatus(svc::NGTPUplinkCore::extendedVehicleStatus *evSts);
    void assembleMsgToTSP_windowStatus(svc::NGTPUplinkCore::extendedVehicleStatus::windowStatus *wSts);
    void assembleMsgToTSP_doorStatus(svc::NGTPUplinkCore::extendedVehicleStatus::doorStatus *dSts);
    void assembleMsgToTSP_odometerValue(svc::NGTPUplinkCore::extendedVehicleStatus::odometerValue *oVal);
    void assembleMsgToTSP_tyreStatuses(svc::NGTPUplinkCore::extendedVehicleStatus::tyreStatuses *tyreSts);
    void assembleMsgToTSP_bulbFailures(svc::NGTPUplinkCore::extendedVehicleStatus::bulbFailures *bulbFail);
    void assembleMsgToTSP_climateStatus(svc::NGTPUplinkCore::climateStatus *cSts);
//    void assembleMsgToTSP_tuStatus(svc::NGTPUplinkCore::tuStatus *tuSts);
//    void assembleMsgToTSP_diagnostics(svc::NGTPUplinkCore::tuStatus::diagnostics *diag);
//    void assembleMsgToTSP_battery(svc::NGTPUplinkCore::tuStatus::battery *batt);
//    void assembleMsgToTSP_sleepCyclesStartTime(svc::NGTPUplinkCore::tuStatus::sleepCyclesStartTime *sleepCycleTime);
    void assembleMsgToTSP_RISM(svc::NGTPServiceData::serviceDataCoreMessage::uplink *uplink, svc::ServiceMessageType svcMessageType, bool appAck, uint8_t errorCode);
    void assembleMsgToTSP_rismAcknowledge(svc::NGTPUplinkCore::rismAcknowledge *rismAck, bool appAck, uint8_t errorCode);
    void assembleMsgToTSP_rismResponse(svc::NGTPUplinkCore::rismResponse *rismResp);
    void assembleMsgToTSP_rismUpdate(svc::NGTPUplinkCore::rismUpdate *rismUpdate);

public:
    virtual void setDoorLatchStatus(uint8_t status);
    virtual void setDoorOpenStatus(uint8_t status);
    virtual void setWindowStatus(uint16_t sigId, uint8_t sigVal);

    virtual void setDistanceToEmpty(uint16_t value);
    virtual void setOdometerMasterValue(uint32_t value);
    virtual void setODODisplayedMiles(uint32_t value);
    virtual void setFuelGaugePosition(uint8_t value);
    void setVehicleStatusInit();
    void setNGTPerrorCode(uint8_t errorCode);
    void initNGTPerrorCode();
    void getRISMerrorCode(svc::NGTPUplinkCore::rismAcknowledge::errorCode *ngtpErrCode, uint8_t errorCode);

    int32_t getNgtpServiceType(RS_ServiceType remoteServiceType);
    uint32_t getNgtpTimeSec(RS_ServiceType remoteServiceType);
    uint32_t getNgtpTimeMil(RS_ServiceType remoteServiceType);
    svc::VehicleStateType getNgtpVehicleState();
    svc::TheftAlarmStatus getNgtpTheftAlarm();
    svc::BOOLEXT getNgtpSunroofOpenStatus();
    svc::BOOLEXT getNgtpCabOpenStatus();
    svc::BOOLEXT getNgtpBootOpenStatus();
    svc::SeatMovementInhibition getNgtpSeatMovementInhibition(seatMovementInhibition inhibition);
    svc::SeatMovementStatus getNgtpSeatMovementState(seatMovementStatus state);
    svc::SeatMovementStatus getNgtpSeatMovementState_armRest(seatMovementStatus state);
    svc::SeatMovementStatus getNgtpSeatMovementState_skiHatch(seatMovementStatus state);
    svc::BOOLEXT getNgtpCrashSituation();

    uint32_t getREONngtpTimeSec();
    uint32_t getREONngtpTimeMil();
    uint32_t getREOFFngtpTimeSec();
    uint32_t getREOFFngtpTimeMil();
    uint32_t getRHONngtpTimeSec();
    uint32_t getRHONngtpTimeMil();
    uint32_t getRHOFFngtpTimeSec();
    uint32_t getRHOFFngtpTimeMil();

    uint32_t getRDUngtpTimeSec();
    uint32_t getRDLngtpTimeSec();
    uint32_t getHBLFngtpTimeSec();
    uint32_t getALOFFngtpTimeSec();
    uint32_t getRDUngtpTimeMil();
    uint32_t getRDLngtpTimeMil();
    uint32_t getHBLFngtpTimeMil();
    uint32_t getALOFFngtpTimeMil();
    uint32_t getRISMMoveNgtpTimeSec();
    uint32_t getRISMMoveNgtpTimeMil();
    uint32_t getRISMCancelNgtpTimeSec();
    uint32_t getRISMCancelNgtpTimeMil();
    uint32_t getRISMUpdateNgtpTimeSec();
    uint32_t getRISMUpdateNgtpTimeMil();
    uint32_t getScheduledWakeupNgtpTimeSec();
    uint32_t getScheduledWakeupNgtpTimeMil();

    void setRISMMoveNgtpTimeSec(uint32_t sec);
    void setRISMMoveNgtpTimeMil(uint32_t mil);
    void setRISMCancelNgtpTimeSec(uint32_t sec);
    void setRISMCancelNgtpTimeMil(uint32_t mil);
    void setRISMUpdateNgtpTimeSec(uint32_t sec);
    void setRISMUpdateNgtpTimeMil(uint32_t mil);
    void setREONngtpTimeSec(uint32_t sec);
    void setREONngtpTimeMil(uint32_t mil);
    void setREOFFngtpTimeSec(uint32_t sec);
    void setREOFFngtpTimeMil(uint32_t mil);
    void setRHONngtpTimeSec(uint32_t sec);
    void setRHONngtpTimeMil(uint32_t mil);
    void setRHOFFngtpTimeSec(uint32_t sec);
    void setRHOFFngtpTimeMil(uint32_t mil);

    void setRDUngtpTimeSec(uint32_t sec);
    void setRDLngtpTimeSec(uint32_t sec);
    void setHBLFngtpTimeSec(uint32_t sec);
    void setALOFFngtpTimeSec(uint32_t sec);
    void setRDUngtpTimeMil(uint32_t mil);
    void setRDLngtpTimeMil(uint32_t mil);
    void setHBLFngtpTimeMil(uint32_t mil);
    void setALOFFngtpTimeMil(uint32_t mil);

    void setScheduledWakeupNgtpTimeSec(uint32_t sec);
    void setScheduledWakeupNgtpTimeMil(uint32_t mil);
    void createRISMUpdateNgtpTime();

    bool getRESWindowStatus();
    void setRESWindowStatus(bool status);
    bool getRESVehicleLockStatus();
    void setRESVehicleLockStatus(bool status);
    bool getRESAlarmStatus();
    void setRESAlarmStatus(bool status);
    bool getRESHazardSwitchStatus();
    void setRESHazardSwitchStatus(bool status);
    bool getRESBrakePedalStatus();
    void setRESBrakePedalStatus(bool status);

    bool getRDUTsimLockState();
    void setRDUTsimLockState(bool status);
    bool getRESTsimLockState();
    void setRESTsimLockState(bool status);

    void requestSimLock(RS_ServiceType remoteServiceType);

    bool RvcTsimLockAcquire();
    bool RvcTsimLockRelease();
    bool RvcPsimLockAcquire();
    bool RvcPsimLockRelease();

    void RvcKeepPowerAcquire();
    void RvcKeepPowerRelease();
    void RvcAllKeepPowerRelease();

    bool checkSeatMovedAsRequested_L462();
    bool checkSeatMovedAsRequested_L405();
    void foldRequestBySldd(int32_t seatPosition);
    void unfoldRequestBySldd(int32_t seatPosition);

    seatMovementCommand getSeatMovementCommand_2D();
    seatMovementCommand getSeatMovementCommand_2P();
    seatMovementCommand getSeatMovementCommand_3D();
    seatMovementCommand getSeatMovementCommand_3P();
    seatMovementStatus getSeatMovementStatus_1P();
    seatMovementStatus getSeatMovementStatus_2D();
    seatMovementStatus getSeatMovementStatus_2P();
    seatMovementStatus getSeatMovementStatus_3D();
    seatMovementStatus getSeatMovementStatus_3P();
    seatMovementInhibition getSeatMovementInhibition_1P();
    seatMovementInhibition getSeatMovementInhibition_2D();
    seatMovementInhibition getSeatMovementInhibition_2P();
    seatMovementInhibition getSeatMovementInhibition_3D();
    seatMovementInhibition getSeatMovementInhibition_3P();
    seatMovementCommand getSeatMovementRequest_2D();
    seatMovementCommand getSeatMovementRequest_2P();
    seatMovementCommand getSeatMovementRequest_3D();
    seatMovementCommand getSeatMovementRequest_3P();
    void setSeatMovementRequest_2D(seatMovementCommand request);
    void setSeatMovementRequest_2P(seatMovementCommand request);
    void setSeatMovementRequest_3D(seatMovementCommand request);
    void setSeatMovementRequest_3P(seatMovementCommand request);

    // for L405 vehicle
    seatMovementCommand getSeatMovementCommand_2L();
    seatMovementCommand getSeatMovementCommand_2C();
    seatMovementCommand getSeatMovementCommand_2R();
    seatMovementCommand getSeatMovementCommand_skiHatch();
    seatMovementStatus getSeatMovementStatus_2L();
    seatMovementStatus getSeatMovementStatus_2C();
    seatMovementStatus getSeatMovementStatus_2R();
    seatMovementStatus getSeatMovementStatus_skiHatch() ;
    seatMovementInhibition getSeatMovementInhibition_2L();
    seatMovementInhibition getSeatMovementInhibition_2C();
    seatMovementInhibition getSeatMovementInhibition_2R();
    seatMovementCommand getSeatMovementRequest_2L();
    seatMovementCommand getSeatMovementRequest_2C();
    seatMovementCommand getSeatMovementRequest_2R();
    seatMovementCommand getSeatMovementRequest_skiHatch();
    void setSeatMovementRequest_2L(seatMovementCommand request);
    void setSeatMovementRequest_2C(seatMovementCommand request);
    void setSeatMovementRequest_2R(seatMovementCommand request);
    void setSeatMovementRequest_skiHatch(seatMovementCommand request);

    void makeLog();
    void assembleLog(uint32_t ngtpTime,uint8_t logIndex);
    uint8_t getLogSuccess();
    uint8_t getLogserviceType();
    void setLogserviceType(uint8_t status);
    uint8_t getLogTransistionFromREStoNormal();
    void setLogTransistionFromREStoNormal(uint8_t status);
    uint8_t getLogPrerequisiteCheckRequestStatus();
    void setLogPrerequisiteCheckRequestStatus(uint8_t status);
    uint8_t getLogPowerModeWhenRequestReceived();
    void setLogPowerModeWhenRequestReceived(uint8_t status);
    uint8_t getLogRESSequenceStatusPrevious();
    void setLogRESSequenceStatusPrevious(uint8_t status);
    uint8_t getLogffhStatusWhenFFHStopped();
    void setLogffhStatusWhenFFHStopped(uint8_t sigVal);
    uint8_t getLogCrashStatusCurrentIgnitionCycle();
    uint8_t getLogCrashStatusPreviousIgnitionCycle();
    uint16_t getLogMaxRunTimeReached();
    uint8_t getLogfuelLevel();
    void setLogfuelLevel(uint8_t low);
    uint16_t getLogResRunTimer();
    uint16_t getLogFfhRunTimer();
    void setLogFfhRunTimer(bool bStart = false);

    uint8_t getUint8fromConfig(const char* name);
    uint16_t getUint16fromConfig(const char* name);
    uint32_t getUint32fromConfig(const char* name);
    void setUint16toConfig(const char* name, uint16_t value);
    bool getCurrCANData(uint16_t sigid, sp<Buffer>& sigdatabuf, bool recevedAfterCanWakeup = false);
    void startTimerId(uint32_t timerId, bool bRestart = false);
    void stopTimerId(uint32_t timerId);
    void setTimerStatus(uint32_t timerId, bool bset);
    void initTimerStatus();
    bool const isTimerOn(uint32_t timerId);

    int8_t updateFromCan_FFHOperatingStatus();
    int8_t updateFromCan_parkClimateMode();
//    bool updateFromCanFrame_resRelevantValiables();
//    bool updateFromCanFrame_fuelRelevantValiables();

    void setCanVariant(uint8_t val);
    uint8_t getCanVariant();
    void updateFromProperty_RPCvariables();
    void saveProperty_RPCvariables(uint16_t sigid, uint8_t sigVal);
    void setProperty_seats(uint16_t sigid, uint8_t sigVal);
    uint8_t getProperty_seats(uint16_t sigid);

    uint32_t getPropertyInt(const char* name);

    bool isRISMFitted();
    bool isRISMAppEnabled();
    uint8_t getAppModeRISM();
    void setAppModeRISM(uint8_t appModeRISM);
    uint8_t getConvenienceFold();
    void setConvenienceFold(uint8_t convenienceFold);
    uint8_t getVehicleType();
    void setVehicleType(uint8_t vehicleType);
    uint8_t getIntelligentSeatFold();
    void setIntelligentSeatFold(uint8_t intelligentSeatFold);
    bool isSeatType3();
    bool updateSeatStatus_L405();
    bool updateSeatStatus_L462();

private:
    void updateFromConfig_sunroofExist();
    void updateFromConfig_movingWindows();
    bool const isSunroofExist();
    int8_t const getMovingWindows();

    void updateWarmupVariable();
    void updateSecureState();
//    void updateFFHPrereq();
    bool updateParkClimateStatusSampling();
    uint8_t updateFromCan_FuelLevelIndicatedQFHS();
    void updateFromCan_FuelLevelIndicatedHS();
    void updateFromCan_FuelGaugePosition();
    void updateFromCan_OdometerMasterValue();
    void updateFromCan_ODODisplayedMiles();
    void updateFromCan_DistanceToEmpty();
    void updateFromCan_doorLatch();
    void updateFromCan_WindowPosition();
    void updateFromCan_sunroofOpen();
    void updateFromCan_roofPositionStatus();
    void updateFromCan_ParkHeatVentTime();
    void updateResRelevantValiables();
    void updateFromCan_RESWindowStatus();
    void updateFromCan_RESVehicleLockStatus();
    void updateFromCan_RESAlarmStatus();
    void updateFromCan_RESHazardSwitchStatus();
    void updateFromCan_RESBrakePedalStatus();
    void updateFromCan_RESVehStationaryStatus();
    void updateFromCan_RESTransmissionStatus();
    void updateFromCan_RESEngineRunningStatus();
    void updateFromCan_RESSequenceStatus();
    void updateFromCan_TimerActivationStatus();
    void updateFromCan_Timer1Time();
    void updateFromCan_Timer2Time();
    void updateFromCan_DoorStatusHS();
    void updateFromCan_HFSCommand();
    void updateFromCan_HRWCommand();
    void updateFromCan_RemoteStartStatusMS();
    void updateFromCan_AlarmModeHS();
    int8_t updateFromCan_Internal_Serv_CAN_VARIANT();
    int8_t updateFromCan_RLSeatMovementStat();
    int8_t updateFromCan_RCntrSeatMovmentStat();
    int8_t updateFromCan_RRSeatMovementStat();
    int8_t updateFromCan_SkiHatchStat();
    int8_t updateFromCan_RLSeatInhib2ndRow();
    int8_t updateFromCan_RMSeatInhib2ndRow();
    int8_t updateFromCan_RRSeatInhib2ndRow();
    int8_t updateFromCan_PasSeatPosition1stRow();
    int8_t updateFromCan_DrvSeatPosition2ndRow();
    int8_t updateFromCan_PasSeatPosition2ndRow();
    int8_t updateFromCan_DrvSeatPosition3rdRow();
    int8_t updateFromCan_PasSeatPosition3rdRow();
    int8_t updateFromCan_PasSeatInhib1stRow();
    int8_t updateFromCan_DrvSeatInhib2ndRow();
    int8_t updateFromCan_PasSeatInhib2ndRow();
    int8_t updateFromCan_DrvSeatInhib3rdRow();
    int8_t updateFromCan_PasSeatInhib3rdRow();

    void RemoteService_recalcurateResRunMax();

    bool decodeNGTPSvcData(uint8_t *buffer, uint32_t size);
    bool decodeNGTPSvcData_scheduledWakeup(uint8_t *buffer, uint32_t size);
    error_t setScheduledWakeUpSecond(uint32_t second);

private:
    class MyHandler : public Handler
    {
        public:
            static const uint32_t RECV_MSG_FROM_MCU = 0;
            static const uint32_t RECV_MSG_FROM_TSP = 1;
            static const uint32_t RECV_MSG_FROM_HMI = 2;
            static const uint32_t RECV_MSG_FROM_PM = 3;
            static const uint32_t RECV_MSG_FROM_DM = 4;
            static const uint32_t RECV_MSG_FROM_DIAG = 5;
            static const uint32_t RECV_MSG_FROM_CONFIG = 6;
            static const uint32_t RECV_MSG_PSIM_LOCK_STATE_CHANGED = 7;
            static const uint32_t RECV_MSG_TSIM_LOCK_STATE_CHANGED = 8;
            static const uint32_t RECV_MSG_ACTIVESIM_STATE_CHANGED = 9;
            static const uint32_t RECV_MSG_ALARM_EXPIRED = 10;
            static const uint32_t RECV_MSG_CAN_BUS_STATE = 11;
            static const uint32_t RECV_MSG_FROM_MCU_FRAME = 12;

        public:
            MyHandler(sp<SLLooper>& looper, RemoteServiceApp& outer)
                :Handler(looper), mApplication(outer) {}
            virtual ~MyHandler() {}

            virtual void handleMessage(const sp<sl::Message>& msg);

        private:
            RemoteServiceApp& mApplication;
    };

public:
    class MyTimer : public TimerTimeoutHandler
    {
        public:
            static const uint32_t TIMER_RES_START = 0;          //90sec RES_ENG_START_TIME
            static const uint32_t TIMER_RES_STOP = 1;           //2.3sec RES_STOP_REQ_RESP_TIME
            static const uint32_t TIMER_RES_PREREQ = 2;         //1sec RES_PREREQ_RESPONSE_TIME
            static const uint32_t TIMER_FFH_MAX_RUN = 3;        //1800sec RES_FFH_RUN_TIME_MAX
            static const uint32_t TIMER_RES_MAX_RUN = 4;
            static const uint32_t TIMER_CLIMATE_REQ = 5;        //35sec PARK_CLIMATE_STARTUP_TIME
            static const uint32_t TIMER_FFH_USER_STOP = 6;      //10sec RES_FFH_STOPPEDBY_USER_DETECTION_TIME
            static const uint32_t TIMER_UNLOCK_VEHICLE = 7;     //2sec DOOR_UNLOCK_TIMEOUT
            static const uint32_t TIMER_SECURE_VEHICLE = 8;     //60sec DOOR_UNLOCK_TIMEOUT
            static const uint32_t TIMER_ALARM_RESET = 9;        //30sec REMOTE_VEHICLE_ALARM_RESET_TIMEOUT
            static const uint32_t TIMER_TCU_AUTH_SEQ = 10;      //15 temp.
            static const uint32_t TIMER_PARK_CLIMATE_START = 11;
            static const uint32_t TIMER_PARK_CLIMATE_STOP = 12;
            static const uint32_t TIMER_RPC_STATUS_WAIT = 13;
            static const uint32_t TIMER_PARKHEATVENT_WAIT = 14; //4.1sec PARKHEATVENTTIME_SAMPLE_WAIT_TIME
            static const uint32_t TIMER_RISM_SEAT_READY = 15;
            static const uint32_t TIMER_RISM_SEAT_MOVE = 16;
            static const uint32_t TIMER_NGTP_RETRY = 17;
            static const uint32_t TIMER_RISM_CANCEL_MOVE = 18;  //5sec RISM_CANCEL_MOVE_TIME
            static const uint32_t TIMER_MIL_FAULT_DETECT = 19;  //12sec MIL_FAULT_DETECTION_TIME
            static const uint32_t TIMER_REG_SVT_STATUS =20;
            static const uint32_t TIMER_SKIP_WARMUP_VALIABLE =21;   //4sec. customizes
            static const uint32_t TIMER_PARK_CLIMATE_SHUTDOWN =22;  //5sec PARK_CLIMATE_SHUTDOWN_TIME
            static const uint32_t TIMER_PSIM_LOCK = 23;
            static const uint32_t TIMER_PARK_CLIMATE_MIN_STATUS_WAIT = 24;  //6sec PARK_CLIMATE_MIN_STATUS_WAIT_TIME
            static const uint32_t TIMER_UPDATE_RES_REMAINING = 25;  //60sec update every 1min
            static const uint32_t TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP = 26;
            static const uint32_t TIMER_RISM_RELEASE_KEEPPOWER = 27; //50sec. retry 10sec * 5times
            static const uint32_t TIMER_KEEPPOWER_RELEASE = 28;
            static const uint32_t TIMER_PARK_CLIMATE_STATUS_TIMEOUT = 29;

            static const uint32_t TIMER_END_NUM = 30;

        public:
            MyTimer(RemoteServiceApp& s):mApplication(s) {}
            virtual ~MyTimer() {}

            void handlerFunction( int );

        private:
            void handlerFunction_ResStart();
            void handlerFunction_ResStop();
            void handlerFunction_ResPrereq();
            void handlerFunction_FfhMaxRun();
            void handlerFunction_ResMaxRun();
            void handlerFunction_ClimateReq();
            void handlerFunction_FfhUserStop();
            void handlerFunction_UnlockVehicle();
            void handlerFunction_SecureVehicle();
            void handlerFunction_AlarmReset();
            void handlerFunction_TcuAuthSeq();
            void handlerFunction_ParkClimateStart();
            void handlerFunction_ParkClimateStop();
            void handlerFunction_RismSeatReady();
            void handlerFunction_RismSeatMove();
            void handlerFunction_NgtpRetry();
            void handlerFunction_RismCancelMove();
            void handlerFunction_MilFaultDetect();
            void handlerFunction_RegSvtStatus();
            void handlerFunction_skipWarmupVariable();
            void handlerFunction_parkHeatVentSampleWaitTime();
            void handlerFunction_parkClimateShutdownTime();
            void handlerFunction_PSIMLock();
            void handlerFunction_parkClimateMinStatusWaitTime();
            void handlerFunction_updateResRemainingTime();
            void handlerFunction_parkClimateShutdownTimeForFfhStop();
            void handlerFunction_RISMrelease();
            void handlerFunction_releaseKeepPower();
            void handlerFunction_parkClimateStatusTimeout();

            RemoteServiceApp& mApplication;
    };

private:
    class DebugMgrReceiver : public BnMgrReceiver
    {
        public:
            DebugMgrReceiver(RemoteServiceApp& app):mApplication(app) { targetModuleID = mApplication.getAppID(); }
            virtual ~DebugMgrReceiver() {}

            virtual void onReceive(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
            {
                mApplication.onNotifyFromDebugMgr(id, cmd, cmd2, buf);
            }

        private:
            RemoteServiceApp& mApplication;
    };

    class VifReceiver : public BnvifManagerReceiver
    {
        public:
            VifReceiver(RemoteServiceApp& app):mApplication(app) { targetModuleID = mApplication.getAppID(); }
            virtual ~VifReceiver() {}

            virtual void onReceive(uint16_t Sigid, sp<Buffer>& buf)
            {
                //LOGE("onReceive Sigid:0x%02X", Sigid);
                mApplication.onNotifyFromMCU(Sigid, buf);
            }

            virtual void onReceiveFrame(uint16_t FrameId, sp<vifCANFrameSigData>& FrameSigData)
            {
                mApplication.onNotifyFromMCUFrame(FrameId, FrameSigData);
            }

        private:
            RemoteServiceApp& mApplication;
    };

    class NgtpReceiver : public BnNGTPReceiver
    {
        public:
            NgtpReceiver(RemoteServiceApp& app):mApplication(app) { targetModuleID = mApplication.getAppID(); }
            virtual ~NgtpReceiver() {}

            virtual void onReceive(InternalNGTPData *data, int datalength)
            {
                mApplication.onNotifyFromTSP(data, datalength);
            }

        private:
            RemoteServiceApp& mApplication;
    };

    class PowerStateReceiver : public BnPowerStateReceiver
    {
        public:
            PowerStateReceiver(RemoteServiceApp& app):mApplication(app) {}
            virtual ~PowerStateReceiver() {}

            virtual void onPowerStateChanged(int32_t newState, int32_t reason)
            {
                //LOGE("onPowerStateChanged newState:%d reason:%d", newState, reason);
                mApplication.onNotifyFromPowerStateMgr(newState, reason);
            }

            virtual void onWakeup(int32_t currentState, int32_t reason)
            {
                //LOGE("onWakeup:%d", currentState);
            }

        private:
            RemoteServiceApp& mApplication;
    };

    class PowerModeReceiver : public BnPowerModeReceiver
    {
        public:
            PowerModeReceiver(RemoteServiceApp& app):mApplication(app) {}
            virtual ~PowerModeReceiver() {}

            virtual void onPowerModeChanged(int8_t newMode)
            {
                //LOGE("onPowerModeChanged:%d", newMode);
                mApplication.onNotifyFromPowerModeMgr(newMode);
            }

        private:
            RemoteServiceApp& mApplication;
    };

    class CanBusStateReceiver : public BnCANBusStateReceiver
    {
        public:
            CanBusStateReceiver(RemoteServiceApp& app):mApplication(app) {}
            virtual ~CanBusStateReceiver() {}

            virtual void onCANBusStateChanged(int8_t newState)
            {
                //LOGE("onCANBusStateChanged:%d", newState);
                mApplication.onNotifyFromCANBusStateMgr(newState);
            }

        private:
            RemoteServiceApp& mApplication;
    };

    class DiagMgrReceiver : public BnDiagManagerReceiver
    {
        public:
            DiagMgrReceiver(RemoteServiceApp& app):mApplication(app) {}
            virtual ~DiagMgrReceiver() {}

            virtual void onReceive(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
            {
                LOGV("DiagMgr onReceive called, cmd = 0x%x, cmd2 = 0x%x, ", cmd, cmd2);
                mApplication.onNotifyFromDiagMgr(id, type, cmd, cmd2, buf);
            }
            virtual void onDiagcmd(int32_t id, uint8_t type, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf)
            {

            }
        private:
            RemoteServiceApp& mApplication;
    };

    class ConfigurationReceiver : public BnConfigurationManagerReceiver
    {
        public:
            ConfigurationReceiver(RemoteServiceApp& app):mApplication(app) {}
            virtual ~ConfigurationReceiver() {}

            virtual void onConfigDataChanged(sp<Buffer>& buf)
            {
                LOGE("onConfigDataChanged name_buffer:%s", buf->data());
                mApplication.onNotifyFromConfiguration(buf);
            }
        private :
            RemoteServiceApp& mApplication;
    };

    class TelephonyListener : public PhoneStateListener
    {
        public:
            TelephonyListener(RemoteServiceApp& app) : PhoneStateListener(false), mApplication(app) {}
            virtual ~TelephonyListener() {}
            virtual void onPsimLockStateChanged(bool locked)
            {
                mApplication.onPsimLockStateChanged(locked);
            }
            virtual void onTsimLockStateChanged(bool locked)
            {
                mApplication.onTsimLockStateChanged(locked);
            }
            virtual void onActiveSimStateChanged(int32_t activeSim)
            {
                mApplication.onActiveSimStateChanged(activeSim);
            }
        private:
            RemoteServiceApp& mApplication;
    };

    class CallbackAlarmExpireListener : public BnAlarmExpireListener
    {
        public:
            CallbackAlarmExpireListener(RemoteServiceApp& app) : mApplication(app) {}
            virtual void onAlarmExpired(const int32_t param)
            {
                mApplication.onAlarmExpired(param);
            }

        private:
            RemoteServiceApp& mApplication;
    };


public:
    RemoteServiceBase *m_remoteService[RS_SERVICE_MAX];
    RemoteService_log *m_remoteServiceLog;

private:
    bool mRemoteStartStatus;
    uint8_t RESSequence;
    RS_ServiceState mCurrentState[RS_SERVICE_TYPE_MAX];
    RS_ServiceType mCurrBCMauthService;
    RS_ServiceType mCurrRISMService;
    bool mDBGcmdActiveStatus;
    bool bSLDDcmdActive;
    int32_t mCurrDBGcmdId;
    uint8_t mCurrPowerMode;
    uint8_t mRESstopReason;
    bool bAlreadyCheckPrereq;

    uint16_t mFuelLevelIndicated;
    uint8_t mFuelLevelIndicatedQF;
    bool mFuelLevelLow;

    bool mCrashStatusRCM;
    bool mHardWiredCrash;
    bool mPrevCrashStatus;
    bool mCurrCrashStatus;

    bool mEngOBDMil;
    bool bSVTactive;
    bool bSubscriptSVT;
    bool bSunroofExist;
    uint8_t mMovingWindows;
    bool bAllDoorLockStatus;
    bool bTrunkLockStatus;
    uint8_t mFLdoorLockStatus;
    uint8_t mFRdoorLockStatus;
    uint8_t mRLdoorLockStatus;
    uint8_t mRRdoorLockStatus;
    uint8_t mTailgateLockStatus;
    uint8_t mBonnetLockStatus;
    bool mSunroofOpenStatus; // 0:FullyClosed 1:NotFullyClosed
    uint8_t mRoofPositionStatus;
    uint8_t mRESSequenceStatus;

    uint8_t mDoorStatus;
    uint8_t mFLdoorOpenStatus;
    uint8_t mFRdoorOpenStatus;
    uint8_t mRLdoorOpenStatus;
    uint8_t mRRdoorOpenStatus;
    uint8_t mTailgateOpenStatus;
    uint8_t mBonnetOpenStatus;

    uint8_t mDrvWindowStatus;
    uint8_t mRDrvWindowStatus;
    uint8_t mPasWindowStatus;
    uint8_t mRPasWindowStatus;

    uint8_t mFuelGaugePosition;
    uint16_t mDistanceToEmpty;
    uint32_t mOdometerMasterValue;
    uint32_t mODODisplayedMiles;

    RS_AlarmMode mCurrAlarmStatus;

    bool mFrontSystemOnCmd;
    bool mHFSCommand;
    bool mHRWCommand;
    uint8_t mHeatedSeatFLModeReq;
    uint8_t mHeatedSeatFLRequest;
    uint8_t mHeatedSeatFRModeReq;
    uint8_t mHeatedSeatFRRequest;
    uint8_t mHeatedSeatRLModeReq;
    uint8_t mHeatedSeatRLRequest;
    uint8_t mHeatedSeatRRModeReq;
    uint8_t mHeatedSeatRRRequest;
    int8_t mPrevParkClimateModeTCU;
    int8_t mCurrParkClimateModeTCU;
    int8_t mParkClimateModeWhenReqEngineHeating;
    uint8_t mCurrParkClimateReqTCU;
    bool mRPCtimerActiveStatus;
    uint32_t mTimer1Time;
    uint32_t mTimer1Month;
    uint32_t mTimer1Day;
    uint32_t mTimer1Hour;
    uint32_t mTimer1Minute;
    uint32_t mTimer2Time;
    uint32_t mTimer2Month;
    uint32_t mTimer2Day;
    uint32_t mTimer2Hour;
    uint32_t mTimer2Minute;
    uint8_t mFFHOperatingStatus;
    uint8_t mParkHeatVentTime;  //Minutes

    uint32_t mREONngtpTimeSec;
    uint32_t mREONngtpTimeMil;
    uint32_t mREOFFngtpTimeSec;
    uint32_t mREOFFngtpTimeMil;
    uint32_t mRHONngtpTimeSec;
    uint32_t mRHONngtpTimeMil;
    uint32_t mRHOFFngtpTimeSec;
    uint32_t mRHOFFngtpTimeMil;

    uint32_t mRDUngtpTimeSec;
    uint32_t mRDUngtpTimeMil;
    uint32_t mRDLngtpTimeSec;
    uint32_t mRDLngtpTimeMil;
    uint32_t mHBLFngtpTimeSec;
    uint32_t mHBLFngtpTimeMil;
    uint32_t mALOFFngtpTimeSec;
    uint32_t mALOFFngtpTimeMil;

    uint32_t mRISMMoveNgtpTimeSec;
    uint32_t mRISMMoveNgtpTimeMil;
    uint32_t mRISMCancelNgtpTimeSec;
    uint32_t mRISMCancelNgtpTimeMil;

    uint32_t mRISMUpdateNgtpTimeSec;
    uint32_t mRISMUpdateNgtpTimeMil;

    uint32_t mScheduledWakeupNgtpTimeSec;
    uint32_t mScheduledWakeupNgtpTimeMil;

    RISM_request requestRISM;
    RISM_status statusRISM;
    uint8_t mAppModeRISM;
    uint8_t mConvenienceFold;
    uint8_t mVehicleType;
    uint8_t mIntelligentSeatFold;

    bool bRESWindowStatus;
    bool bRESVehicleLockStatus;
    bool bRESAlarmStatus;
    bool bRESHazardSwitchStatus;
    bool bRESBrakePedalStatus;
    bool mRESTransmissionStatus;
    bool mRESVehStationaryStatus;
    bool mRESEngineRunningStatus;

    bool bRDUTsimLockState;
    bool bRESTsimLockState;

    uint8_t mLogserviceType;
    uint8_t mLogfuelLevel;
    uint8_t mLogTransistionFromREStoNormal;
    uint8_t mLogPrerequisiteCheckRequestStatus;
    uint8_t mLogPowerModeWhenRequestReceived;
    uint8_t mLogffhStatusWhenFFHStopped;
    uint8_t mLogRESSequenceStatusPrevious;
    uint16_t mLogFfhRunTimer;

    //NGTP service data
    climate_parameters mClimateParameters;
    diag_parameters mDiagParameters;
    battery_parameters mBatteryParameters;

    OssIndex mErrorIdx;
    svc::NGTPUplinkCore::errorCodes mErrCode;

    sp<SLLooper> mLooper;
    sp<MyHandler> mHandler;

    MyTimer *m_myTimer;
    Timer *m_RESstartTimer;
    Timer *m_RESstopTimer;
    Timer *m_RESprereqTimer;
    Timer *m_FFHmaxRunTimer;
    Timer *m_RESmaxRunTimer;
    Timer *m_ParkClimateReqTimer;
    Timer *m_FFHuserIrqTimer;
    Timer *m_unlockVehicleTimer;
    Timer *m_secureVehicleTimer;
    Timer *m_alarmResetTimer;
    Timer *m_TCUauthSeqTimer;
    Timer *m_parkClimateStartTimer;
    Timer *m_parkClimateStopTimer;
    Timer *m_RPCstatusWaitTimer;
    Timer *m_RPCheatVentWaitTimer;
    Timer *m_RISMseatReadyTimer;
    Timer *m_RISMseatMoveTimer;
    Timer *m_NgtpRetryTimer;
    Timer *m_RISMcancelMoveTimer;
    Timer *m_MILfaultDetectTimer;
    Timer *m_RegSVTstatusTimer;
    Timer *m_skipWarmupVariableTimer;
    Timer *m_parkClimateShutdownTimer;
    Timer *m_PSIMLockTimer;
    Timer *m_parkClimateMinStatusWaitTimer;
    Timer *m_updateResRemainingTimer;
    Timer *m_parkClimateShutdownTimerForFfhStop;
    Timer *m_RISMupdateReleaseTimer;
    Timer *m_keepPowerReleaseTimer;
    Timer *m_parkClimateStatusTimeout;

    uint32_t timerStatus[MyTimer::TIMER_END_NUM];

    sp<IDebugManagerService> m_debugMgrService;
    sp<IMgrReceiver> m_debugMgrReceiver;
    sp<IvifManagerService> m_vifManager;
    sp<IvifManagerReceiver> m_vifReceiver;
    sp<INGTPManagerService> m_ngtpManager;
    sp<INGTPReceiver> m_ngtpReceiver;
    sp<IPowerManagerService> m_powerMgrService;
    sp<IPowerStateReceiver> m_powerStateReceiver;
    sp<IPowerModeReceiver> m_powerModeReceiver;
    sp<ICANBusStateReceiver> m_canBusStateReceiver;
    sp<IApplicationManagerService> m_appManager;
    sp<IConfigurationManagerService> m_configManager;
    sp<IConfigurationManagerReceiver> m_configReceiver;
    sp<IDiagManagerService> m_diagManager;
    sp<IDiagManagerReceiver> m_diagReceiver;
    sp<TelephonyListener> m_TelephonyListener;
    sp<IDataService> m_dataService;
    sp<TelephonyManager::TsimLock> m_TsimLock;
    sp<TelephonyManager::PsimLock> m_PsimLock;
    sp<IAlarmManagerService> m_alarmManager;
    sp<IAlarmExpireListener> m_CallbackAlarmListener;

#ifdef KEEP_POWER
    KeepPower mKeepPower;
    uint8_t mKeepPowerCnt;
#endif

    uint32_t mScheduledWakeUpSecond;
    uint8_t mCanVariant;

};
#endif /**_REMOTE_SERVICE_APPLICATION_H_*/
