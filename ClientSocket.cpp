#include <iostream>
#include "ClientSocket.h"
#include "NetworkException.h"

using std::cout;
using std::endl;
using std::string;

#define DEFAULT_BUFLEN 512

ClientSocket::ClientSocket(int bufLength)
{   result = NULL;
    ptr = NULL;

    startup();

    recvbuf = (char *)malloc(sizeof(char) * DEFAULT_BUFLEN);
}

void ClientSocket::connectToServer(string ip, string port)
{   addressInfo(ip, port);
    createSocket();
    connectSocket();
}

void ClientSocket::disconnect()
{   closeSocket();
}

void ClientSocket::startup()
{   int error = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(error != 0)
    {   cout << "WSAStartup failed: " << error << endl;
        throw NetworkException();
    }
}

void ClientSocket::addressInfo(string ip, string port)
{   // This is very bad to use. It zeros the memory pointed to. We need a platform
    // independent solution, as this likely only works for Windows.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	
	int error = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if(error != 0)
	{	cout << "getaddrinfo failed: " << error << endl;
		WSACleanup();
		throw NetworkException();
	}
}

void ClientSocket::createSocket()
{   ConnectSocket = INVALID_SOCKET;
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if(ConnectSocket == INVALID_SOCKET)
	{	cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		throw NetworkException();
	}
}

void ClientSocket::connectSocket()
{   int error = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if(error == SOCKET_ERROR)
    {   closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);
    if(ConnectSocket == INVALID_SOCKET)
    {   cout << "Unable to connect to server." << endl;
        WSACleanup();
        throw NetworkException();
    }
}

void ClientSocket::sendData(const char * const data, int length)
{   recvbuflen = DEFAULT_BUFLEN;
    //recvbuf = (char *)malloc(sizeof(char) * DEFAULT_BUFLEN);

    int error = send(ConnectSocket, data, (int)strlen(data), 0);
    if(error == SOCKET_ERROR)
    {   cout << "Send failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        throw NetworkException();
    }

    cout << "Bytes Sent: " << error << endl;
}

void ClientSocket::closeSocket()
{   int error = shutdown(ConnectSocket, SD_SEND);
    if(error == SOCKET_ERROR)
    {   cout << "Shutdown failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        throw NetworkException();
    }

    cout << "Socket closed." << endl;
}

void ClientSocket::receiveData()
{  int error = 0;
   do
    {   error = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if(error > 0) cout << "Bytes Received: " << error << endl;
        else if(error == 0) cout << "Connection Closed" << endl;
        else cout << "Recv Failed: " << WSAGetLastError() << endl;
        cout << "Result: " << error << endl;
    } while(error > 0);
    
    /*
    error = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if(error > 0) cout << "Bytes Received: " << error << endl;
    else if(error == 0) cout << "Connection Closed" << endl;
    else cout << "Recv Failed: " << WSAGetLastError() << endl;

    
    cout << recvbuf << endl;
    */
}

ClientSocket::~ClientSocket()
{   free(recvbuf);
}