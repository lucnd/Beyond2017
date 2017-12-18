/**
* \file    RemoteService_log.h
* \brief     Declaration of RemoteService_log Class.
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       dongjin1.seo
* \date    2015.11.20
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#ifndef _REMOTE_SERVICE_LOG_H_
#define _REMOTE_SERVICE_LOG_H_

#include <sys/stat.h>
#include "RS_TYPE.h"
#include <utils/RefBase.h>
#include "application/Application.h"

#if 0
typedef enum {
    LOG_NO_SERVICE = 0,
    LOG_UNLOCK_VEHICLE,
    LOG_SECURE_VEHICLE,
    LOG_REDUCE_GUARD,
    LOG_ALARM_RESET,
    LOG_HONK_FLASH,
    LOG_ENGINE_START,
    LOG_ENGINE_STOP,
    LOG_HEATER_START,
    LOG_HEATER_STOP,
    LOG_NOT_AVAILABLE = 255
} LogServiceType;

enum SuccessStatus {
    false = 0,
    true = 1,
    notAvailable = 255
};

enum SimpleStatus {
    noIssueDetected = 0,
    cantProceed_vehicleInUse = 1,
    cantProceed_lowBatt = 2,
    cantProceed_polling = 3,
    CANnetworkIssue = 4,
    BCMauthFailed = 5,
    TSPcommunicationIssue = 6,
    oneOfTheApparatusAjar = 7,
    vehicleNotApproachUser = 8,
    failUnlockOneOrMoreDoor = 9,
    failLockOneOrMoreDoor = 10,
    failShutOneOrMoreWindow = 11,
    alarmNotTriggered = 12,
    alarmTriggered = 13,
    alarmReTriggered = 14,
    notAvailable = 255
};

enum HeaterStatus {
    off = 0,
    startingUp = 1,
    running = 2,
    shutDown = 3,
    lowVoltage = 4,
    lowFuel = 5,
    serviceRequired = 6,
    systemFailure = 7,
    maxTimeReached = 8,
    stoppedByUser = 9,
    notAvailable = 255
};

//CAN signal RESWindowStatus
enum WindowStatus {
    openOrUnknown = 0,
    allClosed = 1,
    notAvailable = 255
};

//CAN signal RESVehicleLockStatus
enum LockStatus {
    unlockOrUnknown = 0,
    vehicleLocked = 1,
    notAvailable = 255
};

//CAN signal RESAlarmStatus
enum AlarmStatus {
    openOrUnknown = 0,
    allClosed = 1,
    notAvailable = 255
};

//CAN signal RESHazardSwitchStatus
enum HazardStatus {
    pressedOrUnknown = 0,
    notPressed = 1,
    notAvailable = 255
};

//CAN signal RESBrakePedalStatus
enum BrakePedalStatus {
    pressedOrUnknown = 0,
    notPressed = 1,
    notAvailable = 255
};

//CAN signal RESVehStationaryStatus
enum StationaryStatus {
    notOrUnknown = 0,
    vehStationary = 1,
    notAvailable = 255
};

//CAN signal RESTransmissionStatus
enum TransmissionStatus {
    notOrUnknown = 0,
    transmissionPark = 1,
    notAvailable = 255
};

enum CurrCrashStatus {
    noCrashDetect = 0,
    crashDetect = 1,
    notAvailable = 255
};

enum PrevCrashStatus {
    noCrashDetect = 0,
    crashDetect = 1,
    notAvailable = 255
};

enum FuelLevel {
    normal = 0,
    low = 1,
    notAvailable = 255
};

enum MaxRunTime {
    false = 0,
    true = 1,
    notAvailable = 255
};

enum SvtStatus {
    SVTnotActive = 0,
    SVTactive = 1,
    notAvailable = 255
};

enum RESnormalTransition {
    false = 0,
    true = 1,
    notAvailable = 255
};

enum PrereqStatus {
    false = 0,
    true = 1,
    notAvailable = 255
};

enum PowerMode {
    keyOut = 0,
    keyRecentlyOut = 1,
    keyApproved = 2,
    postAccessory = 3,
    accessory = 4,
    postIgnition = 5,
    ignitionOn = 6,
    running = 7,
    crank = 9,
    notAvailable = 255
};

//CAN signal RESSequenceStatus
enum CurrRESseqStatus {

};

//CAN signal RESSequenceStatus
enum PrevRESseqStatus f{

};

//ResRunTimer uint16_t
//FfhRunTimer uint16_t

enum FfhStatus {
    off = 0,
    startingUp = 1,
    running = 2,
    shutDown = 3,
    lowVoltage = 4,
    lowFuel = 5,
    serviceRequired = 6,
    systemFailure = 7,
    maxTimeReached = 8,
    stoppedByUser = 9,
    notAvailable = 255
};
#endif

#define LOG_INDEX_MAX 256   // 0~255

typedef struct {
    uint8_t routineResult;
    uint32_t eventCreatTime;            //All
    uint8_t serviceType;                //All
    uint8_t successStatus;              //All
    uint8_t simpleStatus;               //All
    uint8_t heaterStatus;               //RES/RPC FFHOperatingStatus signal
    uint8_t windowStatus;               //RES FFH prerequisite  //1bit 0 open 1 close
    uint8_t lockStatus;                 //RES FFH prerequisite  //1bit 0 unlock 1 lock
    uint8_t alarmStatus;                //RES FFH prerequisite  //1bit 0 active 1 not active
    uint8_t hazardStatus;               //RES FFH prerequisite  //1bit 0 press 1 not press
    uint8_t brakePedalStatus;           //RES FFH prerequisite  //1bit 0 press 1 not press
    uint8_t stationaryStatus;           //RES    //1bit 0 not stationary  1 stainaryy
    uint8_t transmissionStatus;         //RES    //1bit 0 not in park     1 in park
    uint8_t currCrashStatus;            //RES
    uint8_t prevCrashStatus;            //RES
    uint8_t fuelLevel;                  //RES
    uint8_t maxRunTime;                 //RES
    uint8_t svtStatus;                  //RES
    uint8_t transitionNormal;           //RES
    uint8_t prerequisiteStatus;         //All
    uint8_t powerMode;                  //All
    uint8_t currResSequence;            //RES
    uint8_t prevResSequence;            //RES
    uint16_t resRunTimer;               //RES
    uint16_t ffhRunTimer;               //RES/RPC
    uint8_t ffhStatusWhenFFHStopped;    //RES/RPC FFHOperatingStatus signal when ffh stopped
}__attribute__((__packed__)) tf2_tcu_logging;



class RemoteService_log
{
public:
    RemoteService_log();
    virtual ~RemoteService_log() {}

    void initLogData(uint8_t index = 0);
//    void  setLoggingData();
    sp<Buffer> getLoggingData(uint8_t index = 0);

    bool checkLogDataPath();
    bool readLogfromFile();
    bool writeLogToFile();
    uint8_t findLogIndex();

    uint8_t getCurrentLogIndex();
    void setLogIndex(uint8_t index);
    uint8_t increaseLogIndex();

    void set_eventCreateTime(uint32_t eventCreateTime, uint8_t index = 0);
    void set_serviceType(uint8_t serviceType, uint8_t index = 0);
    void set_successStatus(uint8_t successStatus, uint8_t index = 0);
    void set_simpleStatus(uint8_t simpleStatus, uint8_t index = 0);
    void set_heaterStatus(uint8_t heaterStatus, uint8_t index = 0);
    void set_windowStatus(uint8_t windowStatus, uint8_t index = 0);
    void set_lockStatus(uint8_t lockStatus, uint8_t index = 0);
    void set_alarmStatus(uint8_t alarmStatus, uint8_t index = 0);
    void set_hazardStatus(uint8_t hazardStatus, uint8_t index = 0);
    void set_brakePedalStatus(uint8_t brakePedalStatus, uint8_t index = 0);
    void set_stationaryStatus(uint8_t stationaryStatus, uint8_t index = 0);
    void set_transmissionStatus(uint8_t transmissionStatus, uint8_t index = 0);
    void set_currCrashStatus(uint8_t currCrashStatus, uint8_t index = 0);
    void set_prevCrashStatus(uint8_t prevCrashStatus, uint8_t index = 0);
    void set_fuelLevel(uint8_t fuelLevel, uint8_t index = 0);
    void set_maxRunTime(uint8_t maxRunTime, uint8_t index = 0);
    void set_svtStatus(uint8_t svtStatus, uint8_t index = 0);
    void set_transitionNormal(uint8_t transitionNormal, uint8_t index = 0);
    void set_prerequisiteStatus(uint8_t prerequisiteStatus, uint8_t index = 0);
    void set_powerMode(uint8_t powerMode, uint8_t index = 0);
    void set_currResSequence(uint8_t currResSequence, uint8_t index = 0);
    void set_prevResSequence(uint8_t prevResSequence, uint8_t index = 0);
    void set_resRunTimer(uint16_t resRunTimer, uint8_t index = 0);
    void set_ffhRunTimer(uint16_t ffhRunTimer, uint8_t index = 0);
    void set_ffhStatusWhenFFHStopped(uint8_t ffhStatus, uint8_t index = 0);
    void set_routineResult(uint8_t routineResult, uint8_t index = 0);

private:
    tf2_tcu_logging logData[LOG_INDEX_MAX];
    uint8_t mLogIndex;
    FILE* logFile;
};
#endif /**_REMOTE_SERVICE_LOG_H_*/
