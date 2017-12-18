
#define LOG_TAG "RVC"
#include "Log.h"

#include "RemoteServiceApp.h"



RS_ServiceType RemoteServiceApp::getCurrBCMauthService() { return mCurrBCMauthService; }
void RemoteServiceApp::setCurrBCMauthService(RS_ServiceType service) { mCurrBCMauthService = service; }


bool RemoteServiceApp::getCurrRemoteStartStatus() { return mRemoteStartStatus; }
void RemoteServiceApp::setCurrRemoteStartStatus(bool status) { mRemoteStartStatus = status; }

bool RemoteServiceApp::getDBGcmdActiveStatus() { return mDBGcmdActiveStatus; }
void RemoteServiceApp::setDBGcmdActiveStatus(bool status) { mDBGcmdActiveStatus = status; }

int32_t RemoteServiceApp::getCurrDBGcmdId() { return mCurrDBGcmdId; }
void RemoteServiceApp::setCurrDBGcmdId(int32_t cmdId) { mCurrDBGcmdId = cmdId; }

bool RemoteServiceApp::getSLDDcmdActiveStatus() { return bSLDDcmdActive; }
void RemoteServiceApp::setSLDDcmdActiveStatus(bool status) { bSLDDcmdActive = status; }

int8_t RemoteServiceApp::getCurrPowerMode() { return mCurrPowerMode; }
void RemoteServiceApp::setCurrPowerMode(int8_t newMode) { mCurrPowerMode = newMode; }

uint8_t RemoteServiceApp::getRESstopReason() { return mRESstopReason; }
void RemoteServiceApp::setRESstopReason(RES_StopReason reason) { mRESstopReason = (uint8_t) reason; }

uint16_t RemoteServiceApp::getFuelLevelIndicated() { return mFuelLevelIndicated; }

bool RemoteServiceApp::getFuelLevelLow() { return mFuelLevelLow; }
void RemoteServiceApp::setFuelLevelLow(bool status) { mFuelLevelLow = status; }

bool RemoteServiceApp::getSVTactive() { return bSVTactive; }
void RemoteServiceApp::setSVTactive(bool status) { bSVTactive = status; }

bool RemoteServiceApp::getSubscriptSVT() { return bSubscriptSVT; }
void RemoteServiceApp::setSubscriptSVT(bool status) { bSubscriptSVT = status; }
bool const RemoteServiceApp::isSunroofExist() { return bSunroofExist; }
int8_t const RemoteServiceApp::getMovingWindows() { return mMovingWindows; }

bool RemoteServiceApp::getEngOBDMil() { return mEngOBDMil; }
void RemoteServiceApp::setEngOBDMil(bool status) { mEngOBDMil = status; }

int8_t RemoteServiceApp::getPrevParkClimateModeTCU() { return mPrevParkClimateModeTCU; }
void RemoteServiceApp::setPrevParkClimateModeTCU(int8_t mode) { mPrevParkClimateModeTCU = mode; }
int8_t RemoteServiceApp::getCurrParkClimateModeTCU() { return mCurrParkClimateModeTCU; }
int8_t RemoteServiceApp::getParkClimateModeWhenReqEngineHeating() { return mParkClimateModeWhenReqEngineHeating; }
void RemoteServiceApp::setParkClimateModeWhenReqEngineHeating(int8_t mode) { mParkClimateModeWhenReqEngineHeating = mode; }

uint8_t RemoteServiceApp::getCurrParkClimateReqTCU() { return mCurrParkClimateReqTCU; }
void RemoteServiceApp::setCurrParkClimateReqTCU(uint8_t reqMode) { mCurrParkClimateReqTCU = reqMode; }

uint8_t RemoteServiceApp::getParkHeatVentTime() { return mParkHeatVentTime; }
void RemoteServiceApp::setParkHeatVentTime(uint8_t minutes) { mParkHeatVentTime = minutes; }

