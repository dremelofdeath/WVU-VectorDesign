#ifndef __TP_SOCKET_H__
#define __TP_SOCKET_H__

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

#include "NetworkException.h"

using std::string;

class Socket
{ public:
    Socket();
    Socket(string IP, string port);

    virtual void connect(string IP, string port);
    virtual void send(char const * const data);
    virtual void receive(char const * data);

  protected:
    string IP;
    string port;

    WSADATA wsaData;
};

#endif
