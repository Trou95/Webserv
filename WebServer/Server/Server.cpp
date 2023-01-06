#include "Server.h"

Server::Server(int PORT, std::string NAME, std::string ROOT, std::string ERROR_PAGE) :
    PORT(PORT), NAME(NAME), ROOT(ROOT), ERROR_PAGE(ERROR_PAGE)
{
    this->ERROR_PAGE = FileReader::readFile(this->ERROR_PAGE + "404.html");
}


bool Server::connect()
{
    _serverFD = socket(AF_INET,SOCK_STREAM,0);

    if(_serverFD < 0) {
        Log(LOG_INFO,"Error: %s socket",NAME);
        return false;
    }

    int enable = 1;
    if(setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        Log(LOG_INFO, "Error: %s socketopt",NAME);
        return false;
    }

    memset(&data, 0, sizeof(data));
    data.sin_family = AF_INET;
    data.sin_addr.s_addr = htonl(INADDR_ANY);
    data.sin_port = htons(PORT);

    if(bind(_serverFD, (struct sockaddr*)&data, sizeof(data)) < 0) {
        Log(LOG_INFO, "Error: %s bind",NAME);
        return false;
    }

    if(listen(_serverFD,MAX_REQUEST) < 0) {
        Log(LOG_INFO, "Error: %s listen",NAME);
        return false;
    }

    return true;
}

void Server::HandleRequest(int requestFD)
{
    string tmp;
    stRequest request;
    stResponse response;

    tmp = readRequest(requestFD);
    request = requestParser.parseRequest(tmp);
    string ret = getResponseFile(request);

    response.responseHead = "HTTP/1.1 200 OK";
    response.responseHead += "Content-Type: text/html\n";
    response.responseHead += "Content-Length: " + std::to_string(ret.length());
    response.responseHead += "\n\n";

    response.responseData = ret;
    string res = response.responseHead + response.responseData;


    send(requestFD, res.c_str(), res.length() ,MSG_DONTWAIT);

    close(requestFD);
}

string Server::getResponseFile(const stRequest &request)
{
    int index = isValidEndPoint(request.endPoint, request.method);
    if(index == -1)
    {
       return this->ERROR_PAGE;
    }
    return FileReader::readFile(requestParser.parseURL(ROOT + request.endPoint,locations[index].index));
}

int Server::isValidEndPoint(const string& request, const string& method)
{
    string tmp = requestParser.getPath(request);
    int size = locations.size() - 1;

    for(int i = size; i >= 0; i--)
    {
        if(tmp == locations[i].endPoint)
        {
            for(int n = size; n >= 0; n--)
            {
                if(method == locations[i].allowedMethods[n])
                    return i;
            }
            return -1;
        }
    }
    return -1;
}

string Server::readRequest(int requestFD)
{
    string data;
    char c;

    while(recv(requestFD, &c, 1, MSG_DONTWAIT) > 0)
        data += c;
    return data;
}

void Server::addLocation(stScope data)
{
    Location tmp;
    map<string,vector<string> >::iterator iter;

    tmp.endPoint = data.args[0];

    iter = data.values.find("index");
    tmp.index = iter != data.values.end() ? str_trim(iter->second[0], ' ') : "index.html";

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

const int Server::getServerFD()
{
    return this->_serverFD;
}

const int Server::getPort()
{
    return this->PORT;
}

const string &Server::getName()
{
    return this->NAME;
}