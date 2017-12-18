/**
* \file    RS_TYPE.h
* \brief     Define RemoteService enumeration.
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

#ifndef _REMOTE_SERVICE_TYPE_H_
#define _REMOTE_SERVICE_TYPE_H_

#include "Typedef.h"
#include "services/NGTPManagerService/asn2cpp-dspt/ngtp-dspt.h"
#include "services/NGTPManagerService/asn2cpp-svc/tsdp-svc.h"

#define COMMON_CONFIG_FILE "jlr_tcu_data_dictionary_swr_9_3.xml"

#define SIZE_1BYTE   0x01
#define SIZE_2BYTE   0x02
#define SIZE_3BYTE   0x03
#define SIZE_4BYTE   0x04

#define RES_NORMAL_MODE 0
#define RES_REMOTE_MODE 1

#define STATUS_ENABLE   1
#define STATUS_DISABLE  0

#define TRUE  1
#define FALSE 0

#define STR_TRUE          "TRUE"
#define STR_FALSE         "FALSE"
#define STR_SUCCESS       "SUCCESS"
#define STR_FAIL          "FAIL"
#define STR_OK            "OK"
#define STR_PRE_FAIL      "Prerequisite Fail"
#define STR_TIMEOUT       "TIME OUT"
#define STR_NOT_AVAILABLE "not available"
#define STR_NOT_IMPL      "not implemented"
#define STR_BCM_FAIL      "BCM auth Fail"
#define STR_ALOFF_NA      "Alarm not activated"
#define STR_PREV_CRASH    "prevCrashStatus"
#define STR_CURR_CRASH    "currCrashStatus"
//#define STR_FUEL_LEVEL    "fuelLevel"   // -> PROP_FUEL_LEVEL_INDICATED_HS
//#define STR_RES_RUNTIME   "RESruntime"
#define STR_NGTP_ACK_RETRY "NGTPUDPMaxRetryForAck"
#define STR_NGTP_ACK_DELAY "NGTPUDPDelayForAckMs"
#define STR_NGTP_UDP_RETRY "NGTPUDPMaxRetry"
#define STR_NGTP_UDP_DELAY "NGTPUDPDelay"

#define DIST_EMPTY_MAX    0x3FF
#define ODOMETER_MASK     0xFFFFFF
#define INIT_VALUE_ZERO   0
#define INIT_VALUE_ONE    1
#define FUEL_POS_MAX      0x14
#define FUEL_POS_DEFAULT  0x10
#define FUEL_POS_INTERVAL 5
#define ODOMETER_MAX      0x20C49B
#define ODO_DISPLAY_MILES_MAX   0xFFFFFE
#define ALL_DOOR_CLOSED   0x3F
#define INIT_DISTANCE_EMPTY  500

#define RVI_DOORLATCH_ALLLOCKED         0x00
#define RVI_DOORLATCH_ALLUNLOCKED       0x1F
#define RVI_DOORLATCH_DRIVERUNLOCKED    0x01

#define RVI_ALARM_ACTIVATED 0x02

#define RES_SVT_ALERT_STATUS   2
#define RES_SVT_ALARM_STATUS   3

// timer period
#define RES_ENG_START_TIME                      90  // id = 1 : TIMER_RES_START
#define RES_STOP_REQ_RESP_TIME                  2300 //2.3sec // id = 2 : TIMER_RES_STOP
#define RES_PREREQ_RESPONSE_TIME                1   // id = 3 : TIMER_RES_PREREQ
#define RES_FFH_RUN_TIME_MAX                    1800// id = 3 : TIMER_FFH_MAX_RUN
#define PARK_CLIMATE_STARTUP_TIME               35  // id = 5 : TIMER_CLIMATE_REQ
#define RES_FFH_STOPPEDBY_USER_DETECTION_TIME   10  // id = 6 : TIMER_FFH_USER_STOP
#define DOOR_UNLOCK_TIMEOUT                     2   // id = 7 : TIMER_UNLOCK_VEHICLE
#define REMOTE_VEHICLE_SECURE_TIMEOUT           60  // id = 8 : TIMER_SECURE_VEHICLE
#define REMOTE_VEHICLE_ALARM_RESET_TIMEOUT      30  // id = 9 : TIMER_ALARM_RESET
#define PARKHEATVENTTIME_SAMPLE_WAIT_TIME       4100//4.1 sec // id = 14 : TIMER_PARKHEATVENT_WAIT
#define MIL_FAULT_DETECTION_TIME                12  // id = 19 : TIMER_MIL_FAULT_DETECT
#define PARK_CLIMATE_SHUTDOWN_TIME              5   // id = 22 : TIMER_PARK_CLIMATE_SHUTDOWN
#define PARK_CLIMATE_MIN_STATUS_WAIT_TIME       6000//ms   // id = 24 : TIMER_PARK_CLIMATE_MIN_STATUS_WAIT
#define RVC_BCM_AUTHTIME                        4   //seconds
#define MAX_KEEPPOWER_TIME                      240 //seconds
#define BCM_RESP_WAIT_TIME                      1000000 //uSeconds
#define BCM_RESP_WAIT_MAX_TIME                  5000000 //uSeconds

// configration value
#define RCC_TARGET_TEMP             "ClimateControlRccTargetTemp"
#define RES_FUEL_LEVEL_MIN          "ClimateControlResFuelLevelMin_dl"   //7 Litres  ????
#define RES_FUEL_LEVEL_START_MIN    "ClimateControlResFuelLevelStartMin_dl"  //8.67 Litres
#define CONFIG_BODY_CONTROLLER      "bodyController"
#define CONFIG_MOVING_WINDOWS       "movingWindows"
//#define RPC_STARTUP_TIME            "ClimateControlParkClimateStartupTime_s"  //35
//#define RPC_SHUTDOWN_TIME           "ClimateControlParkClimateShutdownTime_s" //5
#define CCF_FFH_PRESENT             "additionalHeater"
#define RES_MIN_REMAINING_TIME      "ClimateControlResRunTimeMinLimit_s" // 3 min
#define RES_ENGINE_RUN_MAX          "ClimateControlResRunTime_s"  //1800 //initial value from CCF
#define RVC_RES_RUNMAX              "RvcResRunTime_s" //remaining res time, internal value to save actual remaining time
#define APP_MODE_SVT                "AppModeSVT"
#define APP_MODE_RVC                "AppModeRVC"
#define APP_MODE_RISM               "AppModeRISM"
#define CONFIG_CONVENIENCE_FOLD     "convenienceFold"
#define CONFIG_VEHICLE_TYPE         "vehicleType"
#define CONFIG_INTELLIGENT_SEAT_FOLD    "intelligentSeatFold"

#define CONFIG_RES_ENG_START_TIME                       "ClimateControlResEngineStartTimer_s"
#define CONFIG_RES_STOP_REQ_RESP_TIME                   "ClimateControlResStopRespTime_ms"
#define CONFIG_RES_PREREQ_RESPONSE_TIME                 "ClimateControlResPreReqRespTimer_ms"
#define CONFIG_RES_FFH_RUN_TIME_MAX                     "ClimateControlResFfhRunTimeMax_s"
#define CONFIG_RES_FFH_STOPPEDBY_USER_DETECTION_TIME    "ClimateControlResUserInterruptionTimer_s"
#define CONFIG_PARK_CLIMATE_STARTUP_TIME                "ClimateControlParkClimateStartupTime_s"
#define CONFIG_DOOR_UNLOCK_TIMEOUT                      "RvcUnlockExecutionTimer"
#define CONFIG_REMOTE_VEHICLE_SECURE_TIMEOUT            "RvcLockExecutionTimer"
#define CONFIG_REMOTE_VEHICLE_ALARM_RESET_TIMEOUT       "RvcAlarmResetExecutionTimer"
#define CONFIG_PARK_CLIMATE_MIN_STATUS_WAIT_TIME        "ClimateControlCanReadDelay_ms"
#define CONFIG_PARKHEATVENTTIME_SAMPLE_WAIT_TIME        "ClimateControlParkClimateShutdownTime_s"

#define REMOTE_VEHICLE_SECURE_PSIM_LOCK_TIMER   "REMOTE_VEHICLE_SECURE_PSIM_LOCK_TIMER"

// some signal values need time from pm became > 6 to warmup
// so, save valid signal values to use while the value become valid
#define PROP_FUEL_LEVEL_INDICATED_HS    "FuelLevelIndicatedHS"
#define PROP_FUEL_LEVEL_INDICATED_QFHS  "FuelLevelIndicatedQFHS"
#define PROP_FUEL_GAUGE_POSITION        "FuelGaugePosition"
#define PROP_ODEMETER_MASTER_VALUE      "OdometerMasterValue"
#define PROP_ODO_DISPLAY_MILES          "ODODisplayedMiles"
#define PROP_DISTANCE_TO_EMPTY          "DistanceToEmpty"
#define PROP_RH_REQUEST                 "RHRequest"
#define PROP_RH_PREREQUSITE             "RHPrerequsite"
#define PROP_RH_PARKCLIMATEMODE         "RHParkClimateMode"
#define PROP_CAN_VARIANT                "canVariant"
#define PROP_RPC_VARIABLES              "RPCvaliables"
#define PROP_TIMER_1_TIME               "timer1time"
#define PROP_TIMER_2_TIME               "timer2time"
#define PROP_SEATS                      "seatStatus"

#define SLDD_RISM_START                     0xFF50
#define SLDD_REQ_FOLD                       0xFF52
#define SLDD_REQ_UNFOLD                     0xFF53
#define SLDD_RISM_END                       0xFF54

#define RES_FUEL_MASK   0x3FF
#define RES_FUEL_QF     3
#define FFH_SYS_PRESENT 0x03

#define REMOTE_FEATURE_REQUEST        0x01
#define TCUDATACONTROL_ALARM_RESET    0x03
#define TCUDATACONTROL_HONK_FLASH     0x0F
#define TCUDATACONTROL_BCM_PREREQ     0x30
#define TCUDATACONTROL_ENGINE_STOP    0x33
#define TCUDATACONTROL_ENGINE_START   0x56
#define TCUDATACONTROL_UNLOCK_VEHICLE 0x68
#define TCUDATACONTROL_SECURE_VEHICLE 0x6F
#define TCUDATACONTROL_INIT_SEATMOVE  0x73
#define TCUDATACONTROL_COMP_SEATMOVE  0x78

#define BCM_AUTH_OK        0x01
#define BCM_AUTH_NOK       0x02
#define BCM_AUTH_BLOCK     0x03
#define BCM_AUTH_CH_ERROR  0x04
#define BCM_AUTH_TIME_OUT  0x05

#define DBG_CMD_TYPE_REQUEST      0x01
#define DBG_CMD_ALARM_RESET       0x41
#define DBG_CMD_VEHICLE_UNLOCK    0x42
#define DBG_CMD_VEHICLE_SECURE    0x43
#define DBG_CMD_VEHICLE_LOCATION  0x44

// temporary for v0.98 DBG cmd response
#define DBG_RESP_SUCCESS        0x01
#define DBG_RESP_FAIL           0x02
#define DBG_PRECOND_FAIL        0x03
#define DBG_BCM_AUTH_FAIL       0x04
#define DBG_BCM_AUTH_SUCCESS    0x05

// ParkClimateReqTCU signal values
#define RPC_REQ_PARK_DEFAULT    0x00
#define RPC_REQ_PARK_VENT       0x01
#define RPC_REQ_PARK_HEAT       0x02
#define RPC_REQ_PARK_AUTO       0x03
#define RPC_REQ_PARK_STOP       0x04
#define RES_FFH_STOP_REQUEST    0x04
#define RES_ENGINE_HEAT_REQUEST 0x05

#define RCC_MAX_TEMP     57
#define RCC_MIN_TEMP     31
#define RCC_USER_TEMP    63
#define RCC_DEFAULT_TEMP 44
#define CONFIG_NAME_VAL   2

#define RVI_DOOR_UNLOCKED  1
#define RVI_DOOR_LOCKED    2

#define RISM_FITTED  0x02
#define RISM_SEAT_READY_TIME  "RISM_SEAT_READINESS_TIMER"
#define RISM_SEAT_MOVE_TIME   "RISM_SEAT_MOVEMENT_TIMER"
#define SEAT_NOT_INHIBITED  0
#define VEHICLETYPE_L462    0x1C
#define VEHICLETYPE_L405    0xB
#define SEATTYPE_6040       0x2
#define SEATTYPE_4040       0x3
#define SEATTYPE_402040     0x4

#define RISM_REQ_FOLD       1
#define RISM_REQ_UNFOLD     2
#define RISM_REQ_DEPLOY     1
#define RISM_REQ_STOW       2
#define RISM_REQ_DROP       1

#define RISM_REQ_ALL_SEAT   0x0F
#define RISM_PROP_DRV2ND    "drvSeat2ndRow"
#define RISM_PROP_PAS2ND    "pasSeat2ndRow"
#define RISM_PROP_DRV3RD    "drvSeat3rdRow"
#define RISM_PROP_PAS3RD    "pasSeat3rdRow"

#define RISM_PROP_DRV2ND_IHB   "drvSeat2ndInhib"
#define RISM_PROP_PAS2ND_IHB   "pasSeat2ndInhib"
#define RISM_PROP_DRV3RD_IHB   "drvSeat3rdInhib"
#define RISM_PROP_PAS3RD_IHB   "pasSeat3rdInhib"

#define RVC_SLDD_COMMAND    0x0000F3ED

#define NGTP_ST_RDU     19
#define NGTP_ST_RDL     20
#define NGTP_ST_HBLF    21
#define NGTP_ST_REON    22
#define NGTP_ST_REOFF   23
#define NGTP_ST_ALOFF   37
#define NGTP_ST_RHON    39
#define NGTP_ST_RHOFF   40
#define NGTP_ST_SCHEDULEDWAKUP    134
#define NGTP_ST_RISM    135

#define RVC_NGTP_RETRY_CNT   5
#define RVC_NGTP_RETRY_DELAY 800
#define RVC_NGTP_UDP_RETRY_CNT   5
#define RVC_NGTP_UDP_RETRY_DELAY 10000

//sldd request post
#define SLDD_REQ_MASK       0xFF
#define SLDD_REQ_RDU        0xF100
#define SLDD_REQ_RDL        0xF101
#define SLDD_REQ_ALOFF      0xF102
#define SLDD_REQ_HBLF       0xF103
#define SLDD_REQ_REON       0xF104
#define SLDD_REQ_REOFF      0xF105
#define SLDD_REQ_RH_AUTO    0xF106
#define SLDD_REQ_RH_STOP    0xF107
#define SLDD_REQ_RH_VENT    0xF108
#define SLDD_REQ_RH_HEAT    0xF109

#define SLDD_RESET_RDU      0xF300
#define SLDD_RESET_RDL      0xF301
#define SLDD_RESET_ALOFF    0xF302
#define SLDD_RESET_HBLF     0xF303
#define SLDD_RESET_REON     0xF304
#define SLDD_RESET_REOFF    0xF305
#define SLDD_RESET_RH_AUTO  0xF306
#define SLDD_RESET_RH_STOP  0xF307
#define SLDD_RESET_RH_VENT  0xF308
#define SLDD_RESET_RH_HEAT  0xF309
#define SLDD_RESET_RH       0xF30A

#define SLDD_RESET_RVC  0xF30B

#define SLDD_REQ_MAX    6
#define SLDD_PROP_MAX   9

//for property
#define SLDD_PROP_PWR     0
#define SLDD_PROP_RES     1
#define SLDD_PROP_AUTH    2
#define SLDD_PROP_SUCCESS 3
#define SLDD_PROP_CAUSE   4
#define SLDD_PROP_SVT     5
#define SLDD_PROP_FUEL    6
#define SLDD_PROP_CRASH   7
#define SLDD_PROP_RUNTIME 8

#define LOG_NOT_AVAILABLE  0xFF
#define DIAG_RESP_SIZE   34 //33  2016.0516. added Routine info.(1byte)
#define DIAG_PARAM_SIZE  31


typedef enum {
    RS_NOT_READY = 0,
    RS_READY,
    RS_ACTIVE,
    RS_WAIT_RESPONSE,
    RS_SERVICE_STATE_MAX
} RS_ServiceState;

typedef enum {
/*00*/  RS_UNLOCK_VEHICLE = 0,
/*01*/  RS_SECURE_VEHICLE,
/*02*/  RS_ALARM_RESET,
/*03*/  RS_HONK_FLASH,
/*04*/  RS_ENGINE_START,
/*05*/  RS_ENGINE_STOP,
/*06*/  RS_PARK_CLIMATE_AUTO,
/*07*/  RS_PARK_CLIMATE_STOP,
/*08*/  RS_PARK_CLIMATE_VENT,
/*09*/  RS_PARK_CLIMATE_HEAT,
/*10*/  RS_FFH_PREREQ,
/*11*/  RS_FFH_START,
/*12*/  RS_FFH_STOP,
/*13*/  RS_TARGET_TEMP,
/*14*/  RS_SEAT_MOVE_INIT,
/*15*/  RS_RISM_MOVE,
/*16*/  RS_RISM_CANCEL,
/*17*/  RS_SEAT_MOVE_COMPLETE,
/*18*/  RS_SCHEDULED_WAKEUP,

