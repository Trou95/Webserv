#include "RequestParser.h"


stRequest RequestParser::parseRequest(const std::string &request)
{
    stRequest res;
    vector<string> requestInfo;

    std::cout << "asdasdas" << std::endl;
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