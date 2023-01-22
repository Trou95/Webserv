#include "Malazgirt.h"


Malazgirt::Malazgirt(const std::string &configPath) : CONFIG_PATH(configPath)
{
    char buffer[PATH_MAX];
    string config;

    config = str_clear(readConfig());

    filetypes = parseFileTypes();
    if(parseConfig(config) == 0)
        return;

    getcwd(buffer, PATH_MAX);
    CURRENT_DIRECTORY = buffer;

    connectServers();
    Run();
}

void Malazgirt::Run()
{
    vector<vector<int> > requests;

    while(1)
    {
        int request_count = acceptRequest(requests);
        if(request_count > 0)
        {
            struct pollfd* pollfds = new pollfd[request_count];
            memset(pollfds,0,sizeof(struct pollfd) * request_count);
            initPoll(pollfds,requests);
            runPoll(pollfds,requests,request_count);
            delete[] pollfds;

        }
        for(int i = requests.size() - 1; i >= 0; i--)
        {
            for(int n = requests[i].size() - 1; n >= 0; n--)
                close(requests[i][n]);
        }
        requests.clear();
    }

}

int Malazgirt::acceptRequest(vector <vector<int> > &requests)
{
    int request_count;
    int requestFD;

    request_count = 0;
    for(int i = servers.size() - 1; i >= 0; i--)
    {
        vector<int> tmp;

        while(1)
        {
            requestFD = accept(servers[i].getServerFD(), NULL, NULL);
            if(requestFD < 0)
                break;
            tmp.push_back(requestFD);
            request_count++;
        }
        requests.push_back(tmp);
    }
    return request_count;
}

void Malazgirt::initPoll(struct pollfd* pollfds, vector <vector<int> > &requests)
{
    int index = 0;

    for(int i = requests.size() - 1; i >= 0; i--)
    {
        for(int n = requests[i].size() - 1; n >= 0; n--)
        {
            pollfds[index].fd = requests[i][n];
            pollfds[index].events = POLLIN;
            index++;
        }
    }
}

void Malazgirt::runPoll(struct pollfd *pollfds, vector <vector<int> > &requests, int request_count)
{
    int ret;
    int count = request_count;
    string response;

    while(count > 0)
    {
        ret = poll(pollfds, request_count, 1000);
        if(ret <= 0)
            break;
        else
        {
            for(int i = request_count - 1; i >= 0; i--)
            {
                if(pollfds[i].revents & POLLIN)
                {
                    int index = getServerIndexByRequestFD(pollfds[i].fd,requests);
                    if(index > -1)
                    {
                        response = servers[index].HandleRequest(pollfds[i].fd);
                        servers[index].sendResponse(pollfds[i].fd, response);
                        pollfds[i].fd = -1;
                        count--;
                    }
                }
            }
        }
    }
}

int Malazgirt::getServerIndexByRequestFD(int requestFD, vector <vector<int> > &requests)
{
    for(int i = requests.size() - 1; i >= 0; i--)
    {
        for(int n = requests[i].size() - 1; n >= 0; n--)
            if(requestFD == requests[i][n])
                return i;
    }
    return -1;
}

void Malazgirt::connectServers()
{
    for(int i = 0, len = servers.size(); i < len; i++)
    {
       if(servers[i].connect() == false)
           servers.erase(servers.begin() + i);
       else
           Log(LOG_INFO,"Info: Server %s connected on port: %d",servers[i].getName().c_str(), servers[i].getPort());
    }
}

Server Malazgirt::initServer(stScope data)
{
    int server_port;
    string server_name;
    vector<pair<int, string> > error_pages;
    map<string,vector<string> >::iterator iter;
    unsigned int client_limit;


    iter = data.values.find("listen");
    server_port = iter != data.values.end() ? std::stoi(iter->second[0]) : DEFAULT_PORT;

    iter = data.values.find("server_name");
    if(iter != data.values.end())
        server_name = iter->second[0];
    else
    {
        char buffer[256];
        memset(buffer,0,sizeof(buffer));
        sprintf(buffer,"server_%lu",servers.size() + 1);
        server_name = buffer;
    }

    iter = data.values.find("root");
    string server_root = iter != data.values.end() ? str_trim(iter->second[0], ' ') : CURRENT_DIRECTORY + DEFAULT_ROOT;

    iter = data.values.find("error_page_404");
    error_pages.push_back(pair<int, string>(404, iter != data.values.end() ? DEFAULT_ERRORPAGE + str_trim(iter->second[0], ' ') : DEFAULT_ERRORPAGE "404.html"));

    iter = data.values.find("error_page_403");
    error_pages.push_back(pair<int, string>(403, iter != data.values.end() ? DEFAULT_ERRORPAGE + str_trim(iter->second[0], ' ') : DEFAULT_ERRORPAGE "403.html"));

    iter = data.values.find("client_limit");
    client_limit = iter != data.values.end() ? std::stoi(iter->second[0]) : DEFAULT_LIMIT;

    return Server(server_port,server_name,server_root, error_pages, client_limit, filetypes);
}


bool Malazgirt::parseConfig(const std::string &config)
{
    CgiParser cgiParser;
    vector<stScope> res;

    res = cgiParser.Parse(config);
    if(res.empty())
        return Log(LOG_INFO,"Error: Cgi config is empty");

    for(int i = 0, size = res.size(); i < size; i++)
    {
        servers.push_back(initServer(res[i]));
        for (int j = 0; j < res[i].scopes.size(); ++j) {
            servers[i].addLocation(res[i].scopes[j]);
        }
    }
    return 1;
}

map<string,string> Malazgirt::parseFileTypes()
{
    map<string,string> ret;

    string res = FileReader::readFile("Conf/types.malazgirt");
    vector<string> lines = str_split(res, '\n');
    for(int i = 0; i < lines.size(); i++)
    {
        int index = lines[i].find(":");
        if(index != string::npos)
        {
            string key = lines[i].substr(0,index);
            string value = lines[i].substr(index + 1);
            ret.insert(pair<string,string>(key,value));
        }
    }
    return ret;
}

const string Malazgirt::readConfig()
{
    string res = FileReader::readFile(this->CONFIG_PATH);
    return res;
}

