#include <climits>
#include "EHApp.h"
#include "EHAppHandler.h"

#define LOG_TAG "[EVT]LongHorizon"
#include "Log.h"
#include "EvtUtil.h"

const int MAX_SESSION_COUNT = 10;

EHApp::EHApp()
{
    //start Handler
    m_Looper = SLLooper::myLooper();
    mHandler = new EHAppHandler(m_Looper, *this);
    m_uFrameCnt = 1;
}

void EHApp::onStart()
{
    if(EvVehicleStatus::GetInstance()->getVehicleType() != EvVehicleStatus::PHEV)
    {
        LOGE("Long Horizon application shall be active when vehicle type is PHEV and Long Horizon application isprovisioned (as described in ReqEH0020 and ReqEH0050)");
        LOGI("End eHorizon Application. Bye JLR~!");
        return;
    }
    //Server Therad start!
    mReceiverThread = new EHAppServer(mHandler);
    mReceiverThread->run();

    LOGI("EHApp boot complete!");
}

bool EHApp::DBG_EH_Data()
{
    //TODO : Change Debug command ---> Requirement had been changed.
    LOGI("###### Start EH TEST using Sldd #####");
    std::vector<uint8_t> _debug_Data = EvConfigData::GetInstance()->debug_EH_Data;

    sp<sl::Message> message = mHandler->obtainMessage(RECV_EH_PACKET);
    message->buffer.setTo(&(_debug_Data[0]), _debug_Data.size()); // copy to message buffer.
    message->buffer.dump();
    message->sendToTarget();
    return true;
}

//------------------------------------------Private Function------------------------------------------//
void EHApp::sendMatrixData(const std::vector<MatrixFrame> &v_MatrixFrames)
{
    for( MatrixFrame value : v_MatrixFrames )
    {
        sp<vifCANContainer> _canContainer = new vifCANContainer();
        pushSingleData(_canContainer, Signal_MatrixCounter_TX,      value._msgCount);
        pushSingleData(_canContainer, Signal_MatrixAverageSpeed_TX, value._speed);
        pushSingleData(_canContainer, Signal_MatrixGradient1_TX,    value._gradient1);
        pushSingleData(_canContainer, Signal_MatrixLength1_TX,      value._length1);
        pushSingleData(_canContainer, Signal_MatrixGradient2_TX,    value._gradient2);
        pushSingleData(_canContainer, Signal_MatrixLength2_TX,      value._length2);
        pushSingleData(_canContainer, Signal_MatrixGradient3_TX,    value._gradient3);
        pushSingleData(_canContainer, Signal_MatrixLength3_TX,      value._length3);
        _canContainer->numbering = m_uFrameCnt;
        EvtVifManager::GetInstance()->sendCanMultiSignal_EH(_canContainer->NumofSignals, _canContainer);
    }
}

void EHApp::sendSegmentData(const std::vector<SegmentFrame> &v_SegmentFrames)
{
    for( SegmentFrame value : v_SegmentFrames )
    {
        sp<vifCANContainer> _canContainer = new vifCANContainer();
        pushSingleData(_canContainer, Signal_EHMessageCounter_TX,       value._msgCount);
        pushSingleData(_canContainer, Signal_EHSegmentIdentifier1_TX,   value._identifier1);
        pushSingleData(_canContainer, Signal_EHSegmentIdentifier2_TX,   value._identifier2);
        pushSingleData(_canContainer, Signal_EHSegmentIdentifier3_TX,   value._identifier3);
        pushSingleData(_canContainer, Signal_EHSegmentOffset1_TX,       value._segOffset0);
        pushSingleData(_canContainer, Signal_EHSegmentContent1_TX,      value._segContent0);
        pushSingleData(_canContainer, Signal_EHSegmentOffset2_TX,       value._segOffset1);
        pushSingleData(_canContainer, Signal_EHSegmentContent2_TX,      value._segContent1);
        pushSingleData(_canContainer, Signal_EHSegmentOffset3_TX,       value._segOffset2);
        pushSingleData(_canContainer, Signal_EHSegmentContent3_TX,      value._segContent2);
        _canContainer->numbering = m_uFrameCnt;
        EvtVifManager::GetInstance()->sendCanMultiSignal_EH(_canContainer->NumofSignals, _canContainer);
    }
}

error_t EHApp::sendCanSignal(int sigID, const uint8_t data)
{
    LOG_EV("Signal[0x%02X][%s] \t\t=[%d]", sigID, EvtVifManager::GetInstance()->getCanName(sigID), data); // ONLY for debug.
    return EvtVifManager::GetInstance()->sendCanSignal(sigID, data);
}

error_t EHApp::sendCanSignal(int sigID, const uint8_t data[])
{
    return EvtVifManager::GetInstance()->sendCanArraySignal(sigID, data);
}

void EHApp::pushSingleData(sp<vifCANContainer>& sigdata, int sigID, uint32_t data)
{
    EvtVifManager::GetInstance()->pushSingleData(sigdata, sigID, data);
}

void EHApp::setByteDataFromInterger(uint8_t _byteData[], uint32_t _intData, uint8_t size)
{
    EvtVifManager::GetInstance()->setByteDataFromInterger(_byteData, _intData, size);
}

void EHApp::increaseFrameCnt()
{
    if(m_uFrameCnt == ULONG_MAX)
    {
        m_uFrameCnt = 1;
    }
    else
    {
        m_uFrameCnt++;
    }
}