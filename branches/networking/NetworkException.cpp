#include "NetworkException.h"

NetworkException::NetworkException() : exception()
{
}

char * NetworkException::what()
{   return "There is a problem with your network.";
}