bool RemoteServiceApp::getRPCtimerActiveStatus() { return mRPCtimerActiveStatus; }
void RemoteServiceApp::setRPCtimerActiveStatus(bool status) { mRPCtimerActiveStatus = status; }

uint8_t RemoteServiceApp::getFFHOperatingStatus() { return mFFHOperatingStatus; }
void RemoteServiceApp::setFFHOperatingStatus(uint8_t status) { mFFHOperatingStatus = status; }

uint32_t RemoteServiceApp::getTimer1Time() { return mTimer1Time; }
uint32_t RemoteServiceApp::getTimer1Month() { return mTimer1Month; }
uint32_t RemoteServiceApp::getTimer1Day() { return mTimer1Day; }
uint32_t RemoteServiceApp::getTimer1Hour() { return mTimer1Hour; }
uint32_t RemoteServiceApp::getTimer1Minute() { return mTimer1Minute; }

uint32_t RemoteServiceApp::getTimer2Time() { return mTimer2Time; }
uint32_t RemoteServiceApp::getTimer2Month() { return mTimer2Month; }
uint32_t RemoteServiceApp::getTimer2Day() { return mTimer2Day; }
uint32_t RemoteServiceApp::getTimer2Hour() { return mTimer2Hour; }
uint32_t RemoteServiceApp::getTimer2Minute() { return mTimer2Minute; }
bool RemoteServiceApp::getFrontSystemOnCmd() { return mFrontSystemOnCmd; }
void RemoteServiceApp::setFrontSystemOnCmd(bool status) { mFrontSystemOnCmd = status; }
void RemoteServiceApp::setHFSCommand(bool status) { mHFSCommand = status; }
void RemoteServiceApp::setHRWCommand(bool status) { mHRWCommand = status; }
void RemoteServiceApp::setHeatedSeatFLModeReq(uint8_t status) { mHeatedSeatFLModeReq = status; }
void RemoteServiceApp::setHeatedSeatFLRequest(uint8_t status) { mHeatedSeatFLRequest = status; }
void RemoteServiceApp::setHeatedSeatFRModeReq(uint8_t status) { mHeatedSeatFRModeReq = status; }
void RemoteServiceApp::setHeatedSeatFRRequest(uint8_t status) { mHeatedSeatFRRequest = status; }
void RemoteServiceApp::setHeatedSeatRLModeReq(uint8_t status) { mHeatedSeatRLModeReq = status; }
void RemoteServiceApp::setHeatedSeatRLRequest(uint8_t status) { mHeatedSeatRLRequest = status; }
void RemoteServiceApp::setHeatedSeatRRModeReq(uint8_t status) { mHeatedSeatRRModeReq = status; }
void RemoteServiceApp::setHeatedSeatRRRequest(uint8_t status) { mHeatedSeatRRRequest = status; }

RS_AlarmMode RemoteServiceApp::getCurrAlarmStatus() { return mCurrAlarmStatus; }
void RemoteServiceApp::setCurrAlarmStatus(RS_AlarmMode alarmStatus) { mCurrAlarmStatus = alarmStatus; }

bool RemoteServiceApp::getAllDoorLockStatus() { return bAllDoorLockStatus; }
uint8_t RemoteServiceApp::getDoorStatus() { return mDoorStatus; }

uint8_t RemoteServiceApp::getSunroofOpenStatus() { return mSunroofOpenStatus; } // 0:FullyClosed 1:NotFullyClosed
void RemoteServiceApp::setSunroofOpenStatus(uint8_t status) { mSunroofOpenStatus = status; }
uint8_t RemoteServiceApp::getRoofPositionStatus() { return mRoofPositionStatus; }
void RemoteServiceApp::setRoofPositionStatus(uint8_t status) { mRoofPositionStatus = status; }