/*19*/  RS_SERVICE_TYPE_MAX
} RS_ServiceType;

typedef enum {
    RS_RES,
    RS_RCC,
    RS_RPC,
    RS_RVI,
    RS_RISM,
    RS_SERVICE_MAX
} RS_RemoteService;

typedef enum {
    PM_KEY_OUT = 0,
    PM_KEY_RECENTLY_OUT,
    PM_KEY_APPROVED_0,
    PM_POST_ACCESSORY_0,
    PM_ACCESSORY_1,
    PM_POST_IGNITION_1,
    PM_IGNITION_ON_2,
    PM_RUNNING_2,
    PM_NO_USED,
    PM_CRANK_3,
    PM_MAX
} RS_PowerMode;

typedef enum {
    PS_UNKNOWN = 0,
    PS_UNPOWERED,
    PS_NORMAL,
    PS_LISTEN,
    PS_SLEEP,
    PS_BACKUPBATTERY,
    PS_MAX
} RS_PowerState;

typedef enum {
    ALARM_SET = 0,
    ALARM_UNSET,
    ALARM_ACTIVE,
    ALARM_IGNORE
} RS_AlarmMode;

typedef enum {
    DRV_DOOR_LOCK,
    DRV_DOOR_UNLOCK,
    PAS_DOOR_LOCK,
    PAS_DOOR_UNLOCK,
    LREAR_DOOR_LOCK,
    LREAR_DOOR_UNLOCK,
    RREAR_DOOR_LOCK,
    RREAR_DOOR_UNLOCK,
    TRUNK_LOCK,
    TRUNK_UNLOCK
} RS_DoorLatchStatus;

