#include "TCPSocket.h"

TCPSocket::TCPSocket() : Socket()
{
}

TCPSocket::TCPSocket(string ip, string port) : Socket(ip, port)
{
}

void TCPSocket::connect(string ip, string port)
{ // This is very bad to use. It zeros the memory pointed to. We need a platform
  // independent solution, as this likely only works for Windows.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

  struct adderinfo *result;
	int error = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if(error != 0)
	{	string str= "getaddrinfo failed: "  + error;
		WSACleanup();
		throw NetworkException(str);
	}

  sock = INVALID_SOCKET;
	ptr = result;
	sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if(sock == INVALID_SOCKET)
	{	string str = "Error at socket(): " + WSAGetLastError();
		freeaddrinfo(result);
		WSACleanup();
		throw NetworkException();
	}

  connectSocket();
}

void TCPSocket::createSocket()
{ sock = INVALID_SOCKET;
	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if(ConnectSocket == INVALID_SOCKET)
	{	cout << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		WSACleanup();
		throw NetworkException();
	}
}

void TCPSocket::connectSocket()
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