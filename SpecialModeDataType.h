#ifndef SPECIALMODEDATATYPE_H
#define SPECIALMODEDATATYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <stdint.h>



#include "Typedef.h"
#include <utils/Timer.h>
#include <utils/Handler.h>
#include <utils/Message.h>
#include <utils/Log.h>

//#include "services/ApplicationManagerService/ApplicationManagerService.h"
#include "services/ApplicationManagerService/IApplicationManagerService.h"

//#include "services/DiagManagerService/DiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerService.h"
#include "services/DiagManagerService/IDiagManagerReceiver.h"

//#include "services/DebugManagerService/DebugManagerService.h"
#include "services/DebugManagerService/IDebugManagerService.h"
#include "services/DebugManagerService/IMgrReceiver.h"

//#include "services/vifManagerService/vifrxcansigval.h"
//#include "services/vifManagerService/vifManagerService.h"
#include "services/vifManagerService/IvifManagerService.h"
#include "services/vifManagerService/IvifManagerReceiver.h"

//#include "services/PowerManagerService/PowerManagerService.h"
//#include "services/PowerManagerService/IPowerModeReceiver.h"

#include "services/ConfigurationManagerService/IConfigurationManagerService.h"

#include "SpecialModeEvent.h"



#define SPECIALMODE_APPLICATION_ID 1013

#define SPCIALMODE_CONFIG_FILE "jlr_tcu_data_dictionary_swr_9_3.xml"


/****** Property for DemoMode : Start *********************/
#define PROPERTY_DEMO_STATUS "demoModeState"
#define PROPERTY_DEMOMODE_TIME "demoModeTime"
#define PROPERTY_DEMOMODE_USER_TIME "demoModeUserTime"
#define PROPERTY_CHANGE_FLAG_EC "demoChangeFlagEC"
#define PROPERTY_CHANGE_FLAG_BC "demoChangeFlagBC"

#define PROPERTY_DEMO_WIFI_STATUS "demoModeWifiState"

#define DEMOMODE_APP_MODE_SVT   "AppModeSVT"

#define DEMOMODE_APP_MODE_BC   "AppModeBC"
#define DEMOMODE_APP_MODE_EC   "AppModeEC"

/*----- Property for DemoMode : End --------------------*/

#define D_ENABLE 1
#define D_DISABLE 0

#define WIFI_OFF 0
#define WIFI_ON 1


/****** Diag command for DemoMode : Start *********************/
#define DIAG_CMD 0x31
#define DIAG_CMD2 0x01
#define DIAG_RESPONE_CMD 0x71
#define DIAG_ROUTINEID1 0xf0
#define DIAG_ROUTINEID2 0x0f

#define DIAG_REQ_ACTIVATE 0x01
#define DIAG_REQ_TERMINATED 0x02
#define DIAG_REQ_RESULT 0x03

#define DIAG_RES_ACTIVATE 0x01
#define DIAG_RES_TERMINATED 0x02
#define DIAG_RES_RESULT 0x03

#define DIAG_RESPONE_SIZE 3

//#define DIAG_RES_ACTIVATE 0x01
//#define DIAG_RES_TERMINATED 0x02

#define DIAG_RES_WIFI_ERROR_TERMINATED 0x02
#define DIAG_RES_CLOCK_ERROR 0x03
/*----- Diag command for DemoMode : End --------------------*/


/****** Debug command for DemoMode : Start *********************/
#define DEBUG_CMD 0x75
#define DEBUG_CMD2_ENABLE 0x31
#define DEBUG_CMD2_DISABLE 0x32
#define DEBUG_RESPNE_SIZE 1
#define DEBUG_RECIEVE_OK 0x01
/*----- Debug command for DemoMode : end --------------------*/


#define DEMOMODE_DISABLE 0x00
#define DEMOMODE_ENABLE 0x01


/****** For sldd command : Start *********************/
#define DEMOMODE_SLDD_ON 0x100
#define DEMOMODE_SLDD_OFF 0x200
#define DEMOMODE_SLDD_TIME_UNIT 0x300
#define DEMOMODE_SLDD_CMD 0x400
#define DEMOMODE_SLDD_DEMO_STATUS 0x500
#define DEMOMODE_SLDD_LOG_ON  0x1000
#define DEMOMODE_SLDD_LOG_OFF 0x2000

/* subparameter for DEMOMODE_SLDD_CMD */
#define DEMOMODE_SLDD_CMD_CMD1 0x401
#define DEMOMODE_SLDD_CMD_CMD2 0x402
/*----- For sldd command : End --------------------*/

#define DEMOMODE_SVT_ACTIVE 0x3e9

#define DEMOMODE_RES_SVT_INACTIVE_STATUS   0
#define DEMOMODE_RES_SVT_ALERT_STATUS   2
#define DEMOMODE_RES_SVT_ALARM_STATUS   3
#define TIMEOUT_SVT_DURATION 2  //sec

enum DemoModeStatus{
    E_DEMOMODE_INIT   = 0,
    E_DEMOMODE_REQUEST,
    E_DEMOMODE_START,
    E_DEMOMODE_STOP,
    E_DEMOMODE_PENDING,
    E_DEMOMODE_MAX
};

enum WiFiStatus {
    E_DISCONNECTED = 6,         // cause Diag check
    E_CONNECTED,
};

#endif // SPECIALMODEDATATYPE_H
