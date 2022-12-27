#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <map>

#include "../Utils/Parser/Cgi/CgiParser.h"

using std::string;
using std::vector;
using std::map;


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
    public:

        const int LISTEN;
        const string ROOT;
        const string INDEX;
        const string ERRORPAGE;
        const vector<string> ALLOW_METHODS;

        vector<Location> locations;

        void addLocation(stScope location);

    public:

        Server(const int LISTEN, const string ROOT,
               const string INDEX, vector<string> ALLOW_METHODS,
               const string ERRORPAGE);


};