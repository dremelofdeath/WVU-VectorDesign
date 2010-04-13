#ifndef __CS_481_NETWORK_EXCEPTION__
#define __CS_481_NETWORK_EXCEPTION__

#include <exception>

using std::exception;

class NetworkException : public exception
{   public:
        NetworkException() throw();

        virtual char * what();
};

#endif