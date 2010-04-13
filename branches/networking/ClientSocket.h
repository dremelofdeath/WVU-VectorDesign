#ifndef __TP_CLIENT_SOCKET_H__
#define __TP_CLIENT_SOCKET_H__

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>

using std::string;

class ClientSocket
{   public:
        ClientSocket(int bufLength = 512);

        void connectToServer(string ip, string port);
        void disconnect();
        void sendData(const char * const data, int length);
        void receiveData();

        ~ClientSocket();

    private:
        void startup();
        void addressInfo(string ip, string port);
        void createSocket();
        void connectSocket();
        void closeSocket();
        

        WSADATA wsaData;
        struct addrinfo *result;
        struct addrinfo *ptr;
        struct addrinfo hints;
        SOCKET ConnectSocket;
        char *recvbuf;
        int recvbuflen;
};

#endif