typedef enum {
    POS_UNDEFINED = 0,
    POS_FULLY_CLOSED,
    POS_10_60_OPEN,
    POS_60_FULLY_OPEN,
    POS_FULLY_OPEN
} RS_WindowStatus;

// must be the same whit RVM RS_ParkClimateMode. Refer Typedef_rvm.h
typedef enum {
    CLIMATE_MODE_OFF = 0,
    CLIMATE_MODE_STARTUP,
    CLIMATE_ENGINE_HEATING,
    CLIMATE_CABIN_HEATING,
    CLIMATE_CABIN_VENTING
} RS_ParkClimateMode;

// must be the same whit RVM RS_FFHOperatingStatus. Refer Typedef_rvm.h
typedef enum {
    FFH_OFF                 = 0,
    FFH_STARTINGUP          = 1,
    FFH_RUNNING             = 2,
    FFH_SHUTDOWN            = 3,
    FFH_FAIL_LOW_VOLTAGE    = 4,
    FFH_FAIL_LOW_FUEL       = 5,
    FFH_FAIL_SVC_REQ        = 6,
    FFH_FAIL_SYS_FAILURE    = 7,
    FFH_FAIL_MAX_TIME       = 8,
    FFH_STOP_BY_USER        = 9,
    FFH_RESERVED            = 10,
    FFH_STATUS_UNKNOWN      = 12
} RS_FFHOperatingStatus;

