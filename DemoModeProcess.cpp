#define LOG_TAG "SpecialModeApplication"
#include "DemoModeProcess.h"
#include "SpecialModeHandler.h"
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/NGTPManagerService/INGTPManagerService.h>

#include "ServiceList.h"
#include <utils/Buffer.h>
#include <string>

uint8_t DEMOMODE_DEBUG              = 0;
static const uint8_t TIME_SEC       = 0x00;
static const uint8_t TIME_HOUR      = 0x01;
static char* timeUnit_str[]         = {"SECOND", "HOUR"};
static char* activeSim_str[]        = {"TSIM", "PSIM"};
static int32_t demoModeChangeFlagEC = 0;
static int32_t demoModeChangeFlagBC = 0;
static uint8_t eCallFlag            = 0;
static uint8_t bCallFlag            = 0;
struct timespec tstart_demo         = {0,};

DemoModeProcess::DemoModeProcess() : mp_PowerLock(NULL) {

#ifdef G_TEST
    LOGI("## DemoModeProcess create.");
    m_RunningTime = 5;
    m_TimeUnit = TIME_SEC;
    m_DemoModeStatus = E_DEMOMODE_INIT;
    m_WifiStatus =  E_CONNECTED;
    mp_PowerLock = new PowerLock(getpid());
#else
    LOGI("## DemoModeProcess create.");
    m_RunningTime = 0;
    m_TimeUnit = TIME_HOUR;
    m_DemoModeStatus = E_DEMOMODE_INIT;
    m_WifiStatus =  E_CONNECTED;
    mp_PowerLock = new PowerLock(getpid());
#endif
}

DemoModeProcess::~DemoModeProcess() {
    LOGI("~DemoModeProcess()++");
    checkBcallStatus();
    checkEcallStatus();
    delete mp_PowerLock;
    m_TimerSet.release();
}

void DemoModeProcess::handleTimerEvent(int timerId) {

#ifdef G_TEST
  LOGD("%s() DEMOMODE DEACTIVE G_TEST [%d]",__func__, timerId);
#else
    switch (timerId) {
        case 0:     // DemoModeTimer timeout
        {
            demoModeStop();
            break;
        }
        case 1:     // SVT register timeout
        {

            break;
        }       
        default:
            break;
        }
#endif    
}

void DemoModeProcess::demoModeClockReset() {
#ifdef G_TEST
    LOGV("G_TEST");
#else
    std::string d_str("0");
    // char d_str[32] = {0,};
    // sprintf(d_str, "%lu", d_str[0]);
    setPropertyChar("demoModeTime", d_str.c_str());
    setPropertyInt("demoModeUserTime", std::stoi(d_str));
#endif
}

void DemoModeProcess::doSpecialModeHandler(const sp<sl::Message>& message){

#ifdef G_TEST
#else

    int32_t arg1 = message->arg1;
    int32_t arg2 = message->arg2;

    switch (message->what) {
    case DEMOMODE_SLDD_ON:
        turnOnDemoMode((uint8_t)arg2, arg1);
        break;
    case DEMOMODE_SLDD_OFF:
        turnOffDemoMode();
        break;
    case DEMOMODE_SLDD_TIME_UNIT:
        m_TimeUnit = (uint8_t)arg1;
        break;
    case DEMOMODE_SLDD_DEMO_STATUS:
        break;
    case RECV_MSG_FROM_WIFI_ON:
        onWiFiStateOn();
        break;
    case RECV_MSG_FROM_WIFI_OFF:
        onWiFiStateOff();
        break;
    case RECV_MSG_FROM_CONFIG:
        onConfigChange(message);
        break;
    default:
        break;
    }

#endif
}

void DemoModeProcess::initializeProcess()
{

    LOGD("## DemoModeProcess::initializeProcess()");
    mp_DemoModeTimer = new DemoModeTimer(*this);
    m_TimerSet.initialize(mp_DemoModeTimer);
    demoModeClockReset();
    setDemoStatus(E_DEMOMODE_INIT);
    checkBcallStatus();
    checkEcallStatus();

}
bool DemoModeProcess::turnOnDemoMode(uint8_t timeUnit, int32_t timeValue) {

#ifdef G_TEST
    setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, timeValue);
    setDemoStatus(E_DEMOMODE_REQUEST);
    LOGV("## Success set demo status");
    m_TimeUnit = timeUnit;
    m_RunningTime = timeValue;
    demoModeStart();
    return true;
