#include <iostream>

#include "ServerSocket.h"
#include "TCPSocket.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{	int length = 10000;
  char test[10000];
  char data[10000];
  for(int i = 0; i < length; i++) test[i] = '0';

  cout << "Welcome to Networking for Dummies!" << endl;

  try
  { if(string(argv[1]) == "s")
    { cout << "Attempting to start server at: " << argv[2] << ":" << argv[3];
      ServerSocket server(argv[2], argv[3]);
      
      server.serve();
      server.receiveData(data, length);

      cout << data << endl;

      server.sendData(test, length);
      server.disconnect();

    }
    else if(string(argv[1]) == "c")
    { cout << "Attempting to connect client to: " << argv[2] << ":" << argv[3];
      TCPSocket client(argv[2], argv[3]);

      client.connectSocket();
      client.sendData(test, 10000);
      client.receiveData(data, 10000);

      cout << data << endl;

      client.disconnect();
    }
  }
  catch(exception& e)
  { cout << e.what() << endl;
  }

  cout << "End of the road, folks!" << endl;
}