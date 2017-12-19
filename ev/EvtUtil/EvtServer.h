#ifndef _EVT_SERVER_H_
#define _EVT_SERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
using namespace std;

#define LOG_TAG "[EVT]Server"
#include "Log.h"

class Address
{
    //SOCKADDR_IN addr;
    sockaddr_in addr;
public:
    Address(const char* ip, uint16_t port)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
    }

    sockaddr* getRawAddress()
    {
        return (sockaddr*)&addr;
    }
};

class Socket
{
    int32_t server_sock;
    int32_t client_sock;

public:
    // PF_PACKET, PF_LOCAL, PF_INET
    Socket(int32_t type)
    {
        server_sock = socket(PF_INET, type, 0);
    }

    void setSocket()
    {
        //for KEEPALIVE.
        bool optval = true;
        setsockopt(server_sock, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(bool));
    }

    bool bind(Address* addr)
    {
        int32_t result = ::bind(server_sock, addr->getRawAddress(), sizeof(sockaddr_in));
        if (result < 0)
        {
            LOGE("[ERROR]Error binding socket! --> REASON[%d][%s]",errno, strerror(errno));
            ::close(server_sock);
        }
        return result < 0 ? false : true;
    }

    void listen(int32_t backlog)
    {
        ::listen(server_sock, backlog);
        LOGV("Listening state...");
    }

    void accept()
    {
        sockaddr_in addr2 = { 0 };
        uint32_t sz = sizeof(addr2);
        client_sock = ::accept(server_sock, (sockaddr*)&addr2, &sz);
        if(client_sock == -1)
        {
            LOGE("ERROR: %s", std::strerror(errno) );
        }
        //LOGV("Access Accepted... ok -> client[%s:%s]", inet_ntoa(addr2.sin_addr), ntohs(addr2.sin_port));
        LOGV("Accepted IMC access ... ");
    }

    // void receive(std::string &buf)
    // {
    //     buf.clear();
    //     char tmpBuf[1024] = { 0, };

    //     int32_t cnt_rcv = 0;
    //     do {
    //         cnt_rcv = ::recv(client_sock, tmpBuf, 100, 0);

    //         if (cnt_rcv < 0)
    //             LOGE("receive error -> Reason[%d][%s]", errno, strerror(errno));
    //         else if (cnt_rcv > 0)
    //             LOGV(" %d bytes has been received",cnt_rcv );

    //         buf += string(tmpBuf, cnt_rcv);
    //     } while (cnt_rcv != 0);

    //     LOGV("Total Data[%d][%s]", buf.size(), buf.c_str() );
    //     LOGE("client has disconnected!");
    // }

    uint32_t receive(uint8_t* buf)
    {
        uint8_t tmpBuf[1024] = { 0, };
        uint32_t totalSize = 0;
        uint8_t* curP = buf;

        int32_t cnt_rcv = 0;
        do {
            cnt_rcv = ::recv(client_sock, tmpBuf, 1024, 0);

            if (cnt_rcv < 0)
            {
                LOGE("receive error -> Reason[%d][%s]", errno, strerror(errno));
                break;
            }
            else if (cnt_rcv > 0)
            {
                LOGV(" %d bytes has been received",cnt_rcv );
            }

            //buf += string(tmpBuf, cnt_rcv);
            memcpy(curP, tmpBuf, cnt_rcv);
            curP = curP + cnt_rcv;
            totalSize += cnt_rcv;

        } while (cnt_rcv != 0);

        LOGV("Total Data[%d]", totalSize);
        LOGE("client has disconnected!");
        return totalSize;
    }

    void close()
    {
        ::close(server_sock);
    }
};

#endif //_EVT_SERVER_H_