typedef enum {
    PROP_INVALID = -1,
    PROP_PWR = 0,
    PROP_RES,
    PROP_AUTH,
    PROP_SUCCESS,
    PROP_CAUSE,
    PROP_SVT,
    PROP_FUEL,
    PROP_CRASH,
    PROP_RUNTIME,
} RVCproperty;

typedef enum {
    RES_STOP_TSP = 0,
    RES_STOP_RUNTIME,
    RES_STOP_FUELLOW,
    RES_STOP_SLDD,
    RES_STOP_MAX
} RES_StopReason;

typedef enum {
    unselected                              = svc::ErrorCode::unselected                             ,
    incorrectSequence_chosen                = svc::ErrorCode::incorrectSequence_chosen               ,
    incorrectEct_chosen                     = svc::ErrorCode::incorrectEct_chosen                    ,
    incorrectState_chosen                   = svc::ErrorCode::incorrectState_chosen                  ,
    notAllowed_chosen                       = svc::ErrorCode::notAllowed_chosen                      ,
    invalidData_chosen                      = svc::ErrorCode::invalidData_chosen                     ,
    invalidFormat_chosen                    = svc::ErrorCode::invalidFormat_chosen                   ,
    outOfMemory_chosen                      = svc::ErrorCode::outOfMemory_chosen                     ,
    noOngoingService_chosen                 = svc::ErrorCode::noOngoingService_chosen                ,
    vehiclePowerModeNotCorrect_chosen       = svc::ErrorCode::vehiclePowerModeNotCorrect_chosen      ,
    conditionsNotCorrect_chosen             = svc::ErrorCode::conditionsNotCorrect_chosen            ,
    executionFailure_chosen                 = svc::ErrorCode::executionFailure_chosen                ,
    fuelLevelTooLow_chosen                  = svc::ErrorCode::fuelLevelTooLow_chosen                 ,
    batteryLevelTooLow_chosen               = svc::ErrorCode::batteryLevelTooLow_chosen              ,
    runTimeExpiry_chosen                    = svc::ErrorCode::runTimeExpiry_chosen                   ,
    theftEvent_chosen                       = svc::ErrorCode::theftEvent_chosen                      ,
    crashEvent_chosen                       = svc::ErrorCode::crashEvent_chosen                      ,
    timeout_chosen                          = svc::ErrorCode::timeout_chosen                         ,
    windowNotClosed_chosen                  = svc::ErrorCode::windowNotClosed_chosen                 ,
    hazardWarningOn_chosen                  = svc::ErrorCode::hazardWarningOn_chosen                 ,
    vehicleNotLocked_chosen                 = svc::ErrorCode::vehicleNotLocked_chosen                ,
    alarmActive_chosen                      = svc::ErrorCode::alarmActive_chosen                     ,
    brakePedalPressed_chosen                = svc::ErrorCode::brakePedalPressed_chosen               ,
    vehicleNotStationary_chosen             = svc::ErrorCode::vehicleNotStationary_chosen            ,
    transmissionNotInPark_chosen            = svc::ErrorCode::transmissionNotInPark_chosen           ,
    climateModeNotCorrect_chosen            = svc::ErrorCode::climateModeNotCorrect_chosen           ,
    executedTooManyTimes_chosen             = svc::ErrorCode::executedTooManyTimes_chosen            ,
    startTimeInThePast_chosen               = svc::ErrorCode::startTimeInThePast_chosen              ,
    startTimeTooFarIntoTheFuture_chosen     = svc::ErrorCode::startTimeTooFarIntoTheFuture_chosen    ,
    conflictWithOnboardChange_chosen        = svc::ErrorCode::conflictWithOnboardChange_chosen       ,
    parameterAlreadyInRequestedState_chosen = svc::ErrorCode::parameterAlreadyInRequestedState_chosen,
    parameterOutOfRange_chosen              = svc::ErrorCode::parameterOutOfRange_chosen             ,
    serviceAlreadyRunning_chosen            = svc::ErrorCode::serviceAlreadyRunning_chosen           ,
    hvacSystemFailure_chosen                = svc::ErrorCode::hvacSystemFailure_chosen               ,
    hvacTimeout_chosen                      = svc::ErrorCode::hvacTimeout_chosen                     ,

// from NGTP 3.14
	startTimeGreaterThanStopTime_chosen     = svc::ErrorCode::startTimeGreaterThanStopTime_chosen    ,
	maxJobLimitReached_chosen               = svc::ErrorCode::maxJobLimitReached_chosen              ,
	stopTimeInThePast_chosen                = svc::ErrorCode::stopTimeInThePast_chosen               ,
	jobAlreadyOngoing_chosen                = svc::ErrorCode::jobAlreadyOngoing_chosen               ,
	jobCancelled_chosen                     = svc::ErrorCode::jobCancelled_chosen                    ,
	alarmNotSet_chosen                      = svc::ErrorCode::alarmNotSet_chosen                     ,
	ecuIdNotSupported_chosen                = svc::ErrorCode::ecuIdNotSupported_chosen               ,
	clientNotActive_chosen                  = svc::ErrorCode::clientNotActive_chosen                 ,
	serviceNotSupported_chosen              = svc::ErrorCode::serviceNotSupported_chosen             ,
	resRequired_chosen                      = svc::ErrorCode::resRequired_chosen                     ,
	chargeCableConnected_chosen             = svc::ErrorCode::chargeCableConnected_chosen            ,

    genericError_chosen                     = svc::ErrorCode::genericError_chosen
} NGTP_ErrorCode;

