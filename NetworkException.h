/*! \file NetworkException.h
 *  \brief Defines a class to handle networking exceptions.
 *  \author Ben Roberts
 */

#ifndef __CS_481_NETWORK_EXCEPTION__
#define __CS_481_NETWORK_EXCEPTION__

//#include <exception>
#include <string>

//using std::exception;
using std::string;

class NetworkException //: public exception
{ public:
    NetworkException() throw();
    NetworkException(string message) throw();

    const char * message() throw();
    virtual const char * what() const throw();

  private:
    string message;
};

#endif

/*!	\class NetworkException
 *	\brief A class to handle networking exceptions.
 */

/*!	\fn NetworkException(string message) throw()
 *	\brief Throws a network exception.
 *	\param message
 *	  Desired message to be thrown (optional).
 *	\memberof NetworkException
 */