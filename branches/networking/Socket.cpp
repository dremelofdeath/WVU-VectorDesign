#include "Socket.h"

Socket::Socket()
{ int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(result != 0) throw NetworkException("WSAStartup failed!");
}

Socket::Socket(string ip, string port)
{ int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(result != 0) throw NetworkException("WSAStartup failed!");

  connect(ip, port);
}

void Socket::connect(string ip, string port)
{ this->ip = ip;
  this->port = port;
}
