#define LOG_TAG "SpecialModeApplication"
#include "DemoModeProcess.h"
#include "SpecialModeHandler.h"
#include "SpecialModeType.h"
#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/NGTPManagerService/INGTPManagerService.h>

#include "ServiceList.h"


uint8_t DEMOMODE_DEBUG  = 0;
static const uint8_t TIME_SEC = 0x00;
static const uint8_t TIME_HOUR = 0x01;
static char* timeUnit_str[] = {"SECOND", "HOUR"};
static char* activeSim_str[] = {"TSIM", "PSIM"};
static int32_t demoModeChangeFlagEC = 0;
static int32_t demoModeChangeFlagBC = 0;
static uint8_t eCallFlag;
static uint8_t bCallFlag;
struct timespec tstart_demo = {0,};

DemoModeProcess::DemoModeProcess() : mp_PowerLock(NULL) {
    LOGI("## DemoModeProcess create.");
    m_RunningTime = 0;
    m_TimeUnit = TIME_HOUR;
    m_DemoModeStatus = E_DEMOMODE_INIT;
    m_WifiStatus =  E_CONNECTED;

//    mp_PowerLock = new PowerLock(APP_ID_SPECIAL);
    mp_PowerLock = new PowerLock(getpid());
}

DemoModeProcess::~DemoModeProcess() {
    LOGI("~DemoModeProcess()++");
    checkBcallStatus();
    checkEcallStatus();
    m_TimerSet.release();
}

void DemoModeProcess::handleEvent(uint32_t eventId){
    LOGV("%s()",__func__ );
}

void DemoModeProcess::handleTimerEvent(int timerId) {
    switch (timerId) {
    case 0:
    {
         LOGD("%s() DEMOMODE DEACTIVE [%d]",__func__, timerId);
          demoModeStop();
        break;
    }

    case 1:
    {
//  TODO prepare for phase 2
//        LOGD("%s() SVT RETRY REGISTER - timer expired [%d]",__func__, timerId);
//        if(bSVTRegOnDemo == false)
//            registerSVTpostReceiver();
    }
        break;
    default:
        break;
    }
}

int DemoModeProcess::byteToInt1(byte data[], int idx){
    return
    (
        ((data[idx+3] & 0xFF) << 24) |
        ((data[idx+2] & 0xFF) << 16) |
        ((data[idx+1] & 0xFF) << 8) |
        (data[idx] & 0xFF)
    );
}

void DemoModeProcess::demoModeClockReset() {

    char d_str[32] = {0,};
    sprintf(d_str, "%lu", d_str[0]);
    setPropertyChar("demoModeTime", d_str);
    setPropertyInt("demoModeUserTime", 0);
}

void DemoModeProcess::doSpecialModeHandler(int32_t what,const sp<sl::Message>& message){
    int32_t arg1 = message->arg1;
    int32_t arg2 = message->arg2;
//    int32_t arg3 = message->arg3;
//    DLOGD("doSpecialModeHandler() what[%d],arg1[%d],arg2[%d],arg3[%d],buf size[%d]",what,arg1,arg2,arg3,message->buffer.size());

//    hard code agr message
//      uint8_t arg1 = 0x1E;  // time value (30s)
//      int32_t arg2 = 0;     // time unit (0 = second, 1 = hour)

    if(message->buffer.size() > 0) {
        LOGD("buff[0]=%x buff[1]=%x buff[2]=%x buff[3]=%x",*(message->buffer.data()),*(message->buffer.data()+1),
        *(message->buffer.data()+2),*(message->buffer.data()+3));
    }

    switch (what) {
    case DEMOMODE_SLDD_ON:
        LOGD("DEMOMODE_SLDD_ON");
        turnOnDemoMode((uint8_t)arg2, arg1);
        break;
    case DEMOMODE_SLDD_OFF:
        LOGD("DEMOMODE_SLDD_OFF");
        turnOffDemoMode();
        break;
    case DEMOMODE_SLDD_TIME_UNIT:
        LOGD("DEMOMODE_SLDD_TIME_UNIT mTimeUnit[%s]", timeUnit_str[(uint8_t)arg1]);
        m_TimeUnit = (uint8_t)arg1;
        break;

    case DEMOMODE_SLDD_LOG_ON:
        DEMOMODE_DEBUG = 1;
        break;

    case DEMOMODE_SLDD_LOG_OFF:
        DEMOMODE_DEBUG = 0;
        break;

    case DEMOMODE_SLDD_CMD:
        break;

    case DEMOMODE_SLDD_DEMO_STATUS:
        LOGD("DEMO MODE STATUS = %s",getDemoStatus());
        break;
    case RECV_MSG_FROM_WIFI_ON:
        LOGI("## Wifi status is changing:\n "
             "Before: E_DISCONNECTED \n "
             "After: E_CONNECTED");

        setWifiStatus(E_CONNECTED);
        if(getDemoStatus() == E_DEMOMODE_PENDING) {
            setDemoStatus(E_DEMOMODE_START);
        }
           break;
    case RECV_MSG_FROM_WIFI_OFF:
        LOGI("## Wifi status is changing:\n "
             "Before: E_CONNECTED \n "
             "After: E_DISCONNECTED");
        setWifiStatus(E_DISCONNECTED);
        if(getDemoStatus() == E_DEMOMODE_START) {
            setDemoStatus(E_DEMOMODE_PENDING);
        }
        break;

    default:
        break;
    }
}

