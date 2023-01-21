#include "RequestParser.h"


stRequest RequestParser::parseRequest(const std::string &request)
{
    stRequest res;
    vector<string> requestInfo;

    vector<string> lines = str_split(request, '\n');
    if(!lines.empty())
    {
        requestInfo = getRequestInfo(lines[0]);
        if(requestInfo.size() >= 2)
        {
            res.method = requestInfo[0];
            res.endPoint = requestInfo[1];

            for(int i = 1, size = lines.size(); i < size; i++)
            {
                std::pair<string,string> header = parseHeader(lines[i]);
                if(!header.first.empty())
                    res.requestHeaders.insert(header);
            }
        }
    }
    return res;
}



string RequestParser::parseURL(const string &request, const string& index)
{
    string ret = request;

    int found_index = request.find_last_of('/');
    if(found_index != request.npos)
    {
        found_index = request.find_last_of('.');
        if(found_index == request.npos)
            ret = request + "/" + index;
    }
    return ret;
}

string RequestParser::getPath(string request)
{
    int index = request.find_last_of('.');
    if(index != request.npos)
    {
        for(; request[index]; index--)
            if(request[index] == '/')
                return request.substr(0, index + 1);
    }
    return request;
}

std::pair<string,string> RequestParser::parseHeader(const std::string &request)
{
    size_t index = request.find(':');
    if(index != request.npos)
    {
        string key = request.substr(0,index);
        string value = str_trim(request.substr(index + 1), ' ');
        return std::pair<string,string>(key,value);
    }
    return std::pair<string,string>("","");
}

vector <string> RequestParser::getRequestInfo(const std::string &request)
{
    vector<string> res = str_split(request, ' ');
    return res;
}