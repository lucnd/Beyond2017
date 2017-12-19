#include <vector>
#include "EHAppServer.h"
#include "EvtUtil.h"
#include "EHAppHandler.h"

#define LOG_TAG "[EVT]EHAppServer"
#include "Log.h"

#define MAXBUF 70000
#define DEFAULT_ADDR "192.168.7.1"
#define DEFAULT_PORT 50020

EHAppServer::~EHAppServer()
{

}

bool EHAppServer::threadLoop()
{
    std::string _addr = getEhAddr();
    uint16_t _port = getEhPort();
    LOGE("EH Server Start : %s:%d ",_addr.c_str(), _port );

    if(_addr.compare("192.168.107.41") == 0)
    {
        _addr = DEFAULT_ADDR;
    }

    return run(_addr.c_str(), _port);
}

bool EHAppServer::run(const char* ip, uint16_t port)
{
    Address addr(ip, port);
    sock.setSocket();
    if (!sock.bind(&addr))
        return false;

    while (1)
    {
        uint8_t* buf = new uint8_t[MAXBUF];
        if(buf == NULL)
        {
            LOGE("Memory Allocation Failure.");
            break;
        }
        memset(buf, 0, MAXBUF);

        //Socket Listening status
        sock.listen(1);
        sock.accept();

        //Socket Receive Status.
        uint32_t rcvSize = sock.receive(buf);
        if(rcvSize > 0)
        {
            cancelPreviousAction();
            /// For Debug log. ///
            sp<Buffer> tBuf = new Buffer();
            tBuf->setTo(buf, rcvSize);
            tBuf->dump();
            //////////////////////
            sp<sl::Message> message = mHandler->obtainMessage(RECV_EH_PACKET);
            message->buffer.setTo(buf, rcvSize); // copy to message buffer.
            message->sendToTarget();
        }
        else
        {
            LOGE("[ERROR]Nothing receive from IMC..");
        }
        delete buf;
    }

    sock.close();
    return true;
}

void EHAppServer::cancelPreviousAction()
{
    if(mHandler->hasMessages(RECV_EH_PACKET) == true)
    {
        mHandler->removeMessages(RECV_EH_PACKET);
    }
}

bool IsValidIpAddress(const string ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    /*
    inet_pton - convert IPv4 and IPv6 addresses from text to binary form
    Return value
    If no error occurs, the InetPton function returns a value of 1 and the buffer pointed to by the pAddrBuf parameter contains the binary numeric IP address in network byte order.
    The InetPton function returns a value of 0 if the pAddrBuf parameter points to a string that is not a valid IPv4 dotted-decimal string or a valid IPv6 address string.
    Otherwise, a value of -1 is returned, and a specific error code can be retrieved by calling the WSAGetLastError for extended error information.
    If the function has an error, the extended error code returned by WSAGetLastError can be one of the following values.
    */
    if(result == 1)
    {
        LOGI("TCU_LH_IPADDRESS[%s] is ok. corret IPv4 Addr Type.", ipAddress.c_str());
        return true;
    }
    else
    {
        LOGE("[ERROR] TCU_LH_IPADDRESS[%s] is not correct IPv4 Addr Type. Recover as [192.168.107.41]", ipAddress.c_str());
        return false;
    }
}

std::string EHAppServer::getEhAddr()
{
    std::string _addr = EvConfigData::GetInstance()->getConfigDataAsString("TCU_LH_IPADDRESS");
    // validation check
    if(!IsValidIpAddress(_addr))
    {
        return DEFAULT_ADDR;
    }

    if(_addr.compare("192.168.107.41") != 0)
    {
        LOGE("[WARN] Reference NGI-SWRS-Network Requirements : 4.2.1 Ethernet network part!\n TCU3::EVT only allow [192.168.107.41] ip address.\n TCU3::EVT is not ensure that it is working using the address of [%s].",_addr.c_str());
    }
    return _addr;
}

uint16_t EHAppServer::getEhPort()
{
    int32_t _port = EvConfigData::GetInstance()->getConfigData("TCU_LH_PORT");

    // validation check
    if(_port == -1 || _port < 49152 || _port > 65535)
    {
        LOGE("[ERROR] TCU_LH_PORT[%d] is not correct IPv4 Port Type. Recover as %d", _port, DEFAULT_PORT);
        return (uint16_t) DEFAULT_PORT;
    }

    if((_port >= 50000 && _port <= 50010) || _port == 50018 || _port == 55555)
    {
        LOGE("[WARN] Reference NGI-SWRS-Network Requirements : 4.5 Reserved Port\n TCU3::EVT is not ensure that it is working using the port of [%d]", _port);
    }
    return (uint16_t)_port;
}

void MatrixFrame::toString()
{
    LOG_EV("_msgCount : %d", _msgCount );
    LOG_EV("_speed    : %d", _speed );
    LOG_EV("_gradient1: %d", _gradient1 );
    LOG_EV("_length1  : %d", _length1 );
    LOG_EV("_gradient2: %d", _gradient2);
    LOG_EV("_length2  : %d", _length2 );
    LOG_EV("_gradient3: %d", _gradient3);
    LOG_EV("_length3  : %d", _length3 );
}

void SegmentFrame::toString()
{
    LOG_EV("_msgCount : %d", _msgCount );
    LOG_EV(" _identifier1 : %d", _identifier1 );
    LOG_EV(" _identifier2 : %d", _identifier2 );
    LOG_EV(" _identifier3 : %d", _identifier3 );
    LOG_EV(" _segOffset0  : %d", _segOffset0 );
    LOG_EV(" _segContent0 : %d", _segContent0 );
    LOG_EV(" _segOffset1  : %d", _segOffset1 );
    LOG_EV(" _segContent1 : %d", _segContent1 );
    LOG_EV(" _segOffset2  : %d", _segOffset2 );
    LOG_EV(" _segContent2 : %d", _segContent2 );
}