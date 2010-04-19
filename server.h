/*

#ifndef __TP_SERVER_SOCKET__
#define __TP_SERVER_SOCKET__

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

using std::string;

int server();

class ServerSocket
{   public:
        ServerSocket(int bufLength = 512);

        void initServer(string ip, string port);
        void wait();

        ~ServerSocket();

    private:
        void startup();
        void addressInfo(string ip, string port);
        void createSocket();
        void bindSocket();

        WSADATA wsaData;
        struct addrinfo *result;
        struct addrinfo *ptr;
        struct addrinfo hints;
        SOCKET ListenSocket;
};

#endif

*/