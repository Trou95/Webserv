#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "utils/FileReader/FileReader.h"
#include "RequestHandler.h"

using namespace std;

#define PORT 8080
#define MAX_REQUEST 10

#define DEBUG 1

enum E_CONNECT_STATE
{
    CONNECT_STATE_WAITING = 0,
    CONNECT_STATE_CONNECTING,
    CONNECT_STATE_CONNECTED
};

class Server {

public:
    Server();

    void Run();
    E_CONNECT_STATE getConnectionState();
    void setConnectionState(E_CONNECT_STATE connectionstate);

    const unsigned short port;


private:
    bool Connect();
    string readData(int fd);

    int _server_fd;
    struct sockaddr_in data;
    E_CONNECT_STATE _connectState;

    RequestHandler requestHandler;
 
 
    #if DEBUG
        void randomPort();
    #endif
};


#endif
