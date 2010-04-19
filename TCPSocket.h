#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "Socket.h"

class TCPSocket : public Socket
{ public:
    TCPSocket();
    TCPSocket(string const ip, string const port);

    virtual void connectSocket();
    virtual int sendData(char const * const data, int length);
    virtual int receiveData(char * const data, int length);
    virtual void disconnect();

  protected:
    virtual void setWsaData(WSADATA const wsaData);
    virtual void setSocket(SOCKET const sock);
    virtual const WSADATA getWsaData();
    virtual const SOCKET getSocket();

  private:
    WSADATA _wsaData;
    struct addrinfo *_result;
    struct addrinfo *_ptr;
    struct addrinfo _hints;
    SOCKET _sock;
};

#endif