#ifndef _EV_RANGE_APP_H_
#define _EV_RANGE_APP_H_

#include <utils/RefBase.h>
#include "EvServiceManager.h"
#include "EvtChargeSettings.h"
#include "EvtElectricVehicleInformation.h"
#include "EvtUtil/EvState.h"
#include "EvtUtil/EvtUtil.h"

class EvRangeApp : public EvtSingleton<EvRangeApp>, EvState
{
public:
    EvRangeApp();
    ~EvRangeApp(){};

    void onStart();
    void initialSetupStart();
    void initialSetupEnd();
    bool rcv_CAN_Signals(sp<Buffer>& buf);
    void rcv_CAN_Frame(uint16_t FrameID);

private:
    void setTimer(uint8_t timerName, uint16_t expireTime);
    virtual void toStringState(unsigned char state, unsigned char newState);
    sp<SLLooper> m_Looper;
    sp<Handler> mHandler;

    //MAP <-> Function pointer
    typedef void (*FrameFunc)();
    typedef std::map<uint16_t, FrameFunc > MAP_FUNC_LIST;
    MAP_FUNC_LIST m_mapFrameFunc;

public:
    enum EVRStatus{
        EVR_OFF = 0,
        EVR_IDLE,
        EVR_END_ENUM // This should be at the end of enum.
        };
};
#endif //_EV_RANGE_APP_H_