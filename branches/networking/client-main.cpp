#include "ClientSocket.h"

int main(int argc, char *argv[])
{	ClientSocket client;

    char test[10000];
    for(int i = 0; i < 10000; i++) test[i] = '0';

    client.connectToServer(argv[1], argv[2]);
    client.sendData(test, 10000);
    client.receiveData();
    client.disconnect();
}