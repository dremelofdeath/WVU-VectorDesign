#ifndef __TP_SOCKET_H__
#define __TP_SOCKET_H__

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

#include "NetworkException.h"

using std::string;

class Socket
{ public:
    Socket() throw(NetworkException);
    Socket(string ip, string port) throw(NetworkException);

    virtual void connectSocket(string ip, string port);
    virtual int sendData(char const * const data, int length) = 0;
    virtual int receiveData(char * const data, int length) = 0;
    virtual void disconnect() = 0;

  protected:
    string ip;
    string port;

    WSADATA wsaData;
};

#endif
