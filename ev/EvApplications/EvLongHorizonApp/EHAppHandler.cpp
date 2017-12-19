#include "EHAppHandler.h"

#define LOG_TAG "[EVT]EHAppHandler"
#include "Log.h"

void EHAppHandler::handleMessage(const sp<sl::Message>& msg)
{
    switch(msg->what)
    {
        case RECV_EH_PACKET:
        {
            decodeIMCpacket(&msg->buffer);
            break;
        }

        case SEND_EH_CAN_TX:
        {
            sendEhCan(&msg->buffer);
        }

        default:
        {
            break;
        }
    }
}

void EHAppHandler::sendEhCan(const Buffer* buffer)
{
    decodeMF(buffer);
    decodeSF(buffer);
    LOGV("Decode IMC Long Horizon Data! -- Done!");
}

void EHAppHandler::decodeIMCpacket(const Buffer* buffer)
{
    LOGV("Start Decode IMC Long Horizon Data! -- 4th Aug Fixed");

    uint8_t _protocolVersion = (buffer->data())[0];
    LOGV("protocolVersion:%d, identifier:%d", _protocolVersion, mEHApp.m_uFrameCnt);

    if( IsValidPacket(buffer) == false)
    {
        LOGE("[Failure] IMC pakcet is worng.");
        return;
    }

    EvtVifManager::GetInstance()->cancelCanMultiSignal_EH(mEHApp.m_uFrameCnt);
    mEHApp.increaseFrameCnt();
    sp<sl::Message> message = this->obtainMessage(/*What*/SEND_EH_CAN_TX);
    message->buffer.setTo(*buffer); // copy to message buffer.
    // Delay after discard request for VIF process. (150ms)
    this->sendMessageDelayed(/*Message*/message, /*DelayTime*/150);
}

bool EHAppHandler::IsValidPacket(const Buffer* buffer)
{
    uint32_t totalSize = buffer->size();

    uint32_t protocolVersion = EvConfigData::GetInstance()->getConfigData("LH_PROTOCOL_VER");
    if(protocolVersion > 0xFF)
    {
        LOGE("[Error] LH_PROTOCOL_VER[%d] is not valid.", protocolVersion);
        return false;
    }

    if(protocolVersion != (buffer->data())[0])
    {
        LOGE("[Error] The packet header is not matched with LH_PROTOCOL_VER[%d].", protocolVersion);
        return false;
    }

    if(totalSize < 4)
    {
        LOGE("[Error] The eHorizon Buffer is wrong. #There is no data for MatrixFrame and SegmentFrame.");
        return false;
    }

    const uint8_t* buf = buffer->data();
    const uint16_t _matrixFrameCnt = (buf[1] << 8) + buf[2]; //SRD9 5.9 Example: Long Horizon data

    if(totalSize < 3/*header*/ + _matrixFrameCnt*8)
    {
        LOGE("[Error] The eHorizon Buffer is wrong. #Total size is less than the _matrixFrameCnt size of header defined.");
        return false;
    }

    if(totalSize < 3/*header*/ + _matrixFrameCnt*8 + 2 /*Segment Frame count header*/)
    {
        LOGE("[Error] The eHorizon Buffer is wrong. #There is no data for SegmentFrame.");
        return false;
    }

    const uint8_t* newBuf = buf + 3 + _matrixFrameCnt*8; // newBuf = buf + offset
    const uint16_t _segmentFrameCnt = (newBuf[0] << 8) + newBuf[1];

    if(totalSize != 3/*header*/ + _matrixFrameCnt*8 + 2 + _segmentFrameCnt*8)
    {
        LOGE("[Error] The eHorizon Buffer is wrong. Received data size:%d is not matched Calculated data size:%d",
             totalSize,
             3/*header*/ + _matrixFrameCnt*8 + 2 /*Segment Frame count header*/ + _segmentFrameCnt*8);
        return false;
    }

    LOGI("[Packet validation Done] Total Size:%d MatrixFrame:%dea SegmentFrame:%dea identifier:%d", totalSize, _matrixFrameCnt, _segmentFrameCnt, mEHApp.m_uFrameCnt);
    return true;
}

