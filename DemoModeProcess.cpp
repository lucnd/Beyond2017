#define LOG_TAG "SpecialModeApplication"
#include "DemoModeProcess.h"
#include "SpecialModeHandler.h"
#include "ServiceList.h"
#include <utils/Buffer.h>
#include <string>

static const uint32_t HOUR_TO_SEC   = 0x0E10;
static const uint8_t TIME_SEC       = 0x00;
static const uint8_t TIME_HOUR      = 0x01;
static const char* timeUnit_str[]   = {"SECOND", "HOUR"};
static uint8_t eCallFlag            = 0;
static uint8_t bCallFlag            = 0;
struct timespec tstart_demo         = {};

DemoModeProcess::DemoModeProcess() {
    LOGI("## DemoModeProcess create.");
    m_CheckPower = false;
    m_RunningTime = 0;
    m_TimeUnit = TIME_HOUR;
    m_DemoModeStatus = E_DEMOMODE_INIT;
    m_WifiStatus =  E_CONNECTED;
    mp_PowerLock = new PowerLock(getpid());
}

DemoModeProcess::~DemoModeProcess() {
    LOGI("~DemoModeProcess()++");
    delete mp_PowerLock;
    m_TimerSet.release();
}

void DemoModeProcess::handleTimerEvent(int timerId) {
    if(timerId == 0){
        demoModeStop();
    }
}

void DemoModeProcess::demoModeClockReset() {
    std::string d_str("0");
    setPropertyChar("demoModeTime", d_str.c_str());
    setPropertyInt("demoModeUserTime", std::stoi(d_str));
}

void DemoModeProcess::doSpecialModeHandler(const sp<sl::Message>& message){
    int32_t arg1 = message->arg1;
    int32_t arg2 = message->arg2;

    switch (message->what) {
    case DEMOMODE_SLDD_ON:
        turnOnDemoMode(static_cast<uint8_t>(arg2), arg1);
        break;
    case DEMOMODE_SLDD_OFF:
        turnOffDemoMode();
        break;
    case DEMOMODE_SLDD_TIME_UNIT:
        m_TimeUnit = static_cast<uint8_t>(arg1);
        break;
    case DEMOMODE_SLDD_DEMO_STATUS:
        break;
    case E_FROM_WIFI_ON:
        onWiFiStateOn();
        break;
    case E_FROM_WIFI_OFF:
        onWiFiStateOff();
        break;
    case E_FROM_CONFIG:
        onConfigChange(message);
        break;
    default:
        break;
    }
}

void DemoModeProcess::initializeProcess()
{
    LOGD("## DemoModeProcess::initializeProcess()");
    mp_DemoModeTimer = new DemoModeTimer(*this);
    setDemoStatus(E_DEMOMODE_INIT);
}

bool DemoModeProcess::turnOnDemoMode(uint8_t timeUnit, int32_t timeValue) {
    if(getWifiStatus() != E_CONNECTED){
        LOGI("## WIFI STATE OFF, DEMO MODE CAN NOT START");
        setWifiStatus(E_DISCONNECTED);
        return false;
    }
    if(-1 == clock_gettime(CLOCK_MONOTONIC, &tstart_demo)) {
        LOGE("%s: Failed to call clock_gettime",__func__);
        demoModeClockReset();
        return false;
    }
    else {
         char d_str[32] = {0};
         LOGD("DemoModeStart() ==> clock_gettime : %ld, user_settime: %d, mTimeUnit[%s]", tstart_demo.tv_sec, timeValue, timeUnit_str[timeUnit]);
         setPropertyChar(PROPERTY_DEMOMODE_TIME, d_str);
         if(timeUnit == TIME_SEC) {
             setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, timeValue);
         }
         else {
             setPropertyInt(PROPERTY_DEMOMODE_USER_TIME, (timeValue * HOUR_TO_SEC));
        }
    }

    setDemoStatus(E_DEMOMODE_REQUEST);
    m_TimeUnit = timeUnit;
    m_RunningTime = timeValue;
    demoModeStart();
    return true;
}

void DemoModeProcess::demoModeStart() {
    LOGD("%s() mTimeUnit[%s], mRunningTime[%d]", __func__, timeUnit_str[m_TimeUnit], m_RunningTime);
    lockPowerMode();

    if(m_TimeUnit == TIME_SEC) {
        LOGV("%s : %d seconds ", __func__, time);
        m_TimerSet.startTimer(0, m_RunningTime);
    }
    else {  // TIME_HOUR
        LOGV("%s : %d hours ", __func__, time);
        m_TimerSet.startTimer(0, m_RunningTime * 3600);
    }
    setDemoStatus(E_DEMOMODE_START);
    m_ServicesMgr->getNGTPManager()->setDemoMode(true);
    LOGI("####################   DEMO MODE WAS STARTED !.   ####################");
}

bool DemoModeProcess::turnOffDemoMode() {
    if (getDemoStatus() == E_DEMOMODE_START || getDemoStatus() == E_DEMOMODE_PENDING) {
        m_TimerSet.stopTimer(0);
        demoModeStop();
    }
    else {
        LOGI("####################   DEMO MODE WAS ALREADY STOPPED.   ####################");
        return false;
    }

    return true;
}

void DemoModeProcess::demoModeStop() {
    m_ServicesMgr->getNGTPManager()->setDemoMode(false);
    demoModeClockReset();
    setDemoStatus(E_DEMOMODE_STOP);
    releasePoweMode();
    LOGI("####################   DEMO MODE WAS STOPPED.   ####################");
}

void DemoModeProcess::setDemoStatus(DemoModeStatus status) {
    LOGD("%s: set value:%d", __func__, status);
    sp<Post> post;
    m_DemoModeStatus = status;
    setPropertyInt(PROPERTY_DEMO_STATUS, m_DemoModeStatus);
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
}

DemoModeStatus DemoModeProcess::getDemoStatus(){
    return m_DemoModeStatus;
}

void DemoModeProcess::setWifiStatus(WiFiStatus status){
    m_WifiStatus = status;
    setPropertyInt(PROPERTY_DEMO_WIFI_STATUS, m_WifiStatus);
}

WiFiStatus DemoModeProcess::getWifiStatus(){
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
    char* name = reinterpret_cast<char*>(message->buffer.data());
    LOGI("Configuration data change buffer.data = %s", name);
    if(strcmp(DEMOMODE_APP_MODE_BC, name) == 0 || strcmp(DEMOMODE_APP_MODE_EC, name)) {

        bCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);
        eCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
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
void DemoModeProcess::lockPowerMode(){
    if(m_CheckPower) {
        LOGI("PowerMode already lock");
    } else {
        m_CheckPower = true;
        mp_PowerLock->acquire();
    }
}

void DemoModeProcess::releasePoweMode(){
    if(m_CheckPower){
        m_CheckPower = false;
        mp_PowerLock->release();
    } else {
        LOGI("PowerMode already release");
    }
}
