#ifndef _EV_EHORIZON_APP_SERVER_H_
#define _EV_EHORIZON_APP_SERVER_H_

// #include "EHAppServerSession.h"
// #include "EHAppServerProtocol.h"
#include <utils/Handler.h>
#include <utils/Thread.h>
using android::Thread;

#include "EvtServer.h"

class EHApp;
class EHAppServer : public android::Thread
{
public:
    EHAppServer(sp<sl::Handler> _mHandler): sock(SOCK_STREAM), mHandler(_mHandler) {}
    virtual ~EHAppServer();

private:
    virtual bool threadLoop();

    EHApp* m_EHApp;
    std::string buf;
    sp<sl::Handler> mHandler;

    Socket sock;
    bool run(const char* ip, uint16_t port);
    void decodeIMCPacket(const uint8_t* buf);
    void decodeMF(const uint8_t* buf);
    void decodeSF(const uint8_t* buf);

    std::string getEhAddr();
    uint16_t getEhPort();

    void cancelPreviousAction();
};

class MatrixFrame
{
public:
    uint16_t _msgCount;
    uint8_t _speed;
    uint8_t _gradient1;
    uint16_t _length1;
    uint8_t _gradient2;
    uint16_t _length2;
    uint8_t _gradient3;
    uint16_t _length3;

    void toString();
};

class SegmentFrame
{
public:
    uint16_t _msgCount;
    uint8_t _identifier1;
    uint8_t _identifier2;
    uint8_t _identifier3;
    uint16_t _segOffset0;
    uint8_t _segContent0;
    uint16_t _segOffset1;
    uint8_t _segContent1;
    uint16_t _segOffset2;
    uint8_t _segContent2;

    void toString();
};

#endif //_EV_EHORIZON_APP_SERVER_H_