typedef enum  {
    seatMovementCommand_none    = svc::SeatMovementCommand_none ,
    seatMovementCommand_fold    = svc::fold                     ,
    seatMovementCommand_unfold  = svc::unfold                   ,

    seatMovementCommand_stow    = svc::fold                     ,
    seatMovementCommand_deploy  = svc::unfold                   ,

    seatMovementCommand_drop    = svc::unfold                   ,
} seatMovementCommand;

typedef enum  {
    seatMovementInhibition_available, // Available to receive a seat movement request
    seatMovementInhibition_inhibited, // Seat movement is Inhibited
} seatMovementInhibition;

typedef enum  {
    seatMovementStatus_default              = 0,
    seatMovementStatus_seatFullyFolded          ,
    seatMovementStatus_seatFullyUnfolded        ,
    seatMovementStatus_seatMidTravelStationary  ,
    seatMovementStatus_seatMoving               ,
    seatMovementStatus_oneTouchReclined     = 5,    // added for L405
    seatMovementStatus_fullyReclined            ,
    seatMovementStatus_returnedToHome           ,

    // arm rest.. Signal_RCntrSeatMovmentStat_RX
    seatMovementStatus_deployedDownPosition = 5,
    seatMovementStatus_stowedUpPosition     = 6,

    // ski hatch.. Signal_SkiHatchStat_RX
    seatMovementStatus_closed               = 0,
    seatMovementStatus_open                 = 1,
} seatMovementStatus;

