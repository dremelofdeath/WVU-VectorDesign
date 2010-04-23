#include "ServerSocket.h"

ServerSocket::ServerSocket() : TCPSocket()
{ setServerSocket(INVALID_SOCKET);
}

ServerSocket::ServerSocket(string port) : TCPSocket("127.0.0.1", port)
{ setServerSocket(INVALID_SOCKET);
}

ServerSocket::ServerSocket(string ip, string port) : TCPSocket(ip, port)
{ setServerSocket(INVALID_SOCKET);
}

void ServerSocket::serve()
{ int error = 0;

  struct addrinfo *result;
  struct addrinfo *ptr;
  struct addrinfo hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(getIp().c_str(), getPort().c_str(), &hints, &result);
  if(error != 0)
  { WSACleanup();
    throw NetworkException("getaddrinfo failed: " + error);
  }

  setServerSocket(socket(result->ai_family, result->ai_socktype, result->ai_protocol));
  if(getServerSocket() == INVALID_SOCKET)
  { int errorcode = WSAGetLastError();
	string str = "Error at socket(): " + errorcode;
    freeaddrinfo(result);
    WSACleanup();
    throw NetworkException(str);
  }

  error = bind(getServerSocket(), result->ai_addr, (int)result->ai_addrlen);
  if(error == SOCKET_ERROR)
  { int errorcode = WSAGetLastError();
	string str = "Bind failed: " + errorcode;
    freeaddrinfo(result);
    closesocket(getServerSocket());
    WSACleanup();
    throw NetworkException(str);
  }

  error = listen(getServerSocket(), SOMAXCONN);
  if(error == SOCKET_ERROR)
  { int errorcode = WSAGetLastError();
	string str = "Error at bind(): " + errorcode;
    closesocket(getServerSocket());
    WSACleanup();
    throw NetworkException(str);
  }

  setSocket(accept(getServerSocket(), NULL, NULL));
  if(getSocket() == INVALID_SOCKET)
  { int errorcode = WSAGetLastError();
	string str = "Accept failed: " + errorcode;
    closesocket(getSocket());
    WSACleanup();
    throw NetworkException(str);
  }
}

void ServerSocket::setServerSocket(SOCKET const server)
{ _server = server;
}

SOCKET const ServerSocket::getServerSocket()
{ return _server;
}