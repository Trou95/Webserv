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
    fcntl(_serverFD, F_SETFL, O_NONBLOCK);

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
    string response;
    stRequest request;

    tmp = readRequest(requestFD);


    request = requestParser.parseRequest(tmp);
    response = initResponse(request);


    send(requestFD, response.c_str(), response.length() ,MSG_DONTWAIT);

    close(requestFD);
}

string Server::initResponse(const stRequest& request)
{
    stResponse response;


    stResponseInfo responseInfo = getResponseInfo(request);
    if(responseInfo.status == HTTP_200 && !locations[responseInfo.index].cgiPath.empty())
    {
        string res = runCGI(locations[responseInfo.index],request);
        response = parseResponse(res, responseInfo);
    }
    else
    {
        response.responseHead = "HTTP/1.1 " + std::to_string(responseInfo.status) + " OK\n";
        response.responseHead += "Content-Type: text/html\n";
        response.responseHead += "Content-Length: " + std::to_string(responseInfo.info.length());
        response.responseHead += "\r\n\r\n";
        response.responseData = responseInfo.info;
    }
    string ret = response.responseHead + response.responseData;
    return ret;
}

string Server::runCGI(const Location& location, const stRequest &request)
{
    int tmpFD = dup(1);
    int fd[2];
    int pid;

    if(pipe(fd) == -1)
        return "";

    pid = fork();
    if(pid == -1)
        return "";

    if(pid == 0)
    {
        char** tmp = new char*[3];
        tmp[0] = str_join("/bin/",location.cgiPath.c_str());
        tmp[1] = strdup(requestParser.parseURL(ROOT + request.endPoint, location.index).c_str());
        tmp[2] = 0;

        map<string,string>::const_iterator it = request.requestHeaders.find("Cookie");


        char **env = initEnv(tmp[1],request);

        dup2(fd[1],1);
        close(fd[0]);
        close(fd[1]);
        execve(tmp[0],tmp,env);
        exit(1);
    }
    else
    {
        int count;
        char buffer[1024];
        string res;
        struct pollfd p_fd;


        close(fd[1]);
        p_fd.fd = fd[0];
        p_fd.events = POLLIN;

        int ret = poll(&p_fd, 1, 1000);
        if(ret > 0)
        {
            if(p_fd.revents & POLLIN)
            {
                while((count = read(fd[0], buffer, sizeof(buffer))) > 0)
                {
                    buffer[count] = '\0';
                    res += buffer;
                }
            }
        }
        wait(NULL);
        dup2(1,tmpFD);
        return res;
    }
    return "";
}

stResponse Server::parseResponse(string &response, const stResponseInfo &responseInfo)
{
    stResponse res;


    int index = response.find("\r\n\r\n");
    if(index != response.npos)
    {
        res.responseHead = "HTTP/1.1 ";
        res.responseHead += std::to_string(responseInfo.status) + " ";
        if(responseInfo.status == HTTP_200)
            res.responseHead += "OK\n";
        else
            res.responseHead += "NOT FOUND\n";
        res.responseHead += response.substr(0,index);
        response = response.substr(index + 4);
        res.responseHead += "Content-Length: " + std::to_string(response.length());
        res.responseHead += "\r\n\r\n";
        res.responseData = response;
    }
    else
        res.responseData = response;
    return res;
}

char **Server::initEnv(const char *filePath, const stRequest& request)
{
    char **env = new char*[10];
    int index = 4;

    env[0] = strdup("GATEWAY_INTERFACE=CGI/1.1");
    env[1] = strdup("REDIRECT_STATUS=true");
    env[2] = str_join("REQUEST_METHOD=",request.method.c_str());
    env[3] = str_join("SCRIPT_FILENAME=",filePath);

    map<string,string>::const_iterator it = request.requestHeaders.find("Cookie");
    map<string,string>::const_iterator end = request.requestHeaders.end();
    if(it != end)
        env[index++] = str_join("HTTP_COOKIE=",it->second.c_str());
    it = request.requestHeaders.find("Content-Length");
    if(it != end)
        env[index++] = str_join("CONTENT_LENGTH=",it->second.c_str());
    it = request.requestHeaders.find("Content-Type");
    if(it != end)
        env[index++] = str_join("CONTENT_TYPE=",it->second.c_str());
    env[index] = 0;
    return env;
}

stResponseInfo Server::getResponseInfo(const stRequest &request)
{
    stResponseInfo response;

    int index = isValidEndPoint(request.endPoint, request.method);
    if(index == -1)
    {
        response.status = HTTP_404;
        response.info = this->ERROR_PAGE;
        return response;
    }
    response.status = HTTP_200;
    response.index = index;
    response.info = FileReader::readFile(requestParser.parseURL(ROOT + request.endPoint,locations[index].index));
    return response;
}

int Server::isValidEndPoint(const string& request, const string& method)
{
    string tmp = requestParser.getPath(request);
    int size = locations.size() - 1;

    for(int i = size; i >= 0; i--)
    {
        if(tmp == locations[i].endPoint)
        {
            for(int n = locations[i].allowedMethods.size() - 1; n >= 0; n--)
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
    int count;
    char buffer[1024];
    string data;


    while((count = recv(requestFD, buffer, sizeof(buffer), MSG_DONTWAIT)) > 0)
    {
        buffer[count] = '\0';
        data += buffer;
    }
    return data;
}

void Server::addLocation(stScope data)
{
    Location tmp;
    map<string,vector<string> >::iterator iter;

    tmp.endPoint = data.args[0];

    iter = data.values.find("index");
    tmp.index = iter != data.values.end() ? str_trim(iter->second[0], ' ') : "index.html";

    iter = data.values.find("allowed_methods");
    if(iter != data.values.end())
    {
        for(int i = iter->second.size() - 1; i >= 0; i--)
            iter->second[i] = str_trim(iter->second[i],' ');
        tmp.allowedMethods = iter->second;
    }
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