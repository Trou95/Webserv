#include "Server.h"

map<string,string> Server::filetypes;

Server::Server(int PORT, std::string NAME, std::string ROOT, vector<pair<int, string> > error_pages, map<string,string>& filetypes) :
    PORT(PORT), NAME(NAME), ROOT(ROOT)
{
    Server::filetypes = filetypes;

    for(int i = error_pages.size() - 1; i >= 0; i--)
    {
        if(error_pages[i].first == 404)
            this->ERROR_PAGE_404 =  FileReader::readFile(error_pages[i].second);
        else if(error_pages[i].first == 403)
            this->ERROR_PAGE_403 = FileReader::readFile(error_pages[i].second);
    }
}


bool Server::connect()
{
    _serverFD = socket(AF_INET,SOCK_STREAM,0);

    if(_serverFD < 0) {
        Log(LOG_INFO,"Error: %s socket",NAME.c_str());
        return false;
    }
    fcntl(_serverFD, F_SETFL, O_NONBLOCK);

    int enable = 1;
    if(setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        Log(LOG_INFO, "Error: %s socketopt",NAME.c_str());
        return false;
    }

    memset(&data, 0, sizeof(data));
    data.sin_family = AF_INET;
    data.sin_addr.s_addr = htonl(INADDR_ANY);
    data.sin_port = htons(PORT);

    if(bind(_serverFD, (struct sockaddr*)&data, sizeof(data)) < 0) {
        Log(LOG_INFO, "Error: %s bind",NAME.c_str());
        return false;
    }

    if(listen(_serverFD,MAX_REQUEST) < 0) {
        Log(LOG_INFO, "Error: %s listen",NAME.c_str());
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

    cout << tmp << endl;

    request = requestParser.parseRequest(tmp);
    response = initResponse(request);
    send(requestFD, response.c_str(), response.length() ,MSG_DONTWAIT);

    close(requestFD);
}

string Server::initResponse(const stRequest& request)
{
    stResponse response;

    stResponseInfo responseInfo = getResponseInfo(request);
    if(responseInfo.status == HTTP_200 && !locations[responseInfo.index].cgiPath.empty() && responseInfo.contentType == "text/html")
    {
        string res = runCGI(locations[responseInfo.index],request,responseInfo);
        response = parseResponse(res, responseInfo);
    }
    else
    {
            response.responseHead = "HTTP/1.1 " + std::to_string(responseInfo.status) + " OK\r\n";
            response.responseHead += "Content-Type: " + responseInfo.contentType + "\r\n";
            response.responseHead += "Content-Length: " + std::to_string(responseInfo.info.length());
            response.responseHead += "\r\n\r\n";
            response.responseData = responseInfo.info;

    }
    string ret = response.responseHead + response.responseData;
    return ret;
}

string Server::runCGI(const Location& location, const stRequest &request, const stResponseInfo &responseInfo)
{
    int fd[2];
    int fd2[2];
    int pid;


    if(pipe(fd) == -1)
        return "";
    pipe(fd2);

    pid = fork();
    if(pid == -1)
        return "";

    if(pid == 0)
    {
        char** tmp = new char*[3];
        tmp[0] = "/Users/dev_gdemirta/.brew/bin/php-cgi";
        tmp[1] = str_join("",responseInfo.filePath.c_str());
        tmp[2] = 0;


        char **env = initEnv(tmp[1],request,responseInfo.contentType);

        dup2(fd[1],1);
        dup2(fd2[0], 0);
        close(fd[0]);
        close(fd[1]);
        close(fd2[1]);
        close(fd2[1]);
        execve(tmp[0],tmp,env);
        exit(1);
    }
    else
    {
        int count;
        char buffer[1024];
        string res;
        struct pollfd p_fd;


        cout << close(fd[1]) << endl;
        cout << fd[0] << " " << fd[1] << endl;
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
        close(fd[0]);
        close(fd2[0]);
        close(fd2[1]);
        waitpid(pid,NULL,0);
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

char **Server::initEnv(const char *filePath, const stRequest& request, const string& contentType)
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
    if(request.method == "POST")
        env[index++] = str_join("CONTENT_LENGTH=","200");
    //it = request.requestHeaders.find("Content-Length");
    else if(it != end)
        env[index++] = str_join("CONTENT_LENGTH=",it->second.c_str());
    it = request.requestHeaders.find("Content-Type");
    env[index++] = str_join("CONTENT_TYPE=",it != request.requestHeaders.end() ? it->second.c_str() : contentType.c_str());
    env[index] = 0;

    for (int i = 0; env[i]; ++i) {
        cout << env[i] << endl;
    }

    return env;
}

stResponseInfo Server::getResponseInfo(const stRequest &request)
{
    stResponseInfo response;

    int index_location = isValidEndPoint(request.endPoint, request.method);
    if(index_location == -1) {
        response.status = HTTP_404;
        response.contentType = "text/html";
        response.info = this->ERROR_PAGE_404;
        cout << "1" << endl;
    }
    else if(index_location == -2) {
        response.status = HTTP_403;
        response.contentType = "text/html";
        response.info = this->ERROR_PAGE_403;
    }
    else {
        string tmp = request.endPoint;
        int index = request.endPoint.find_last_of('.');
        if(index == request.endPoint.npos)
        {
            if(tmp[tmp.length() - 1] != '/')
                tmp += '/';
            tmp += locations[index_location].index;
        }
        tmp = ROOT + tmp;
        if(isValidFile(tmp) == false) {
            response.status = HTTP_404;
            response.contentType = "text/html";
            response.info = this->ERROR_PAGE_404;
            cout << "2" << endl;
        }
        else {
            response.status = HTTP_200;
            response.index = index_location;
            response.filePath = tmp;
            response.contentType = getContentType(tmp.substr(tmp.find_last_of(".")));

            cout << "file " << tmp << endl;

            ifstream file(tmp, std::ios::ate | std::ios::binary);
            auto size = file.tellg();
            char *image = new char[size];
            file.seekg(0, std::ios::beg);
            file.read(image,size);
            image[size] = 0;
            file.close();
            cout << "Size " << size << endl;
            for(int i = 0; i < size; i++)
                response.info += image[i];

            cout << response.info.length() << endl;

            std::ofstream file2("asdasd.jpeg", std::ios::binary);
            file2 << response.info;


        }
    }
    return response;
}

int Server::isValidEndPoint(const string& request, const string& method)
{
    string tmp = requestParser.getPath(request);
    int size = locations.size() - 1;

    if(tmp[tmp.length() - 1] == '/')
        tmp.pop_back();
    cout << "method " << tmp << endl;
    for(int i = size; i >= 0; i--)
    {
        if(tmp == locations[i].endPoint)
        {
            for(int n = locations[i].allowedMethods.size() - 1; n >= 0; n--)
            {
                if(method == locations[i].allowedMethods[n])
                    return i;
            }
            return -2;
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

void Server::uploadFile(const stRequest &request, const string &requestcontent, const string& type)
{
   string file_boundary = "--" + type.substr(type.find("boundary=") + 9);
   int start = requestcontent.find("\r\n\r\n") + 4;
   int len = file_boundary.length();


   int file_start = requestcontent.find(file_boundary,start);
   while(file_start != requestcontent.npos)
   {
       int index = requestcontent.find("filename=",file_start + len);
       if(index != requestcontent.npos)
       {
           string file_name;

           int i;
           for(i = index + 10; requestcontent[i] && requestcontent[i] != '\"'; i++)
               file_name += requestcontent[i];

           start = requestcontent.find("\r\n\r\n",i) + 4;
           file_start = requestcontent.find(file_boundary, start);

           int size = (file_start - start) - 2;

           string file_content;
           for(int index = start; index < start + size; index++)
           {
               cout << (int)requestcontent[index];
               file_content += requestcontent[index];
           }

           cout << file_start << " " << start << endl;
           cout << request.requestHeaders.find("Content-Length")->second << " " << size << endl;
           cout << "--------" << endl;

           std::ofstream file("www/web/test/" + file_name, std::ios::binary);
           file << file_content;
           file.close();

           std::ifstream file2("image.jpeg", std::ios::binary);
           std::ofstream file3("image.txt", std::ios::binary);
           file3 << file2.rdbuf();
           file3.close();

           cout << file2.tellg() << " " << file_content.length() << endl;

           std::ofstream file4("image_2.txt", std::ios::binary);
           file4 << file_content;
           file4.close();
       }
       else
           file_start = requestcontent.find(file_boundary, file_start + len);
   }

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

string Server::getContentType(const string &fileExtension)
{
    if(fileExtension == ".html" || fileExtension == ".php" || fileExtension == ".py")
        return "text/html";
    else
    {
        map<string,string>::iterator it = filetypes.find(fileExtension);
        return it != filetypes.end() ? it->second : "text/html";
    }
}

vector <string> Server::getAllDirectoryies(const string& filePath, int hidden_files)
{
    vector<string> directoryies;

    DIR *dir = opendir(filePath.c_str());
    if(dir != NULL)
    {
        struct dirent *ent;
        if(hidden_files)
        {
            while((ent = readdir(dir)) != NULL)
                directoryies.push_back(ent->d_name);
        }
        else
        {
            while((ent = readdir(dir)) != NULL)
            {
                if(ent->d_name[0] != '.')
                    directoryies.push_back(ent->d_name);
            }
        }
        closedir(dir);
    }
    return directoryies;
}

bool Server::isValidFile(const string &filePath)
{
    struct stat file;

    if(stat(filePath.c_str(), &file) == -1)
        return false;
    return true;
}

bool Server::isDirectory(const string& filePath)
{
    if(filePath.find_last_of('.') != filePath.npos)
        return false;
    return true;
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