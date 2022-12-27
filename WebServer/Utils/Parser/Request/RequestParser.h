#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <utility>

#include "../../Helper.h"


using std::string;
using std::vector;
using std::map;

struct stRequest
{
    string method;
    string endPoint;
    map<string,string> requestHeaders;
};

class RequestParser
{
    private:

        vector<string> getRequestInfo(const string& request);
        std::pair<string,string> parseHeader(const string& request);

    public:

        stRequest parseRequest(const string& request);


};