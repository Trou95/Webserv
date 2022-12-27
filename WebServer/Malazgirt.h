#pragma once

#include <iostream>
#include <unistd.h>
#include <limits>

#include "Utils/FileReader/FileReader.h"
#include "Utils/Parser/Cgi/CgiParser.h"
#include "Server/Server.h"

using std::count;
using std::end;
using std::string;

#define DEFAULT_PORT 80
#define DEFAULT_PATH "/www"
#define DEFAULT_ROOT DEFAULT_PATH "/html"
#define DEFAULT_ERRORPAGE DEFAULT_PATH  "/error-page/"
#define DEFAULT_INDEX "index.html"
#define MAX_LOG_LEN 256


class Malazgirt
{
    private:
        const string CONFIG_PATH;
        vector<stScope> SERVER_CONFIG;

        string CURRENT_DIRECTORY;

        vector<Server> servers;

        const string readConfig();
        bool parseConfig(const string& config);

        Server initServer(stScope data);

        int Log(const char* format, ...);


    public:

        Malazgirt(const string& configPath);




};