//decode Matrix Frame
void EHAppHandler::decodeMF(const Buffer* buffer)
{
    uint8_t* buf = buffer->data();
    uint16_t _matrixFrameCnt = (buf[1] << 8) + buf[2]; //SRD9 5.9 Example: Long Horizon data
    LOGV("_matrixFrameCnt : 0x%04X [%d]", _matrixFrameCnt, _matrixFrameCnt);

    // Prevent stack over flow.
    if(_matrixFrameCnt >= buffer->size())
    {
        LOGE("[ERROR] The eHorizon Buffer is wrong. _matrixFrameCnt[%d] is bigger than Buffer size[%d]. #STOP EH processing.#", _matrixFrameCnt, buffer->size());
        return;
    }

    std::vector<MatrixFrame> v_MatrixFrames;
    uint32_t index = 3;

    for(int k=0 ; k < _matrixFrameCnt ; k++)
    {
        MatrixFrame tmp;
        tmp._msgCount   = (buf[index+0] << 2) + ((buf[index+1] & 0xC0) >> 6);
        tmp._speed      = buf[index+1] & 0x3F;
        tmp._gradient1  = (buf[index+2] & 0xF0) >> 4;
        tmp._length1    = ((buf[index+2] & 0x0F) << 8) + buf[index+3];
        tmp._gradient2  = (buf[index+4] & 0xF0) >> 4;
        tmp._length2    = ((buf[index+4] & 0x0F) << 8) + buf[index+5];
        tmp._gradient3  = (buf[index+6] & 0xF0) >> 4;
        tmp._length3    = ((buf[index+6] & 0x0F) << 8) + buf[index+7];
        LOGV("[Index %d][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", k+1, buf[index+0],buf[index+1],buf[index+2],buf[index+3],buf[index+4],buf[index+5],buf[index+6],buf[index+7]);
        v_MatrixFrames.push_back(tmp);
        tmp.toString(); // for debugging
        index = index + 8;
    }

    mEHApp.sendMatrixData(v_MatrixFrames);
}

//decode Segment Frame.
void EHAppHandler::decodeSF(const Buffer* buffer)
{
    uint8_t* buf = buffer->data();
    uint16_t _matrixFrameCnt = (buf[1] << 8) + buf[2]; //SRD9 5.9 Example: Long Horizon data
    const uint8_t* newBuf = buf + 3 + _matrixFrameCnt*8; // newBuf = buf + offset
    uint16_t _segmentFrameCnt = (newBuf[0] << 8) + newBuf[1];
    LOGV("_segmentFrameCnt : 0x%04X [%d]", _segmentFrameCnt, _segmentFrameCnt);
    if(_matrixFrameCnt + _segmentFrameCnt >= buffer->size())
    {
        LOGE("[ERROR] The eHorizon Buffer is wrong. (_segmentFrameCnt+_matrixFrameCnt)[%d] is bigger than Buffer size[%d]. #STOP EH processing.#", _segmentFrameCnt + _matrixFrameCnt, buffer->size());
        return;
    }

    std::vector<SegmentFrame> v_SegmentFrames;
    uint32_t index = 2;

    for(int k=0 ; k < _segmentFrameCnt ; k++)
    {
        SegmentFrame tmp;
        tmp._msgCount = (newBuf[index+0] << 2) + ((newBuf[index+1] & 0xC0) >> 6);
        tmp._identifier1 = (newBuf[index+1] & 0x3F) >> 4;
        tmp._identifier2 = (newBuf[index+1] & 0x0C) >> 2;
        tmp._identifier3 = (newBuf[index+1] & 0x03) >> 0;
        tmp._segOffset0  = (newBuf[index+2] << 2) + ((newBuf[index+3] & 0xC0) >> 6);
        tmp._segContent0 = (newBuf[index+3] & 0x3F);
        tmp._segOffset1  = (newBuf[index+4] << 2) + ((newBuf[index+5] & 0xC0) >> 6);
        tmp._segContent1 = (newBuf[index+5] & 0x3F);
        tmp._segOffset2  = (newBuf[index+6] << 2) + ((newBuf[index+7] & 0xC0) >> 6);
        tmp._segContent2 = (newBuf[index+7] & 0x3F);
        LOGV("[Index %d][%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", k+1, newBuf[index+0],newBuf[index+1],newBuf[index+2],newBuf[index+3],newBuf[index+4],newBuf[index+5],newBuf[index+6],newBuf[index+7]);
        v_SegmentFrames.push_back(tmp);
        tmp.toString(); // for debugging
        index = index + 8;
    }

    mEHApp.sendSegmentData(v_SegmentFrames);
}