typedef struct RISM_request
{
    int8_t requestedType;  // initValue(-1) svc::startRequest(2) or svc::terminateReq(3)

    // for L462 vehicle
    seatMovementCommand pasSeat1stRowCmd;   //currently not used
    seatMovementCommand drvSeat2ndRowCmd;
    seatMovementCommand pasSeat2ndRowCmd;
    seatMovementCommand drvSeat3rdRowCmd;
    seatMovementCommand pasSeat3rdRowCmd;

    // for L405 vehicle
    seatMovementCommand REM2ndRowLeftMoveReq;
    seatMovementCommand REM2ndRowCntrMoveReq;
    seatMovementCommand REM2ndRowRightMoveReq;
    seatMovementCommand REMSkiHatchReq;

    seatMovementCommand request2D;   //request sent from TCU to vehicle
    seatMovementCommand request2P;   //request sent from TCU to vehicle
    seatMovementCommand request3D;   //request sent from TCU to vehicle
    seatMovementCommand request3P;   //request sent from TCU to vehicle

    seatMovementCommand request2L;   //request sent from TCU to vehicle
    seatMovementCommand request2C;   //request sent from TCU to vehicle
    seatMovementCommand request2R;   //request sent from TCU to vehicle
    seatMovementCommand requestSkiHatch;   //request sent from TCU to vehicle

} RISM_request;

