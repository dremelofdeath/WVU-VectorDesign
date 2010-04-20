#include "NetworkException.h"

NetworkException::NetworkException() : exception()
{ message = "There is a problem with your network.";
}

NetworkException::NetworkException(string message)
{ this->message = message;
}

const char * NetworkException::what()
{   return message.c_str();
}