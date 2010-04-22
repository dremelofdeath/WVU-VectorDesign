/*!	\file TCPSocket.h
 *	\author Ben Roberts
 */

#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "Socket.h"

class TCPSocket : public Socket
{ public:
    TCPSocket();
    TCPSocket(string const ip, string const port);

    virtual void connectSocket();
    virtual int sendData(char const * const data, int length);
    virtual int receiveData(char * const data, int length);
    virtual void disconnect();

  protected:
    virtual void setWsaData(WSADATA const wsaData);
    virtual void setSocket(SOCKET const sock);
    virtual const WSADATA getWsaData();
    virtual const SOCKET getSocket();

  private:
    WSADATA _wsaData;
    struct addrinfo *_result;
    struct addrinfo *_ptr;
    struct addrinfo _hints;
    SOCKET _sock;
};

#endif

/*!	\class TCPSocket
 *	\brief ....
 *
 *	...detailed...
 */

/*!	\fn TCPSocket(string const ip, string const port)
 *	\brief Constructor
 *	\param ip
 *	  The desired destination ip address.
 *	\param port
 *	  The desired port to establish the connection over.
 *	\memberof TCPSocket
 */

/*!	\fn virtual void connectSocket()
 *	\brief Attempts to establish a TCP connection with the server.
 *	\memberof TCPSocket
 */

/*!	\fn virtual int sendData(char const * const data, int length)
 *	\brief Sends data over the TCP connection.
 *	\param data
 *	  The data that will be sent.
 *	\param length
 *	  The length of the data being sent.
 *	\return
 *	  .............
 *	\memberof TCPSocket
 */

/*!	\fn virtual int receiveData(char * const data, int length)
 *	\brief Recieves data through the TCP connection.
 *	\param data
 *	  The storage place for the incoming data.
 *	\param length
 *	  The expected length of the incoming data.
 *	\return
 *	  .......................
 *	\memberof TCPSocket
 */

/*!	\fn virtual void disconnect()
 *	\brief Closes the socket and disconnects the TCP connection.
 *	\memberof TCPSocket
 */