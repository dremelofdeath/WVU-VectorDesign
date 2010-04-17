#ifndef __CS_481_NETWORK_EXCEPTION__
#define __CS_481_NETWORK_EXCEPTION__

#include <exception>
#include <string>

using std::exception;
using std::string;

class NetworkException : public exception
{ public:
    NetworkException() throw();
    NetworkException(string message) throw();

    virtual const char * what();

  private:
    string message;
};

#endif