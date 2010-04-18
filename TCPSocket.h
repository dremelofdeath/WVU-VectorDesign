#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include <string>
#include "Socket.h"

class TCPSocket : public Socket
{ public:
    TCPSocket();
    TCPSocket(string ip, string port);

    virtual void connectSocket(string ip, string port);
    virtual int sendData(char const * const data, int length);
    virtual int receiveData(char * const data, int length);
    virtual void disconnect();

  protected:
    void createSocket();
    void connectSocket();

    WSADATA wsaData;
    struct addrinfo *result;
    struct addrinfo *ptr;
    struct addrinfo hints;
    SOCKET sock;
};

#endif