void DemoModeProcess::initializeProcess()
{
    LOGD("## DemoModeProcess::initializeProcess()");

    mp_DemoModeTimer = new DemoModeTimer(*this);
    m_TimerSet.initialize(mp_DemoModeTimer);
    demoModeClockReset();
    setDemoStatus(E_DEMOMODE_INIT);
}
bool DemoModeProcess::turnOnDemoMode(uint8_t timeUnit, int32_t timeValue) {

    // check wifi condition
    if(getWifiStatus() != E_CONNECTED){
        LOGI("## WIFI STATE IS DISCONNECTED");
        LOGI("## DEMO MODE CAN NOT START");
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
}

void DemoModeProcess::demoModeStart() {
    LOGD("%s() mTimeUnit[%s], mRunningTime[%d]", __func__, timeUnit_str[m_TimeUnit], m_RunningTime);
    // TODO prepare phase 2
    lockPowerMode();

     if(m_TimeUnit == TIME_SEC) {
         LOGV("%s : %d seconds ", __func__, time);
         m_TimerSet.startTimer(0, m_RunningTime);        // unit:Seconds (for only TEST)
     }
     else {
         LOGV("%s : %d hours ", __func__, time);
         m_TimerSet.startTimer(0, m_RunningTime * 3600); // unit:Hours
     }
     setDemoStatus(E_DEMOMODE_START);
    // TODO phase 2
    // setDisableEbcallStatus();

     disableBCall();
     disableECall();

    // TODO wait for NGTP service ready
     sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>
                  (defaultServiceManager()->getService(String16("service_layer.NGTPManagerService")));
     iNGTP->setDemoMode(true);

    LOGI("###################################################################");
    LOGI("####################   DEMO MODE WAS STARTED !.   ####################");
    LOGI("###################################################################");

}

bool DemoModeProcess::turnOffDemoMode() {
    if (getDemoStatus() == E_DEMOMODE_START || getDemoStatus() == E_DEMOMODE_PENDING) {
        m_TimerSet.stopTimer(0);
        demoModeStop();
    }
    else {
        LOGI("###################################################################");
        LOGI("####################   DEMO MODE WAS ALREADY STOPPED.   ####################");
        LOGI("###################################################################");
        return false;
    }

    return true;
}

void DemoModeProcess::demoModeStop() {

    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>
                 (defaultServiceManager()->getService(String16("service_layer.NGTPManagerService")));
    iNGTP->setDemoMode(false);

    demoModeClockReset();
    setDemoStatus(E_DEMOMODE_STOP);
    releasePoweMode();


    LOGI("###################################################################");
    LOGI("####################   DEMO MODE WAS STOPPED.   ####################");
    LOGI("###################################################################");
}

void DemoModeProcess::setDemoStatus(DemoModeStatus status) {
    LOGD("%s: set value:%d", __func__, status);
    sp<Post> post;
    sp<Post> postSVT;
    m_DemoModeStatus = status;

    switch(status)
    {
    case E_DEMOMODE_START:

          LOGD("## broadcast post:: DEMO MODE START");
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_START);
//        m_AppMgr->broadcastSystemPost(post);
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_START);
//        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
//        postSVT = Post::obtain((appid_t)APP_ID_SPECIAL, mDemoModeStatus);
//        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, postSVT);
        break;
    case E_DEMOMODE_STOP:

          LOGD("## broadcast post:: DEMO MODE END");
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_END);
//        m_AppMgr->broadcastSystemPost(post);
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_END);
//        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
//        postSVT = Post::obtain((appid_t)APP_ID_SPECIAL, mDemoModeStatus);
//        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, postSVT);
        break;

    case E_DEMOMODE_PENDING:
          LOGD("## broadcast post:: DEMO MODE PENDING");
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_START);
//        m_AppMgr->broadcastSystemPost(post);
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_PENDING);
//        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
        break;

    default:
        break;
    }
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

