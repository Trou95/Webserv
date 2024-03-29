#pragma once

#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <poll.h>

#include "Utils/FileReader/FileReader.h"
#include "Utils/Parser/Cgi/CgiParser.h"
#include "Server/Server.h"

using std::cout;
using std::end;
using std::string;
using std::pair;

#define DEFAULT_PORT 80
#define DEFAULT_PATH "www"
#define DEFAULT_ROOT DEFAULT_PATH "/web"
#define DEFAULT_ERRORPAGE DEFAULT_ROOT "/error-page/"
#define DEFAULT_INDEX "index.html"
#define DEFAULT_LIMIT 2000


class Malazgirt
{
    private:
        const string CONFIG_PATH;
        vector<stScope> SERVER_CONFIG;
        string CURRENT_DIRECTORY;

        vector<Server> servers;
        int servers_size;

        map<string,string> filetypes;

        const string readConfig();
        bool parseConfig(const string& config);
        map<string,string> parseFileTypes();

        Server initServer(stScope data);
        void connectServers();
        void Run();

        int acceptRequest(vector<vector<int> >& requests);
        void initPoll(struct pollfd* pollfds,vector<vector<int> >& requests);
        void runPoll(struct pollfd* pollfds,vector<vector<int> >& requests, int request_count);

        int getServerIndexByRequestFD(int requestFD, vector<vector<int> >& requests);

    public:

        Malazgirt(const string& configPath);

};