#else

    if(getWifiStatus() != E_CONNECTED){
        LOGI("## WIFI STATE IS DISCONNECTED, DEMO MODE CAN NOT START");
        setWifiStatus(E_DISCONNECTED);
        return false;
    }
    if(-1 == clock_gettime(CLOCK_MONOTONIC, &tstart_demo)) {
        LOGE("%s: Failed to call clock_gettime",__func__);
        demoModeClockReset();
        return false;
    }
    else {      // save the time value
         char d_str[32] = {0};
         LOGD("DemoModeStart() ==> clock_gettime : %ld, user_settime: %d, mTimeUnit[%s]", tstart_demo.tv_sec, timeValue, timeUnit_str[timeUnit]);
         sprintf(d_str, "%lu", tstart_demo.tv_sec);
          setPropertyChar(PROPERTY_DEMOMODE_TIME, d_str);
         if(timeUnit == TIME_SEC) {
             setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, timeValue);
         }
         else {
             setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, (timeValue * 3600));
        }
    }

    setDemoStatus(E_DEMOMODE_REQUEST);
    LOGV("## Success set demo status");
    m_TimeUnit = timeUnit;
    m_RunningTime = timeValue;
    demoModeStart();
    return true;
#endif

}

void DemoModeProcess::demoModeStart() {
    LOGD("%s() mTimeUnit[%s], mRunningTime[%d]", __func__, timeUnit_str[m_TimeUnit], m_RunningTime);
    lockPowerMode();

#ifdef G_TEST
    setDemoStatus(E_DEMOMODE_START);
    disableBCall();
    disableECall();
#else
    if(m_TimeUnit == TIME_SEC) {
        LOGV("%s : %d seconds ", __func__, time);
        m_TimerSet.startTimer(0, m_RunningTime);        // unit:Seconds
    }
    else {
        LOGV("%s : %d hours ", __func__, time);
        m_TimerSet.startTimer(0, m_RunningTime * 3600); // unit:Hours
    }
    setDemoStatus(E_DEMOMODE_START);
    disableBCall();
    disableECall();
    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>
                  (defaultServiceManager()->getService(String16("service_layer.NGTPManagerService")));
    iNGTP->setDemoMode(true);
    LOGI("####################   DEMO MODE WAS STARTED !.   ####################");
#endif

}

bool DemoModeProcess::turnOffDemoMode() {
#ifdef G_TEST
    demoModeStop();
#else
    if (getDemoStatus() == E_DEMOMODE_START || getDemoStatus() == E_DEMOMODE_PENDING) {
        m_TimerSet.stopTimer(0);
        demoModeStop();
    }
    else {
        LOGI("####################   DEMO MODE WAS ALREADY STOPPED.   ####################");
        return false;
    }
#endif

    return true;
}

void DemoModeProcess::demoModeStop() {
#ifdef G_TEST
    demoModeClockReset();
    setDemoStatus(E_DEMOMODE_STOP);
    releasePoweMode();
    checkBcallStatus();
    checkEcallStatus();
    LOGI("####################   DEMO MODE WAS STOPPED.   ####################");
#else
    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>
                 (defaultServiceManager()->getService(String16("service_layer.NGTPManagerService")));
    iNGTP->setDemoMode(false);
    demoModeClockReset();
    setDemoStatus(E_DEMOMODE_STOP);
    releasePoweMode();
    checkBcallStatus();
    checkEcallStatus();
    LOGI("####################   DEMO MODE WAS STOPPED.   ####################");
#endif
}

void DemoModeProcess::setDemoStatus(DemoModeStatus status) {
    LOGD("%s: set value:%d", __func__, status);
    sp<Post> post;
    m_DemoModeStatus = status;
    setPropertyInt(PROPERTY_DEMO_STATUS, m_DemoModeStatus);
#ifdef G_TEST
    LOGD("## broadcast post:: G_TEST");
#else
    switch(status)
    {
    case E_DEMOMODE_START:

        LOGD("## broadcast post:: DEMO MODE START");
        post = Post::obtain(0);
        post->action = "post.action.SEND_CALL";
        post->what = 0;   // wait for define
        post->arg1 = 1;
        post->arg2 = 2;
        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        break;

    case E_DEMOMODE_STOP:
        LOGD("## broadcast post:: DEMO MODE END");
        post = Post::obtain(0);
        post->action = "post.action.SEND_CALL";
        post->what = 0;
        post->arg1 = 1;
        post->arg2 = 2;
        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        break;

    case E_DEMOMODE_PENDING:
        LOGD("## broadcast post:: DEMO MODE PENDING");
        post = Post::obtain(0);
        post->action = "post.action.SEND_CALL";
        post->what = 0;
        post->arg1 = 1;
        post->arg2 = 2;
        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        break;

    default:
        break;
    }
#endif
}

