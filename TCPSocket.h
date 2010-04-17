#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include <string>
#include "Socket.h"

class TCPSocket : public Socket
{ public:
    TCPSocket();
    TCPSocket(string ip, string port);

    virtual void connect(string ip, string port);
    virtual void serve();
    virtual int send(char * const data, int length);
    virtual int receive(char * data, int length);
    virtual void disconnect();

  protected:
    void createSocket();
    void connectSocket();

    WSADATA wsaData;
    //struct addrinfo *result;
    struct addrinfo *ptr;
    struct addrinfo hints;
    SOCKET sock;
};

#endif