void DemoModeProcess::checkBcallStatus() {
    bCallFlag = m_ServicesMgr ->getConfigurationManager()
            ->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);
    demoModeChangeFlagBC = sl::atoi(getPropertyWrap(PROPERTY_CHANGE_FLAG_BC));

    LOGI("%s() bCallFlag property: %d", __func__, bCallFlag);
    LOGI("%s() demoChangeFlagBC property: %d", __func__, demoModeChangeFlagBC);

    if(demoModeChangeFlagBC == 1) {
        if(bCallFlag == 0)
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC,1);
        else
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC,0);
        setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 0);
    }
}

void DemoModeProcess::checkEcallStatus() {
    eCallFlag = m_ServicesMgr ->getConfigurationManager()
            ->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);
    demoModeChangeFlagEC = sl::atoi(getPropertyWrap(PROPERTY_CHANGE_FLAG_EC));

    LOGI("%s() bCallFlag property: %d", __func__, eCallFlag);
    LOGI("%s() demoChangeFlagEC property: %d", __func__, demoModeChangeFlagEC);

    if(demoModeChangeFlagEC == 1) {
        if(eCallFlag == 0)
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC,1);
        else
            m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC,0);
        setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 0);
    }
}

void DemoModeProcess::disableBCall() {
    bCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC);

    LOGI("%s() Check bcall status: bCallFlag=%d", __func__, bCallFlag);

    if (bCallFlag == 1) {
        LOGI("################# BCALL DISABLED !!#################");
        setPropertyInt(PROPERTY_CHANGE_FLAG_BC, 1);
        m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_BC, 0);
    }
}

void DemoModeProcess::disableECall() {
    eCallFlag = m_ServicesMgr->getConfigurationManager()->get_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC);

    LOGI("%s() Check ecall status: eCallFlag=%d", __func__, eCallFlag);

    if (eCallFlag == 1) {
        LOGI("################# ECALL DISABLED !!#################");
        setPropertyInt(PROPERTY_CHANGE_FLAG_EC, 1);
        m_ServicesMgr->getConfigurationManager()->change_set_int(SPECIALMODE_CONFIG_FILE, 2, DEMOMODE_APP_MODE_EC, 0);
    }
}

void DemoModeProcess::lockPowerMode(){
    LOGI("%s()", __func__);
    LOGI("PowerLock Status before lock: %d", m_ServicesMgr->getPowerManager()->getPowerLockStatus());
    if(m_CheckPower) {
        LOGI("PowerMode already lock");
    } else {
        m_CheckPower = true;
        mp_PowerLock->acquire();
    }
    LOGI("PowerLockStatus after lock: %d", m_ServicesMgr->getPowerManager()->getPowerLockStatus());

}

void DemoModeProcess::releasePoweMode(){
    LOGI("%s()", __func__);
    LOGI("PowerLock Status before release: %d", m_ServicesMgr->getPowerManager()->getPowerLockStatus());
    if(m_CheckPower){
        m_CheckPower = false;
        mp_PowerLock->release();
    } else {
        LOGI("PowerMode already release");
    }
    LOGI("PowerLock Status after release: %d", m_ServicesMgr->getPowerManager()->getPowerLockStatus());
}
