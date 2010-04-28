#include "NetworkException.h"

NetworkException::NetworkException() //: exception()
{ message = "There is a problem with your network.";
}

NetworkException::NetworkException(string message)
{ this->message = message;
}

const char * NetworkException::Message() throw()
{ return message.c_str();
}

const char * NetworkException::what() const throw()
{   return message.c_str();
}