typedef struct RISM_status
{
    // for L462 vehicle
    seatMovementStatus pasSeat1stRowState;   //currently not used
    seatMovementStatus drvSeat2ndRowState;
    seatMovementStatus pasSeat2ndRowState;
    seatMovementStatus drvSeat3rdRowState;
    seatMovementStatus pasSeat3rdRowState;
    seatMovementInhibition pasSeat1stRowInhib;   //currently not used
    seatMovementInhibition drvSeat2ndRowInhib;
    seatMovementInhibition pasSeat2ndRowInhib;
    seatMovementInhibition drvSeat3rdRowInhib;
    seatMovementInhibition pasSeat3rdRowInhib;

    // for L405 vehicle
    seatMovementStatus RLSeatMovementStat;
    seatMovementStatus RCntrSeatMovementStat;
    seatMovementStatus RRSeatMovementStat;
    seatMovementStatus REMSkiHatchStat;
    seatMovementInhibition RLSeatInhib2ndRow;
    seatMovementInhibition RMSeatInhib2ndRow;
    seatMovementInhibition RRSeatInhib2ndRow;
} RISM_status;

typedef struct climate_parameters
{
    svc::BOOLEXT timerActivationStatus;
    uint32_t timer1month;
    uint32_t timer1day;
    uint32_t timer1hour;
    uint32_t timer1minute;
    uint32_t timer2month;
    uint32_t timer2day;
    uint32_t timer2hour;
    uint32_t timer2minute;
    svc::ClimateOperatingStatus climateOperatingStatus;
    uint32_t remainingRunTime;
    uint32_t ventingTime;
    svc::BOOLEXT heatedFrontScreenActive;
    svc::BOOLEXT heatedRearWindowActive;
    svc::BOOLEXT heatedSteeringWheelActive;
} climate_parameters;

