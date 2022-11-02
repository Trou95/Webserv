#include "Server.h"

Server::Server() : port(PORT) , _server_fd(0) {
    setConnectionState(CONNECT_STATE_WAITING);
}

void Server::Run() {
    if(Connect() == false)
        exit(1);
    int requestFD;
    string requestData;
    int dataSize = sizeof(data);
    stResponse res;

    while(1)
    {
        cout << "Server: Waiting for connection" << endl;
        memset(&data, 0, sizeof(data));
        requestFD = accept(_server_fd, (sockaddr*)&data, (socklen_t*)&dataSize);
        if(requestFD < 0){
            perror("Error: accept packet");
            continue;
        }
        requestData = readData(requestFD);
        res = requestHandler.Process(requestData);
        
        //printf("Request: \n%s\n",requestData.c_str());

        string response(res.responseHead + res.responseData);


        send(requestFD,response.c_str(),response.length(),0);

        close(requestFD);
    }
}

bool Server::Connect() {

    setConnectionState(CONNECT_STATE_CONNECTING);
    _server_fd = socket(AF_INET,SOCK_STREAM,0);

    if(_server_fd < 0){
        perror("Error: creating socket");
        return false;
    }

    memset(&data, 0, sizeof(data));
    data.sin_family = AF_INET;
    data.sin_addr.s_addr = htonl(INADDR_ANY);
    data.sin_port = htons(port);

    
    if(bind(_server_fd, (struct sockaddr*)&data, sizeof(data)) < 0){

        #if DEBUG
            randomPort();
        #else
            perror("Error: bind socket");
            return false;
        #endif
    }

    if(listen(_server_fd, MAX_REQUEST) < 0){
        perror("Error: listen error");
        return false;
    }
    setConnectionState(CONNECT_STATE_CONNECTED);
    return true;
}

string Server::readData(int fd){
    string data;
    char c;
    pollfd p_data;
    int ret;

    while(recv(fd, &c, 1, 0) > 0){
        data += c;
        p_data.fd = fd;
        p_data.events = POLLIN;
        ret = poll(&p_data, 1, 1000);
        if(ret <= 0)
            break;
    }
    return data;
}

E_CONNECT_STATE Server::getConnectionState(){
    return _connectState;
}

void Server::setConnectionState(E_CONNECT_STATE connectionstate){
    if(connectionstate > 0 && _connectState <= E_CONNECT_STATE::CONNECT_STATE_CONNECTED)
        _connectState = connectionstate;
}

#if DEBUG

void Server::randomPort()
{
    int randomPort = port + rand() % 10;
    data.sin_port = htons(randomPort);
    while(bind(_server_fd, (struct sockaddr*)&data, sizeof(data)) < 0)
    {
        randomPort = port + rand() % 10;
        data.sin_port = htons(randomPort);
    }
    cout << "Port: " << randomPort << endl;
}

#endif