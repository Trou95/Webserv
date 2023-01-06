#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <utility>
#include <map>


#include "../Utils/Parser/Cgi/CgiParser.h"
#include "../Utils/Parser/Request/RequestParser.h"
#include "../Utils/FileReader/FileReader.h"
#include "../Utils/Helper.h"

#define MAX_REQUEST 10

using std::string;
using std::vector;
using std::map;


struct stResponse
{
    string responseHead;
    string responseData;
};


class Location
{
    public:

        string endPoint;
        string index;
        string cgiPath;
        vector<string> allowedMethods;

};

class Server
{
    private:

        int _serverFD;
        struct sockaddr_in data;

        int PORT;
        string NAME;
        string ROOT;
        string ERROR_PAGE;

        vector<Location> locations;
        RequestParser requestParser;

        string readRequest(int requestFD);
        int isValidEndPoint(const string& request, const string& method);
        string getResponseFile(const stRequest& request);

    public:

        Server(int PORT,string NAME, string ROOT, string ERROR_PAGE);

        bool connect();
        void addLocation(stScope location);
        void HandleRequest(int requestFD);

        const int getServerFD();
        const int getPort();
        const string& getName();

};