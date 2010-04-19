#include "Socket.h"

Socket::Socket()
{ int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(result != 0) throw NetworkException("WSAStartup failed!");
}

Socket::Socket(string const ip, string const port)
{ int result = WSAStartup(MAKEWORD(2,2), &wsaData);
  if(result != 0) throw NetworkException("WSAStartup failed!");

  connectTo(ip, port);
}

void Socket::connectTo(string const ip, string const port)
{ setIp(ip);
  setPort(port);
}

void Socket::setIp(string const ip)
{ _ip = ip;
}

const string Socket::getIp()
{ return _ip;
}

void Socket::setPort(string const port)
{ _port = port;
}

const string Socket::getPort()
{ return _port;
}
