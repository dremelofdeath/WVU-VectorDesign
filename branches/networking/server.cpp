#include <iostream>
#include "server.h"
#include "NetworkException.h"

using std::cout;
using std::endl;
using std::string;

ServerSocket::ServerSocket(int bufLength)
{   result = NULL;
    ptr = NULL;

    startup();
}

void ServerSocket::startup()
{   int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult != 0)
	{	cout << "WSAStartup failed: " << iResult << endl;
		throw NetworkException();
	}
}

void ServerSocket::initServer(string ip, string port)
{   addressInfo(ip, port);
    createSocket();
    bindSocket();
}

void ServerSocket::wait()
{   if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
 	{	cout << "Error at bind(): " << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		throw NetworkException();
	}
    
    while(true)
    {  	SOCKET ClientSocket = INVALID_SOCKET;
    	ClientSocket = accept(ListenSocket, NULL, NULL);
    	if(ClientSocket == INVALID_SOCKET)
     	{	cout << "Accept failed: " << WSAGetLastError() << endl;
    		closesocket(ListenSocket);
    		WSACleanup();
    		throw NetworkException();
    	}
        
    	#define DEFAULT_BUFLEN 512

    	char recvbuf[DEFAULT_BUFLEN];
        int iSendResult;
        int recvbuflen = DEFAULT_BUFLEN;
        int iResult;

        do
        {   iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if(iResult > 0)
            {   cout << "Bytes received: " << iResult << endl;
                cout << recvbuf << endl;

                iSendResult = send(ClientSocket, recvbuf, iResult, 0);
                if(iSendResult == SOCKET_ERROR)
                {   cout << "Send failed: " << WSAGetLastError() << endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    throw NetworkException();
                }

                cout << "Bytes sent: " << iSendResult << endl;
            } else if(iResult == 0)
            {   
                cout << "Connection closing..." << endl;
            }
            else
            {   cout << "recv failed: " << WSAGetLastError() << endl;
                closesocket(ClientSocket);
                WSACleanup();
                throw NetworkException();
            }
        } while(iResult > 0);

        iResult = shutdown(ClientSocket, SD_SEND);
        if(iResult == SOCKET_ERROR)
        {   cout << "Shutdown failed: " << WSAGetLastError() << endl;
            closesocket(ClientSocket);
            WSACleanup();
            throw NetworkException();
        }
    }
}

void ServerSocket::addressInfo(string ip, string port)
{   // This is very bad to use. It zeros the memory pointed to. We need a platform
    // independent solution, as this likely only works for Windows.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	
	int error = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if(error != 0)
	{	cout << "getaddrinfo failed: " << error << endl;
		WSACleanup();
		throw NetworkException();
	}
}

void ServerSocket::createSocket()
{   ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(ListenSocket == INVALID_SOCKET)
	{	cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		throw NetworkException();
	}
}

void ServerSocket::bindSocket()
{   int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR)
    {   cout << "bind failed: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        throw NetworkException();
    }
}

ServerSocket::~ServerSocket()
{
}