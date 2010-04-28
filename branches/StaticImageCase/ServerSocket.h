#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

#include "TCPSocket.h"

class ServerSocket : public TCPSocket
{ public:
    ServerSocket();
    ServerSocket(string port);
    ServerSocket(string ip, string port);

    void serve();

  protected:
    void setServerSocket(SOCKET const server);
    SOCKET const getServerSocket();

  private:
    SOCKET _server;
};

#endif