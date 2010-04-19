#include "ServerSocket.h"

int main(int argc, char *argv[])
{	ServerSocket server;

    server.initServer(argv[1], argv[2]);
    server.wait();
}