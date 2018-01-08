#include "DemoModeProcess.h"
#include "SpecialModeHandler.h"
#include "SpecialModeType.h"


uint8_t DEMOMODE_DEBUG  = 0;
static const uint8_t TIME_SEC = 0x00;
static const uint8_t TIME_HOUR = 0x01;
static char* timeUnit_str[] = {"SECOUND", "HOUR"};
static char* activeSim_str[] = {"TSIM", "PSIM"};

static int32_t demoModeChangeFlagEC = 0;
static int32_t demoModeChangeFlagBC = 0;

static uint8_t ecFlag;
static uint8_t bcFlag;

struct timespec tstart_demo = {0,};

//static bool mDiagStateCheck = false;

DemoModeProcess::DemoModeProcess() {
    LOGI("### Demomode process create!");
    mRunningTime = 0;
    mTimeUnit = TIME_HOUR;
    mDemoModeStatus = DEMOMODE_STATUS_INIT;
}

DemoModeProcess::~DemoModeProcess() {
    LOGI("~DemoModeProcess()++");

    m_TimerSet.release();
}

// implement virtual function of base class
void DemoModeProcess::handleEvent(uint32_t eventId){

}

void DemoModeProcess::handleTimerEvent(int timerId) {
    switch (timerId) {
    case 0:
        DemoModeStop();
        break;
    case 1:
        LOGD("%s() SVT RETRY REGISTER - timer expired [%d]",__func__, timerId);
//        if(bSVTRegOnDemo == false)
//            registerSVTpostReceiver();
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

void DemoModeProcess::demoModeClockReset() {

    char d_str[32] = {0,};
    sprintf(d_str, "%lu", d_str[0]);
    setPropertyChar("demoModeTime", d_str);
    setPropertyInt("demoModeUserTime", 0);
}

void DemoModeProcess::do_SpecialModeHandler(int32_t what, sp<sl::Message> &messages){
    int32_t arg1 = messages->arg1;
    int32_t arg2 = messages->arg2;
    int32_t arg3 = messages->arg3;

    if(messages->buffer.size() > 0) {
        LOGD("buff[0]=%x buff[1]=%x buff[2]=%x buff[3]=%x",*(messages->buffer.data()),*(messages->buffer.data()+1),
        *(messages->buffer.data()+2),*(messages->buffer.data()+3));
    }

    switch (what) {
    case DEMOMODE_SLDD_ON:
        TurnOnDemoMode((uint8_t)arg2, arg1);
        break;
    case DEMOMODE_SLDD_OFF:
        TurnOffDemoMode();
        break;
    case DEMOMODE_SLDD_TIME_UNIT:
        mTimeUnit = (uint8_t)arg1;
        break;

    case DEMOMODE_SLDD_LOG_ON:
        DEMOMODE_DEBUG = 1;
        break;

    case DEMOMODE_SLDD_LOG_OFF:
        DEMOMODE_DEBUG = 0;
        break;

    case DEMOMODE_SLDD_CMD:  // for test
        break;

    default:
        break;
    }
}

void DemoModeProcess::initializeProcess() {
    LOGD("## DemoModeProcess::initializeProcess()");
    mp_DemoModeTimer = new DemoModeTimer(*this);
    m_TimerSet.initialize(mp_DemoModeTimer);

    demoModeClockReset();
    setDemoModeStatus(DEMOMODE_STATUS_INIT);

}

bool DemoModeProcess::TurnOnDemoMode(uint8_t timeUnit, int32_t m_time) {

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

    setDemoModeStatus(DEMOMODE_STATUS_REQUEST);
    mTimeUnit = timeUnit;
    mRunningTime = m_time;

    DemoModeStart();

    return true;
}

void DemoModeProcess::DemoModeStart() {
    if(mTimeUnit == TIME_SEC) {   // unit = second (only for test)
        LOGV("%s : %d seconds ", __func__, time);
        m_TimerSet.startTimer(0, mRunningTime);
    }
    else {
        LOGV("%s : %d hours ", __func__, time);
        m_TimerSet.startTimer(0, mRunningTime * 3600); // unit:Hours
    }

    setDemoModeStatus(DEMOMODE_STATUS_START);
    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>(defaultServiceManager()
                                                                        ->getService(String16("service_layer.NGTPManagerService")));
    iNGTP->setDemoMode(true);
    LOGI("###################################################################");
    LOGI("####################   DemoMode was started.   ####################");
    LOGI("###################################################################");

//    return true;
}

bool DemoModeProcess::TurnOffDemoMode() {
    if (getDemoModeStatus() == DEMOMODE_STATUS_START || getDemoModeStatus() == DEMOMODE_STATUS_PENDING) {
        m_TimerSet.stopTimer(0);
        DemoModeStop();
    }
    else {
        LOGI("###################################################################");
        LOGI("####################   DemoMode already stop.   ####################");
        LOGI("###################################################################");
        return false;
    }

    return true;
}

void DemoModeProcess::DemoModeStop() {

    sp<INGTPManagerService> iNGTP = interface_cast<INGTPManagerService>(defaultServiceManager()
                                                                        ->getService(String16("service_layer.NGTPManagerService")));
    iNGTP->setDemoMode(false);
    demoModeClockReset();
    setDemoModeStatus(DEMOMODE_STATUS_STOP);
    LOGI("###################################################################");
    LOGI("####################   DemoMode was stopped.   ####################");
    LOGI("###################################################################");
}

void DemoModeProcess::setDemoModeStatus(DEMO_MODE_STATUS status) {
    LOGD("%s: set value:%d", __func__, status);
    sp<Post> post;
    sp<Post> postSVT;

    mDemoModeStatus = status;
    setPropertyInt(PROPERTY_DEMO_STATUS, mDemoModeStatus);

//    switch(status)
//    {
//    case DEMOMODE_STATUS_START:
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_START);
//        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
//        postSVT = Post::obtain((appid_t)APP_ID_SPECIAL, mDemoModeStatus);
//        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, postSVT);
//        break;
//    case DEMOMODE_STATUS_STOP:
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_END);
//        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
//        postSVT = Post::obtain((appid_t)APP_ID_SPECIAL, mDemoModeStatus);
//        m_ServicesMgr->getApplicationManager()->sendPost((appid_t)APP_ID_SVT, postSVT);
//        break;
//    case DEMOMODE_STATUS_PENDING:
//        post = Post::obtain(SYS_POST_DEMO_MODE, SYS_POST_DEMO_MODE_PENDING);
//        m_ServicesMgr->getApplicationManager()->broadcastSystemPost(post);
//        break;

//    default:
//        break;
//    }
}

DEMO_MODE_STATUS DemoModeProcess::getDemoModeStatus(){
    return mDemoModeStatus;
}


void DemoModeProcess::sendResponse(int32_t what, const sp<sl::Message> &messages) {
    sp<Buffer> buf = new Buffer();
    uint8_t responsePayload[DIAG_RESPONE_SIZE];
    uint8_t responsePayload_1[DIAG_RESPONE_SIZE-1];

}




























