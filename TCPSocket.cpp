#include "TCPSocket.h"

TCPSocket::TCPSocket() : Socket()
{
}

TCPSocket::TCPSocket(string ip, string port) : Socket(ip, port)
{
}

void TCPSocket::connectSocket(string ip, string port)
{ // This is very bad to use. It zeros the memory pointed to. We need a platform
  // independent solution, as this likely only works for Windows.
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

  result = NULL;
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

  error = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
  if(error == SOCKET_ERROR)
  {   closesocket(sock);
      sock = INVALID_SOCKET;
  }
  
  freeaddrinfo(result);
  if(sock == INVALID_SOCKET)
  {   WSACleanup();
      throw NetworkException("Unable to connect to server.");
  }

  connectSocket();
}

int TCPSocket::sendData(char const * const data, int length)
{ int bytes = send(sock, data, length, 0);
  if(bytes == SOCKET_ERROR)
  { string str = "Send failed: " + WSAGetLastError();
    throw NetworkException(str);
  }
}

int TCPSocket::receiveData(char * const data, int length)
{ int bytes = 0;
  bytes = recv(sock, data, length, 0);
  if(bytes < 0)
  { string str = "Receive failed: " + WSAGetLastError();
    throw NetworkException(str);
  }
}

void TCPSocket::disconnect()
{ int error = shutdown(sock, SD_SEND);
  if(error == SOCKET_ERROR)
  { string str = "Shutdown failed: " + WSAGetLastError();
    closesocket(sock);
    throw NetworkException(str);
  }
}