uint8_t RemoteServiceApp::getRESSequenceStatus() { return mRESSequenceStatus; }
void RemoteServiceApp::setRESSequenceStatus(uint8_t status) { mRESSequenceStatus = status; }
bool RemoteServiceApp::getRESWindowStatus() { return bRESWindowStatus; }
void RemoteServiceApp::setRESWindowStatus(bool status) { bRESWindowStatus = status; }
bool RemoteServiceApp::getRESVehicleLockStatus() { return bRESVehicleLockStatus; }
void RemoteServiceApp::setRESVehicleLockStatus(bool status) { bRESVehicleLockStatus = status; }
bool RemoteServiceApp::getRESAlarmStatus() { return bRESAlarmStatus; }
void RemoteServiceApp::setRESAlarmStatus(bool status) { bRESAlarmStatus = status; }
bool RemoteServiceApp::getRESHazardSwitchStatus() { return bRESHazardSwitchStatus; }
void RemoteServiceApp::setRESHazardSwitchStatus(bool status) { bRESHazardSwitchStatus = status; }
bool RemoteServiceApp::getRESBrakePedalStatus() { return bRESBrakePedalStatus; }
void RemoteServiceApp::setRESBrakePedalStatus(bool status) { bRESBrakePedalStatus = status; }
bool RemoteServiceApp::getRESTransmissionStatus() { return mRESTransmissionStatus; }
void RemoteServiceApp::setRESTransmissionStatus(bool status) { mRESTransmissionStatus = status; }
bool RemoteServiceApp::getRESVehStationaryStatus() { return mRESVehStationaryStatus; }
void RemoteServiceApp::setRESVehStationaryStatus(bool status) { mRESVehStationaryStatus = status; }
bool RemoteServiceApp::getRESEngineRunningStatus() { return mRESEngineRunningStatus; }
void RemoteServiceApp::setRESEngineRunningStatus(bool status) { mRESEngineRunningStatus = status; }

bool RemoteServiceApp::getRDUTsimLockState() { return bRDUTsimLockState; }
void RemoteServiceApp::setRDUTsimLockState(bool status) { bRDUTsimLockState = status; }

bool RemoteServiceApp::getRESTsimLockState() { return bRESTsimLockState; }
void RemoteServiceApp::setRESTsimLockState(bool status) { bRESTsimLockState = status; }

uint8_t RemoteServiceApp::getAppModeRISM() { return mAppModeRISM; }
void RemoteServiceApp::setAppModeRISM(uint8_t appModeRISM) { mAppModeRISM = appModeRISM; }
uint8_t RemoteServiceApp::getConvenienceFold() { return mConvenienceFold; }
void RemoteServiceApp::setConvenienceFold(uint8_t convenienceFold) { mConvenienceFold = convenienceFold; }
uint8_t RemoteServiceApp::getVehicleType() { return mVehicleType; }
void RemoteServiceApp::setVehicleType(uint8_t vehicleType) { mVehicleType = vehicleType; }
uint8_t RemoteServiceApp::getIntelligentSeatFold() { return mIntelligentSeatFold; }
void RemoteServiceApp::setIntelligentSeatFold(uint8_t intelligentSeatFold) { mIntelligentSeatFold = intelligentSeatFold; }

