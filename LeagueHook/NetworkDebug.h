#pragma once
//#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

enum NetworkCmd : int
{
    kAck, // acknowledgment, sent when client connects
    kPrintLog,
    kDrawCircle,
    kUnk = -1
};

class NetworkDebug
{
public:
    NetworkDebug( const char *ip, unsigned short port );
    bool IsReady();
    // Does not directly close the socket, rather prepares it to be closed (on the next NetworkLoop)
    void Close();
    static void Init();
private:
    std::string ip;
    unsigned short port;
    SOCKET sock;
    SOCKADDR_IN addr;
    bool isReady;

    void NetworkLoop();
};