typedef struct diag_parameters
{
    svc::TUPowerSource power;
    svc::TUDiagnostic speaker;
    svc::TUDiagnostic mic;
    svc::TUDiagnostic extHandsfree;
    svc::TUDiagnostic handset;
    svc::TUDiagnostic gsmModem;
    svc::TUDiagnostic gsmExtAntenna;
    svc::TUDiagnostic extPower;
    svc::TUDiagnostic intPower;
    svc::TUDiagnostic gnss;
    svc::TUDiagnostic gnssAntenna;
    svc::TUDiagnostic can;
    svc::TUDiagnostic buttons;
    svc::TUDiagnostic crashInput;
    svc::TUDiagnostic intRtc;
    uint32_t daysSinceGnssFix;
} diag_parameters;

typedef struct battery_parameters
{
    uint32_t primaryVoltx10;
    uint32_t primaryChargePerc;
    uint32_t secondaryVoltx10;
    uint32_t secondaryChargePerc;
} battery_parameters;

// must be the same whit RVM. Refer namespace RVM_FFH_Status_e on Typedef_rvm.h
typedef enum {
    CLIMATE_NONE                    = 100,
    CLIMATE_USER_INTERRUPTION       = 101,
    CLIMATE_STATUS_CHANGED          = 102,
    REMOTE_ENGINE_START_FAILURE     = 103,
    REMOTE_ENGINE_START_TONORMAL    = 104,
    VEHICLE_COMMUNICATION_FAILED    = 105,
    REMOTE_ENGIEN_STOP_FAILURE      = 106,
    FFH_MAX,
} request_to_rvm_type;
#endif /**_REMOTE_SERVICE_TYPE_H_*/
