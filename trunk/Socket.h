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
    Socket(string const ip, string const port);

    virtual void connectTo(string const ip, string const port);
    virtual void connectSocket() = 0;
    virtual int sendData(char const * const data, int length) = 0;
    virtual int receiveData(char * const data, int length) = 0;
    virtual void disconnect() = 0;

  protected:
    virtual void setIp(string const ip);
    virtual void setPort(string const port);
    virtual const string getIp();
    virtual const string getPort();

  private:
    string _ip;
    string _port;

    WSADATA wsaData;
};

#endif