DemoModeStatus DemoModeProcess::getDemoStatus(){
     LOGD("%s: return value:%d", __func__, m_DemoModeStatus);
    return m_DemoModeStatus;
}

void DemoModeProcess::setWifiStatus(WiFiStatus status){
    LOGD("%s: setWifiStatus() set value:%d", __func__, status);
    m_WifiStatus = status;
    setPropertyInt(PROPERTY_DEMO_WIFI_STATUS, m_WifiStatus);
}

WiFiStatus DemoModeProcess::getWifiStatus(){
    LOGD("%s: getWifiStatus() return value:%d", __func__, m_WifiStatus);
    return m_WifiStatus;
}

void DemoModeProcess::onWiFiStateOn(){
    LOGI("## Wifi status is changing: E_DISCONNECT => E_CONNECT");
    setWifiStatus(E_CONNECTED);
    if(getDemoStatus() == E_DEMOMODE_PENDING) {
        setDemoStatus(E_DEMOMODE_START);
    }
}

void DemoModeProcess::onWiFiStateOff(){
    LOGI("## Wifi status is changing:\n E_CONNECT => E_DISCONNECT");
    setWifiStatus(E_DISCONNECTED);
    if(getDemoStatus() == E_DEMOMODE_START) {
        setDemoStatus(E_DEMOMODE_PENDING);
    }
}

void DemoModeProcess::onConfigChange(const sp<sl::Message>& message) {
    LOGI("Configuration data change");
    char* name = message->buffer.data();
    LOGI("#### buffer.data = %s", name);
    if(strcmp(DEMOMODE_APP_MODE_BC, name) == 0 || strcmp(DEMOMODE_APP_MODE_EC, name)) {
        bCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);
        eCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
        LOGV("%s() EBCALL RECV_MSG_FROM_CONFIG bcFlag=%d, ecFlag=%d", __func__, bCallFlag, eCallFlag);

        if ((getDemoStatus() == E_DEMOMODE_START) ||(getDemoStatus() == E_DEMOMODE_PENDING)) {
            if (bCallFlag == 1) {
                m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 0);
            }
            else if (eCallFlag == 1) {
                m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 0);
            }
        }
        else{
            LOGV("%s() SPECIAL MODE NOT START or PENDING !!", __func__);
        }
    }
}

void DemoModeProcess::checkBcallStatus() {
#ifdef G_TEST
    LOGI("%s() bCallFlag property: %d", __func__, bCallFlag);
#else
    bCallFlag = m_ServicesMgr ->getConfigurationManager()
            ->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);
    demoModeChangeFlagBC = std::stoi(getPropertyWrap(PROPERTY_CHANGE_FLAG_BC));

    if(demoModeChangeFlagBC == 1) {
        if(bCallFlag == 0)
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC,1);
        else
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC,0);
        setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 0);
    }
#endif
}

// TODO remove ifdef
void DemoModeProcess::checkEcallStatus() {
#ifdef G_TEST
    LOGI("%s() bCallFlag property: %d", __func__, eCallFlag);
#else
    eCallFlag = m_ServicesMgr ->getConfigurationManager()
            ->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
    demoModeChangeFlagEC = std::stoi(getPropertyWrap(PROPERTY_CHANGE_FLAG_EC));

    if(demoModeChangeFlagEC == 1) {
        if(eCallFlag == 0)
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC,1);
        else
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC,0);
        setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 0);
    }
#endif

}

// TODO remove ifdef
void DemoModeProcess::disableBCall() {
#ifdef G_TEST
    setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 1);
#else
    bCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);
    if (bCallFlag == 1) {
        LOGI("################# BCALL DISABLED !!#################");
        setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 1);
        m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 0);
    }
#endif
}

// TODO remove ifdef
void DemoModeProcess::disableECall() {
#ifdef G_TEST
    setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 1);
#else
    eCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
    if (eCallFlag == 1) {
        LOGI("################# ECALL DISABLED !!#################");
        setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 1);
        m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 0);
    }
#endif
}

void DemoModeProcess::lockPowerMode(){
#ifdef G_TEST
    m_CheckPower = true;
    mp_PowerLock->acquire();
#else
    LOGI("%s()", __func__);
    if(m_CheckPower) {
        LOGI("PowerMode already lock");
    } else {
        m_CheckPower = true;
        mp_PowerLock->acquire();
    }
#endif
}

void DemoModeProcess::releasePoweMode(){
#ifdef G_TEST
    m_CheckPower = false;
    mp_PowerLock->release();
#else
    LOGI("%s()", __func__);
    if(m_CheckPower){
        m_CheckPower = false;
        mp_PowerLock->release();
    } else {
        LOGI("PowerMode already release");
    }
#endif
}
