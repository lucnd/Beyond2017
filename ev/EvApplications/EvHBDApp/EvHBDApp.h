#ifndef _EV_HBD_APP_H_
#define _EV_HBD_APP_H_
#include <map>
#include <utils/RefBase.h>
#include "Handler.h"
#include "Message.h"
#include "SLLooper.h"
#include "Timer.h"

#include "EvtUtil/EvState.h"
// #include "EvtUtil/EvtUtil.h"
#include "EvtHvBatteryPack.h"

using namespace sl;

class EvHBDApp : public EvtSingleton<EvHBDApp>, EvState
{
public:
    EvHBDApp();
    ~EvHBDApp(){};

    void onStart();

    bool rcv_CAN_Signals(sp<Buffer>& buf);
    void rcv_CAN_Frame(uint16_t FrameID);

    enum HBDStatus{
    HBD_OFF = 0,
    HBD_ON,
    HBD_OPER_RECORD_AND_UPDATE,
    HBD_STOP_RECORD,
    HBD_OPER_RECORD_ONLY,
    HBD_OTHER,
    HBD_END_ENUM // This should be at the end of enum.
    };

    enum{
    HBD_CAN_READ_TIMER  = 0,
    HBD_CAN_BUFFER_TIMER,
    HBD_BUFFER_MAX_TIMER,
    HBD_TCU_PUSH_TIMER,
    HBD_END_TIMER
    };

    void resetTimer(uint8_t timerName);

    //TimeoutHandler Function
    void timeout_HBD_CAN_READ_TIMER();
    void timeout_HBD_CAN_BUFFER_TIMER();
    void timeout_HBD_BUFFER_MAX_TIMER();
    void timeout_HBD_TCU_PUSH_TIMER();

    void determineHBDOperation(uint32_t toState);

    unsigned char getState();

private:
    sp<SLLooper> m_Looper;
    sp<Handler> mHandler;

    Timer* HBD_Timers[HBD_END_TIMER];

    //Timer Functions
    virtual void toStringState(unsigned char state, unsigned char newState);
    void setTimer(uint8_t timerName, uint16_t expireTime, bool isMsTime = false);
    uint32_t getRemainTimerTime(uint8_t timerName);
    void cancelTimer(uint8_t timerName);
    void cancelAllTimer();

    int32_t convertDayToSec(int32_t valueDays);

    // check Data Sampling Condition
    bool isTimeToDataRecord(uint16_t curPM, uint32_t isState, uint32_t toState);
    bool isTimeToUpdateToTSP();

    bool bTimeToDataRecord;
    bool bTimeToUpdateTSP;

    //MAP <-> Function pointer
    typedef void (*FrameFunc)();
    typedef std::map<uint16_t, FrameFunc > MAP_FUNC_LIST;
    MAP_FUNC_LIST m_mapFrameFunc;
};
#endif //_EV_HBD_APP_H_