seatMovementCommand RemoteServiceApp::getSeatMovementCommand_2D() { return requestRISM.drvSeat2ndRowCmd; }
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_2P() { return requestRISM.pasSeat2ndRowCmd; }
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_3D() { return requestRISM.drvSeat3rdRowCmd; }
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_3P() { return requestRISM.pasSeat3rdRowCmd; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_1P() { return statusRISM.pasSeat1stRowState; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_2D() { return statusRISM.drvSeat2ndRowState; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_2P() { return statusRISM.pasSeat2ndRowState; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_3D() { return statusRISM.drvSeat3rdRowState; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_3P() { return statusRISM.pasSeat3rdRowState; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_1P() { return statusRISM.pasSeat1stRowInhib; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_2D() { return statusRISM.drvSeat2ndRowInhib; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_2P() { return statusRISM.pasSeat2ndRowInhib; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_3D() { return statusRISM.drvSeat3rdRowInhib; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_3P() { return statusRISM.pasSeat3rdRowInhib; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_2D() { return requestRISM.request2D; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_2P() { return requestRISM.request2P; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_3D() { return requestRISM.request3D; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_3P() { return requestRISM.request3P; }
void RemoteServiceApp::setSeatMovementRequest_2D(seatMovementCommand request) { requestRISM.request2D = request; }
void RemoteServiceApp::setSeatMovementRequest_2P(seatMovementCommand request) { requestRISM.request2P = request; }
void RemoteServiceApp::setSeatMovementRequest_3D(seatMovementCommand request) { requestRISM.request3D = request; }
void RemoteServiceApp::setSeatMovementRequest_3P(seatMovementCommand request) { requestRISM.request3P = request; }

// for L405 vehicle
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_2L() { return requestRISM.REM2ndRowLeftMoveReq; }
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_2C() { return requestRISM.REM2ndRowCntrMoveReq; }
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_2R() { return requestRISM.REM2ndRowRightMoveReq; }
seatMovementCommand RemoteServiceApp::getSeatMovementCommand_skiHatch() { return requestRISM.REMSkiHatchReq; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_2L() { return statusRISM.RLSeatMovementStat; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_2C() { return statusRISM.RCntrSeatMovementStat; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_2R() { return statusRISM.RRSeatMovementStat; }
seatMovementStatus RemoteServiceApp::getSeatMovementStatus_skiHatch() { return statusRISM.REMSkiHatchStat; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_2L() { return statusRISM.RLSeatInhib2ndRow; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_2C() { return statusRISM.RMSeatInhib2ndRow; }
seatMovementInhibition RemoteServiceApp::getSeatMovementInhibition_2R() { return statusRISM.RRSeatInhib2ndRow; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_2L() { return requestRISM.request2L; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_2C() { return requestRISM.request2C; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_2R() { return requestRISM.request2R; }
seatMovementCommand RemoteServiceApp::getSeatMovementRequest_skiHatch() { return requestRISM.requestSkiHatch; }
void RemoteServiceApp::setSeatMovementRequest_2L(seatMovementCommand request) { requestRISM.request2L = request; }
void RemoteServiceApp::setSeatMovementRequest_2C(seatMovementCommand request) { requestRISM.request2C = request; }
void RemoteServiceApp::setSeatMovementRequest_2R(seatMovementCommand request) { requestRISM.request2R = request; }
void RemoteServiceApp::setSeatMovementRequest_skiHatch(seatMovementCommand request) { requestRISM.requestSkiHatch = request; }

uint8_t RemoteServiceApp::getLogserviceType() { return mLogserviceType; }
uint8_t RemoteServiceApp::getLogTransistionFromREStoNormal() { return mLogTransistionFromREStoNormal; }
uint8_t RemoteServiceApp::getLogPrerequisiteCheckRequestStatus() { return mLogPrerequisiteCheckRequestStatus; }
void RemoteServiceApp::setLogPrerequisiteCheckRequestStatus(uint8_t status) { mLogPrerequisiteCheckRequestStatus = status; }
uint8_t RemoteServiceApp::getLogPowerModeWhenRequestReceived() { return mLogPowerModeWhenRequestReceived; }
void RemoteServiceApp::setLogPowerModeWhenRequestReceived(uint8_t status) { mLogPowerModeWhenRequestReceived = status; }
uint8_t RemoteServiceApp::getLogRESSequenceStatusPrevious() { return mLogRESSequenceStatusPrevious; }
void RemoteServiceApp::setLogRESSequenceStatusPrevious(uint8_t status) { mLogRESSequenceStatusPrevious = status; }
uint8_t RemoteServiceApp::getLogffhStatusWhenFFHStopped() { return mLogffhStatusWhenFFHStopped; }
uint8_t RemoteServiceApp::getLogfuelLevel() { return mLogfuelLevel; }
void RemoteServiceApp::setLogfuelLevel(uint8_t low) { mLogfuelLevel = low; }

