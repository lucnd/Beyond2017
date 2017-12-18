/**
* \file    DemoModeProcess.cpp
* \brief     Declaration of DemoModeProcess
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       junghun77.kim
* \date    2016.12.13
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#define LOG_TAG "DemoModeProcess"

#include "DemoModeProcess.h"
#include "SpecialModeHandler.h"
#include "SpecialModeType.h"

uint8_t DEMOMODE_DEBUG = 0;
#define DLOGI(...)  ({if(DEMOMODE_DEBUG) LOGI(__VA_ARGS__); \
                      else LOGV(__VA_ARGS__);})
#define DLOGD(...)   if(DEMOMODE_DEBUG) LOGD(__VA_ARGS__)

static const uint8_t TIME_SEC = 0x00;
static const uint8_t TIME_HOUR = 0x01;
static char* timeUnit_str[] = {"SECOUND", "HOUR"};
static char* activeSim_str[] = {"TSIM", "PSIM"};

static int32_t demoModeChangeFlagEC = 0;
static int32_t demoModeChangeFlagBC = 0;

static uint8_t ecFlag;
static uint8_t bcFlag;

struct timespec tstart_demo = {0,};


static bool mDiagStateCheck = false;

DemoModeProcess::DemoModeProcess():mp_KeepPower(NULL){
    LOGI("## DemoModeProcess create.");
    mp_KeepPower    = new KeepPower(APP_ID_SPECIAL);

    m_pSimCheck = false;
    mCheckPower = false;
    mRunningTime = 0;
    mTimeUnit = TIME_HOUR;
    mDemoModeStatus = DEMOMODE_STATUS_INIT;
    bSVTRegOnDemo = false;
    bSVTAlertStatus= false;
    mWifiStatus =  WIFI_DISCONNECTED_DEMO;

}

DemoModeProcess::~DemoModeProcess(){
    LOGI("~DemoModeProcess()++");

    checkEBcallStatus();

    if(mp_KeepPower != NULL) {
        delete mp_KeepPower;
    }
    m_TimerSet.release();

}

void DemoModeProcess::activatePSIM() {
    int32_t activeSim = TelephonyManager::getActiveSim();

    LOGI("%s()  activeSim[ %s ] ", __func__, activeSim_str[activeSim]);
    if(activeSim == TelephonyManager::REMOTE_SIM_PSIM){
        LOGD("%s() DemoMode: REMOTE_SIM_PSIM keep going!! ", __func__);
        acquirePSimLock();
    }
    else if (activeSim == TelephonyManager::REMOTE_SIM_TSIM){
        LOGD("%s() DemoMode: REMOTE_SIM_TSIM - sim switting to PSIM ", __func__);
        m_pSimCheck = true;
        // switching to PSIM
        TelephonyManager::setActiveSim(TelephonyManager::REMOTE_SIM_PSIM,
                                       TelephonyManager::REMOTE_SIM_PRIORITY_HIGH,
                                       TelephonyManager::REMOTE_SIM_CAUSE_DEMO_MODE);
    }
    else if (activeSim == TelephonyManager::REMOTE_SIM_IN_PROGRESS){
        LOGD("%s() DemoMode: REMOTE_SIM_IN_PROGRESS!! ", __func__);
        m_pSimCheck = true;
    }
    else{
        LOGE("%s() DemoMode: activatePSIM() - Fail!!! ", __func__);
    }
}

void DemoModeProcess::do_KeepPower(){
    LOGV("%s()",__func__ );

    if(mCheckPower == true) {
        LOGI("AGAIN : do_KeepPowering ");
    } else {
        mCheckPower = true;
        mp_KeepPower->acquire();
    }
}

void DemoModeProcess::do_releaseKeepPower(){
    LOGV("%s()",__func__ );

    if(mCheckPower == true) {
        mCheckPower = false;
        mp_KeepPower->release();
    } else {
        LOGI("Already KeepPower release!!");
    }
}

//void DemoModeProcess::do_Telephony(){
//    LOGV("do_Telephony()");
//}

void DemoModeProcess::handleEvent(uint32_t ev){
    LOGV("%s()",__func__ );
}

void DemoModeProcess::handleTimerEvent(int timerId) {   //handlerFunction()
    LOGD("%s() timerId[%d]",__func__, timerId);

    switch(timerId) {
        case 0:
        {
            LOGD("%s() DEMOMODE DEACTIVE [%d]",__func__, timerId);
            DemoModeStop();
        }
            break;
        case 1:
        {
            LOGD("%s() SVT RETRY REGISTER - timer expired [%d]",__func__, timerId);
            if(bSVTRegOnDemo == false)
                registerSVTpostReceiver();
        }
        break;

        default:
            break;
    }
}

int DemoModeProcess::byteToInt1(byte data[], int idx)
{
    return
    (
        ((data[idx+3] & 0xFF) << 24) |
        ((data[idx+2] & 0xFF) << 16) |
        ((data[idx+1] & 0xFF) << 8) |
        (data[idx] & 0xFF)
    );
}

void DemoModeProcess::do_SpecialModeHandler(int32_t what, const sp<sl::Message>& message)
{
    int32_t arg1=message->arg1;
    int32_t arg2=message->arg2;
    int32_t arg3=message->arg3;

    DLOGD("do_SpecialModeHandler() what[%d],arg1[%d],arg2[%d],arg3[%d],buf size[%d]",what,arg1,arg2,arg3,message->buffer.size());
    if(message->buffer.size() > 0) {
        LOGD("buff[0]=%x buff[1]=%x buff[2]=%x buff[3]=%x",*(message->buffer.data()),*(message->buffer.data()+1),
        *(message->buffer.data()+2),*(message->buffer.data()+3));
    }

    switch (what){
        case RECV_MSG_FROM_DEBUG_COMMAND:
            if(arg2 == DEBUG_CMD2_ENABLE) {
                if (message->buffer.size() == 4) {
                    LOGV(" ### RECV_MSG_FROM_DEBUG_COMMAND INT32/UINT32 : %d ",byteToInt1(message->buffer.data(), 0));
                    TurnOnDemoMode(TIME_HOUR, byteToInt1(message->buffer.data(), 0));
                }
                else {
                    LOGV(" ### RECV_MSG_FROM_DEBUG_COMMAND INT8/UINT8 : %d ", *(message->buffer.data()));
                    TurnOnDemoMode(TIME_HOUR, *(message->buffer.data()));
                }
            }
            else if(arg2 == DEBUG_CMD2_DISABLE)
                TurnOffDemoMode();
            else
                LOGV("RECV_MSG_FROM_DEBUG_DATA wrong buffer is %x",message->buffer.data());
            break;

        case RECV_MSG_FROM_DIAGNOSTICS_DATA:
            LOGD("%s RECV_MSG_FROM_DIAGNOSTICS_DATA", __func__);
            if(arg2 == DIAG_REQ_ACTIVATE) {
                TurnOnDemoMode(TIME_HOUR, *(message->buffer.data()+2));
            }
            else if(arg2 == DIAG_REQ_TERMINATED) {
                LOGI("%s() DIAG_REQ_TERMINATED %d",__func__, arg2);
                TurnOffDemoMode();
            }
            else if(arg2 == DIAG_REQ_RESULT) {
                if(getWifiStatus() == WIFI_DISCONNECTED_DEMO) {
                    LOGI("%s() WiFi Disconnected - DIAG_REQ_RESULT %d, DemoModestate %d",__func__, arg2, getDemoStatus());
                    if (getDemoStatus() == DEMOMODE_STATUS_PENDING)
                        mDiagStateCheck = false;
                    else
                        mDiagStateCheck = true;
                }
            }
            else
                LOGI("RECV_MSG_FROM_DIAGNOSTICS_DATA wrong command %d", arg2);
            sendResponse(what,message);
            break;

        case RECV_MSG_FROM_TELEPHONY_PSIM_STATE_CHANGED:
            DLOGD("DemoMode Handler: RECV_MSG_FROM_TELEPHONY_PSIM_STATE_CHANGED, simAct[%d], event[%d], slot[%d] ", arg1,arg2,arg3);
            //onPSimStateChanged(arg1, arg2, arg3);
            break;

        case RECV_MSG_FROM_TELEPHONY_ACTIVESIM_STATE_CHANGED:
            DLOGD("DemoMode Handler: RECV_MSG_FROM_TELEPHONY_ACTIVESIM_STATE_CHANGED, activeSim[%d], ", arg1);
            //onActiveSimStateChanged(arg1);
            break;

        case RECV_MSG_FROM_TELEPHONY_PSIM_LOCK_STATE_CHANGED:
            DLOGD("DemoMode Handler: RECV_MSG_FROM_TELEPHONY_PSIM_LOCK_STATE_CHANGED, PSimLocked[%d], ", arg1);
            //onPsimLockStateChanged(arg1);
            break;

        case DEMOMODE_SLDD_ON:
            LOGD("DEMOMODE_SLDD_ON do KeepPower Time[%d], TimeUnit[%d:%s] ", arg1, (uint8_t)arg2, timeUnit_str[arg2]);
            TurnOnDemoMode((uint8_t)arg2, arg1);
            break;

        case DEMOMODE_SLDD_OFF:
            LOGD("DEMOMODE_SLDD_OFF Release KeepPower");
            TurnOffDemoMode();
            break;

        case DEMOMODE_SLDD_TIME_UNIT:
            LOGD("DEMOMODE_SLDD_TIME_UNIT mTimeUnit[%s]", timeUnit_str[(uint8_t)arg1]);
            mTimeUnit = (uint8_t)arg1;
            break;

        case DEMOMODE_SLDD_CMD:  //for TEST.
            LOGD("DEMOMODE_SLDD_CMD command[%d]", arg1);
            break;

        case DEMOMODE_SLDD_LOG_ON:
            LOGD("DEMOMODE_SLDD_LOG_ON ");
            DEMOMODE_DEBUG = 1;
            break;

        case DEMOMODE_SLDD_LOG_OFF:
            LOGD("DEMOMODE_SLDD_LOG_OFF ");
            DEMOMODE_DEBUG = 0;
            break;

        case RECV_MSG_FROM_WIFI_ON:
           DLOGD("RECV_MSG_FROM_WIFI_ON status[%d]", message->arg1);
           setWifiStatus(WIFI_CONNECTED_DEMO);
            if(getDemoStatus() == DEMOMODE_STATUS_PENDING) {
                setDemoStatus(DEMOMODE_STATUS_START);
            }
           break;

          case RECV_MSG_FROM_WIFI_OFF:
            DLOGD("RECV_MSG_FROM_WIFI_OFF status[%d]", message->arg1);
            setWifiStatus(WIFI_DISCONNECTED_DEMO);
            if(getDemoStatus() == DEMOMODE_STATUS_START) {
                setDemoStatus(DEMOMODE_STATUS_PENDING);
            }
           //TurnOffDemoMode();
            break;

        case DEMOMODE_SVT_ACTIVE:
        {
           DLOGD("Received SVT alert event");
           if(arg1 == DEMOMODE_RES_SVT_INACTIVE_STATUS) {
               bSVTRegOnDemo = false;
           }
           else {
               bSVTRegOnDemo = true;
           }

           if((arg1 == DEMOMODE_RES_SVT_ALERT_STATUS) || (arg1 == DEMOMODE_RES_SVT_ALARM_STATUS)) {
               LOGV("SVT ALERT ENABLE");
               bSVTAlertStatus = true;
               if ((getDemoStatus() == DEMOMODE_STATUS_START) ||(getDemoStatus() == DEMOMODE_STATUS_PENDING)) {
                   TurnOffDemoMode();
                   sp<Post> post = Post::obtain((appid_t)APP_ID_SPECIAL, getDemoStatus());
                    m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, post);
               }
           }
           else {
               bSVTAlertStatus = false;
           }
        }
        break;

        case RECV_MSG_FROM_CONFIG:
        {
            DLOGD("Received config event");
            char* t_name  = message->buffer.data();
            if(strcmp(DEMOMODE_APP_MODE_SVT, t_name) == 0) {
                if(getSVTConfig(DEMOMODE_APP_MODE_SVT) == D_ENABLE) {
                    LOGV("%s() SVT RECV_MSG_FROM_CONFIG Enabled", __func__);
                    registerSVTpostReceiver();
                }
                else {
                    LOGV("%s() SVT RECV_MSG_FROM_CONFIG Disabled bSVTRegOnDemo = [%d]", __func__, bSVTRegOnDemo);
                    bSVTRegOnDemo = false;
                }
            }
            else if ((strcmp(DEMOMODE_APP_MODE_BC, t_name) == 0) || (strcmp(DEMOMODE_APP_MODE_BC, t_name) == 0)) {
                ecFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
                bcFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);
                LOGV("%s() EBCALL RECV_MSG_FROM_CONFIG bcFlag=%d, ecFlag=%d", __func__, bcFlag, ecFlag);
                    if ((getDemoStatus() == DEMOMODE_STATUS_START) ||(getDemoStatus() == DEMOMODE_STATUS_PENDING)) {
                        if (bcFlag == 1) {
                            m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 0);
                            TurnOffDemoMode();
                        }
                        else if (ecFlag == 1) {
                            m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 0);
                            TurnOffDemoMode();
                        }
                    }
                    else
                        LOGV("%s() NOT START or PENDING !!", __func__);
             }
        }
        break;

        default:
            LOGE("%s()  what[0x%x], arg1[%d], arg2[%d], arg3[%d] ", __func__, what, arg1, arg2, arg3);
            break;
    }
}

uint8_t DemoModeProcess::getSVTConfig(const char* name) {
    uint8_t result = 0;
    sp<ConfigurationBuffer> buf = m_ServicesMgr->getConfigurationManager()->get(SPCIALMODE_CONFIG_FILE, 2, name);

    if(buf != NULL && buf->errorFlag() == E_OK) {
        buf->data(&result);
        LOGV("%s() getSVTConfig() parameter ==> name:%s value:%d", __func__, name, result);
    }
    else {
        LOGE("%s() getSVTConfig() Can't read config parameter: %s", __func__, name);
    }
    return result;
}

void DemoModeProcess::registerSVTpostReceiver() {
    uint8_t appStatus = (uint8_t) m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, "AppModeSVT");
    LOGD("%s(): SVT appStatus [%d], bSVTRegOnDemo [%d]",__func__, appStatus, bSVTRegOnDemo);

    if(bSVTRegOnDemo == false && m_ServicesMgr->getApplicationManager()->isEnabledApplication(APP_ID_SVT))
    {
        sp<Post> post = Post::obtain((appid_t)APP_ID_SPECIAL);
        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, post);

        m_TimerSet.startTimer(1, TIMEOUT_SVT_DURATION);  // check to two seconds
    }
}

void DemoModeProcess::onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot)
{
    LOGD("%s  simAct[%s], event[%d], slot[%d]", __func__, activeSim_str[simAct], event, slot);

    if (simAct == TelephonyManager::REMOTE_SIM_PSIM) {
        if (event == TelephonyManager::PSIM_STATE_EJECTED) {
            TurnOffDemoMode();
        } else if (event == TelephonyManager::PSIM_STATE_ERROR) {
            TurnOffDemoMode(); // when PSIM is removed.
        } else {
            // Ignore
        }
    }
}

void DemoModeProcess::onActiveSimStateChanged(int32_t activeSim)
{
    LOGD("%s()  activeSim:[%s], m_pSimCheck: %d", __func__, activeSim_str[activeSim], m_pSimCheck);

    if(activeSim == TelephonyManager::REMOTE_SIM_PSIM && m_pSimCheck == true) {
       acquirePSimLock();
       m_pSimCheck = false;
    }
    else if(activeSim == TelephonyManager::REMOTE_SIM_TSIM && m_pSimCheck == true) {
        // switching to PSIM
        TelephonyManager::setActiveSim(TelephonyManager::REMOTE_SIM_PSIM,
                                       TelephonyManager::REMOTE_SIM_PRIORITY_HIGH,
                                       TelephonyManager::REMOTE_SIM_CAUSE_DEMO_MODE);
        m_pSimCheck = false;
    }
    else {
       LOGI("DemoMode: not acquire this case on specialmode");
    }
}


void DemoModeProcess::demoModeClockReset() {

    char d_str[32] = {0,};
    sprintf(d_str, "%lu", d_str[0]);
    setPropertyChar("demoModeTime", d_str);
    setPropertyInt("demoModeUserTime", 0);
}

void DemoModeProcess::onPsimLockStateChanged(bool locked)
{
    LOGD("%s()  locked[ %d ]", __func__, locked);
    if(true == locked) {
        DemoModeStart();
    }
}

void DemoModeProcess::initializeProcess()
{
    LOGD("## DemoModeProcess::initializeProcess()");
    sp<ISystemManagerService> systemService = interface_cast<ISystemManagerService>
                                                                        (defaultServiceManager()->getService(
                                                                            String16(SystemManagerService::getServiceName())));

    int32_t mdmVariant  = systemService->getMDMvariant();
    LOGV("%s() mdmVariant : %d", __func__, mdmVariant);

    if(mdmVariant == MY18_HIGH) {
        LOGD("%s() getWiFiConnectionState():[%d]", __func__, m_ServicesMgr->getWifiManager()->getWiFiConnectionState());
        if(m_ServicesMgr->getWifiManager()->getWiFiConnectionState() == 0)
            setWifiStatus(WIFI_DISCONNECTED_DEMO);
        else
            setWifiStatus(WIFI_CONNECTED_DEMO);
    }
    else
        LOGV("%s() NOT HIGH BOARD !!", __func__);

    mp_DemoModeTimer = new DemoModeTimer(*this);
    m_TimerSet.initialize(mp_DemoModeTimer);

    demoModeClockReset();
    setDemoStatus(DEMOMODE_STATUS_INIT);
    checkEBcallStatus();
    registerSVTpostReceiver();
}

void DemoModeProcess::DemoModeStart()
{
    LOGD("%s() mTimeUnit[%s], mRunningTime[%d]", __func__, timeUnit_str[mTimeUnit], mRunningTime);

    do_KeepPower();

    if(mTimeUnit == TIME_SEC) {
        LOGV("%s : %d seconds ", __func__, time);
        m_TimerSet.startTimer(0, mRunningTime);        // unit:Secounds (for only TEST)
    }
    else {
        LOGV("%s : %d hours ", __func__, time);
        m_TimerSet.startTimer(0, mRunningTime * 3600); // unit:Hours
    }

    setDisableEbcallStatus();
    setDemoStatus(DEMOMODE_STATUS_START);

    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>
                 (defaultServiceManager()->getService(String16( NGTPManagerService::getServiceName())));
    iNGTP->setDemoMode(true);

    LOGI("####################   DemoMode was started.   ####################");

    return true;
}

void DemoModeProcess::setDisableEbcallStatus() {
    ecFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
    bcFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);

    LOGD("%s() , Check to ecall/bcall status : ecFlag=%d, bcFlag=%d", __func__, ecFlag, bcFlag);

    if(ecFlag ==1) {
        LOGI("################# ECALL DISABLED !!#################");
        setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 1);  // set to 1 for disable Ecall check
        m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 0);
    }

    if(bcFlag ==1) {
        LOGI("################# BCALL DISABLED !!#################");
        setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 1); // set to 1 for disable Bcall check
        m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 0);
    }
}

bool DemoModeProcess::TurnOnDemoMode(uint8_t timeUnit, int32_t m_time)
{
    int32_t statePSim = TelephonyManager::getPSimState();

    LOGD("%s() =====================================", __func__);
    LOGD("%s()  timeUnit[%d:%s], timeValue(usertime)[%d], PSimState[%d], getWifiStatus()[%d] ", __func__, timeUnit, timeUnit_str[timeUnit], m_time, statePSim, getWifiStatus() );

    if (getWifiStatus() != WIFI_CONNECTED_DEMO){
      LOGI("####################   WIFI DISCONNECT MODE => DEMOMODE OFF  ####################");
      mDiagStateCheck = true;
      setWifiStatus(WIFI_DISCONNECTED_DEMO);
      return false;
    }

    if(-1 == clock_gettime(CLOCK_MONOTONIC, &tstart_demo)) {
        LOGE("%s: Failed to call clock_gettime",__func__);
        demoModeClockReset();
        return false;
    }
    else {
         char d_str[32] = {0,};
         LOGD("DemoModeStart() ==> clock_gettime : %ld, user_settime: %d, mTimeUnit[%s]", tstart_demo.tv_sec, m_time, timeUnit_str[timeUnit]);
         sprintf(d_str, "%lu", tstart_demo.tv_sec);
         setPropertyChar(PROPERTY_DEMOMODE_TIME, d_str);
         if(timeUnit == TIME_SEC) {
             setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, m_time);
         }
         else {
             setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, (m_time * 3600));
        }
    }

    setDemoStatus(DEMOMODE_STATUS_REQUEST);

    mTimeUnit = timeUnit;
    mRunningTime = m_time;

    DemoModeStart();

    return true;
}

void DemoModeProcess::DemoModeStop()    // timer expired
{
    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>
                 (defaultServiceManager()->getService(String16( NGTPManagerService::getServiceName())));
    iNGTP->setDemoMode(false);

    demoModeClockReset();
    setDemoStatus(DEMOMODE_STATUS_STOP);
    do_releaseKeepPower();
    checkEBcallStatus();

    LOGI("####################   DemoMode was stoped.   ####################");
}

bool DemoModeProcess::TurnOffDemoMode(){
    LOGD("%s() =====================================", __func__);

    if ((getDemoStatus() == DEMOMODE_STATUS_START) ||(getDemoStatus() == DEMOMODE_STATUS_PENDING)) {
      m_TimerSet.stopTimer(0);
      DemoModeStop();
    }
    else {
      LOGI("####################   DemoMode has already stoped.   ####################");
      return false;
    }
    return true;
}

void DemoModeProcess::setDemoStatus(DEMO_MODE_STATUS status)
{
    LOGD("%s: set value:%d", __func__, status);
    sp<Post> post;
    sp<Post> postSVT;

    mDemoModeStatus = status;
    setPropertyInt(PROPERTY_DEMO_STATUS, mDemoModeStatus);

    switch(status)
    {
    case DEMOMODE_STATUS_START:
        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_START);
        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        postSVT = Post::obtain((appid_t)APP_ID_SPECIAL, mDemoModeStatus);
        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, postSVT);
        break;
    case DEMOMODE_STATUS_STOP:
        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_END);
        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        postSVT = Post::obtain((appid_t)APP_ID_SPECIAL, mDemoModeStatus);
        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, postSVT);
        break;
    case DEMOMODE_STATUS_PENDING:
        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_PENDING);
        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        break;
    default:
        break;
    }
}

DEMO_MODE_STATUS DemoModeProcess::getDemoStatus()
{
    LOGD("%s: return value:%d", __func__, mDemoModeStatus);
    return mDemoModeStatus;
}

void DemoModeProcess::setWifiStatus(WCONNECT_STATUS status)
{
    LOGD("%s: setWifiStatus() set value:%d", __func__, status);
    mWifiStatus = status;
    setPropertyInt(PROPERTY_DEMO_WIFI_STATUS, mWifiStatus);
}

WCONNECT_STATUS DemoModeProcess::getWifiStatus()
{
    LOGD("%s: getWifiStatus() return value:%d", __func__, mWifiStatus);
    return mWifiStatus;
}

void DemoModeProcess::checkEBcallStatus()
{
    ecFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
    bcFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);

    demoModeChangeFlagBC = sl::atoi(getPropertyWrap(PROPERTY_CHANGE_FLAG_BC));
    demoModeChangeFlagEC = sl::atoi(getPropertyWrap(PROPERTY_CHANGE_FLAG_EC));

    LOGI("%s() ecFlag property: %d, bcFlag property: %d", __func__,ecFlag, bcFlag);
    LOGI("%s() demoChangeFlagBC property: %d, demoChangeFlagEC property: %d", __func__,demoModeChangeFlagBC, demoModeChangeFlagEC);

    if (demoModeChangeFlagBC == 1) {
        if (bcFlag == 0)
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 1);
        else
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 0);

        setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 0);
    }
    if (demoModeChangeFlagEC == 1) {
        if (ecFlag == 0)
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 1);
        else
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPCIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 0);

        setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 0);
    }
}

void DemoModeProcess::sendResponse(int32_t what, const sp<sl::Message>& message) {
    LOGD("%s() what:%d", __func__, what);
    sp<Buffer> buf = new Buffer();
    uint8_t responsePayload[DIAG_RESPONE_SIZE];
    uint8_t responsePayload_1[DIAG_RESPONE_SIZE-1];

//    uint8_t responsePayload_2[DIAG_RESPONE_SIZE+2];

    switch (what){
        case RECV_MSG_FROM_DEBUG_COMMAND:
            break;

         case RECV_MSG_FROM_DIAGNOSTICS_DATA:
            LOGD("%s() mDemoModeStatus:%d, getwifiStatus():%d, mDiagStateCheck:%d", __func__, mDemoModeStatus, getWifiStatus(), mDiagStateCheck);
            if(message->arg2 == DIAG_REQ_ACTIVATE) {                         // diag start check
                responsePayload[0] = *(message->buffer.data());
                responsePayload[1] = *(message->buffer.data()+1);
                responsePayload[2] = DIAG_RES_TERMINATED;

                buf->clear();
                buf->setTo(responsePayload, 3);
                m_ServicesMgr->getDiagManager()->responseDiagcmd(0, DIAG_RESPONE_CMD, DIAG_RES_ACTIVATE, buf);
            }
            else if(message->arg2  == DIAG_REQ_TERMINATED) {
                responsePayload_1[0] = *(message->buffer.data());
                responsePayload_1[1] = *(message->buffer.data()+1);

                buf->clear();
                buf->setTo(responsePayload_1, 2);
                m_ServicesMgr->getDiagManager()->responseDiagcmd(0, DIAG_RESPONE_CMD, DIAG_RES_TERMINATED, buf);
            }
            else if(message->arg2  == DIAG_REQ_RESULT) {
                responsePayload[0] = *(message->buffer.data());
                responsePayload[1] = *(message->buffer.data()+1);

                if(mDiagStateCheck == true) {                   // wifi disconnected
                    responsePayload[2] = getWifiStatus();
                }
                else {
                    responsePayload[2] = getDemoStatus();
                }

                buf->clear();
                buf->setTo(responsePayload, 3);
                m_ServicesMgr->getDiagManager()->responseDiagcmd(0, DIAG_RESPONE_CMD, DIAG_RES_RESULT, buf);
            }
            else {
                LOGD("%s() DO NOT SUPPORTED DIAG COMMAND : %d", __func__, message->arg2);
            }
            break;
        default:
            break;
    }
    mDiagStateCheck = false;
}
