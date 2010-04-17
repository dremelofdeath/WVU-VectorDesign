#include "Socket.h"

Socket::Socket()
{ int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(result != 0)
  { 
  }
}

Socket::Socket(string IP, string port)
{ 
}

void Socket::connect(string IP, string port)
{ 
}

void Socket::send(char const * const data)
{
}

void Socket::receive(char const * data)
{
}