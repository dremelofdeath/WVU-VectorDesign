#include "TCPSocket.h"

TCPSocket::TCPSocket() : Socket()
{ setSocket(INVALID_SOCKET);
}

TCPSocket::TCPSocket(string ip, string port) : Socket(ip, port)
{ setSocket(INVALID_SOCKET);
}

void TCPSocket::connectSocket()
{ ZeroMemory(&_hints, sizeof(_hints));
	_hints.ai_family = AF_UNSPEC;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_protocol = IPPROTO_TCP;

  _result = NULL;
	int error = getaddrinfo(getIp().c_str(), getPort().c_str(), &_hints, &_result);
	if(error != 0)
	{	string str= "getaddrinfo failed: "  + error;
		WSACleanup();
		throw NetworkException(str);
	}

	_ptr = _result;
	setSocket(socket(_ptr->ai_family, _ptr->ai_socktype, _ptr->ai_protocol));
	if(getSocket() == INVALID_SOCKET)
	{	int errorcode = WSAGetLastError();
		string str = "Error at socket(): " + errorcode;
		freeaddrinfo(_result);
		WSACleanup();
		throw NetworkException();
	}

  error = connect(getSocket(), _ptr->ai_addr, (int)_ptr->ai_addrlen);
  if(error == SOCKET_ERROR)
  {   closesocket(getSocket());
      setSocket(INVALID_SOCKET);
  }
  
  freeaddrinfo(_result);
  if(getSocket() == INVALID_SOCKET)
  {   WSACleanup();
      throw NetworkException("Unable to connect to server.");
  }
}

int TCPSocket::sendData(char const * const data, int length)
{ int bytes = send(getSocket(), data, length, 0);
  if(bytes == SOCKET_ERROR)
  { int errorcode = WSAGetLastError();
    string str = "Send failed: " + errorcode;
    throw NetworkException(str);
  }

  return bytes;
}

int TCPSocket::receiveData(char * const data, int length)
{ int bytes = 0;
  bytes = recv(getSocket(), data, length, 0);
  if(bytes < 0)
  { string str = "Receive failed: " + WSAGetLastError();
    throw NetworkException(str);
  }

  return bytes;
}

void TCPSocket::disconnect()
{ int error = shutdown(getSocket(), SD_SEND);
  if(error == SOCKET_ERROR)
  { string str = "Shutdown failed: " + WSAGetLastError();
    closesocket(getSocket());
    throw NetworkException(str);
  }
}

void TCPSocket::setWsaData(WSADATA const wsaData)
{ _wsaData = wsaData;
}

void TCPSocket::setSocket(SOCKET const sock)
{ _sock = sock;
}

const WSADATA TCPSocket::getWsaData()
{ return _wsaData;
}

const SOCKET TCPSocket::getSocket()
{ return _sock;
}
