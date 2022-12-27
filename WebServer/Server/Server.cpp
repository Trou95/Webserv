#include "Server.h"

Server::Server(const int LISTEN, const std::string ROOT, const std::string INDEX, vector <std::string> ALLOW_METHODS, const string ERRORPAGE) :
                LISTEN(LISTEN), ROOT(ROOT), INDEX(INDEX), ALLOW_METHODS(ALLOW_METHODS), ERRORPAGE(ERRORPAGE)
{

}

void Server::addLocation(stScope data)
{
    Location tmp;
    map<string,vector<string> >::iterator iter;

    tmp.endPoint = data.args[0];

    iter = data.values.find("index");
    tmp.index = iter != data.values.end() ? iter->second[0] : "index.html";

    iter = data.values.find("allow_methods");
    if(iter != data.values.end())
        tmp.allowedMethods = iter->second;
    else
    {
        const char* methods[] = {"GET"};
        tmp.allowedMethods = vector<string>(methods, methods + 1);
    }

    iter = data.values.find("cgi_path");
    if (iter != data.values.end())
        tmp.cgiPath = iter->second[0];

    locations.push_back(tmp);
}