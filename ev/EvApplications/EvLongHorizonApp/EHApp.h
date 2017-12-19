#ifndef _EV_LONG_HORIZON_APP_H_
#define _EV_LONG_HORIZON_APP_H_

#include "EvtVifManager.h"
#include "EvConfigData.h"
#include "EvVehicleStatus.h"
#include "EvtSingleton.h"

#include <vector>
#include <array>
#include <utils/Thread.h>
#include "EHAppServer.h"

using android::Thread;

class EHApp : public EvtSingleton<EHApp>
{
public:
    EHApp();
    virtual ~EHApp(){};

    int PM_Status;
    uint32_t m_uFrameCnt;

    void onStart();

    bool DBG_EH_Data();
    void sendMatrixData(const std::vector<MatrixFrame> &v_MatrixFrames);
    void sendSegmentData(const std::vector<SegmentFrame> &v_SegmentFrames);
    void increaseFrameCnt();

private:

    error_t sendCanSignal(int sigID, const uint8_t data);
    error_t sendCanSignal(int sigID, const uint8_t data[]);
    void pushSingleData(sp<vifCANContainer>& sigdata, int sigID, uint32_t data);
    void setByteDataFromInterger(uint8_t _byteData[], uint32_t _intData, uint8_t size);

    sp<Thread> mReceiverThread;
    sp<SLLooper> m_Looper;
    sp<Handler> mHandler;
};

#endif //_EV_LONG_HORIZON_APP_H_