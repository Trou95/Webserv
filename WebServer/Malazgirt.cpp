#include "Malazgirt.h"


Malazgirt::Malazgirt(const std::string &configPath) : CONFIG_PATH(configPath)
{
    char buffer[PATH_MAX];
    string config;

    config = readConfig();
    if(parseConfig(config) == 0)
        return;

    getcwd(buffer, PATH_MAX);
    CURRENT_DIRECTORY = buffer;

}

Server Malazgirt::initServer(stScope data)
{
    int server_listen;
    string server_root;
    string server_index;
    string server_errorpage;
    vector<string> server_allowmethods;
    map<string,vector<string> >::iterator iter;


    iter = data.values.find("listen");
    server_listen = iter != data.values.end() ? std::stoi(iter->second[0]) : DEFAULT_PORT;

    iter = data.values.find("root");
    server_root = iter != data.values.end() ? iter->second[0] : CURRENT_DIRECTORY + DEFAULT_ROOT;

    iter = data.values.find("index");
    server_index = iter != data.values.end() ? iter->second[0] : DEFAULT_INDEX;

    iter = data.values.find("allow_methods");
    if(iter != data.values.end())
        server_allowmethods = iter->second;
    else
    {
        const char* tmp[] = {"GET","POST","DELETE"};
        server_allowmethods = vector<string>(tmp,tmp + 3);
    }

    iter = data.values.find("error_page");
    server_errorpage = iter != data.values.end() ? iter->second[0] : DEFAULT_ERRORPAGE;

    return Server(server_listen,server_root,server_index,server_allowmethods,server_errorpage);
}


bool Malazgirt::parseConfig(const std::string &config)
{
    CgiParser cgiParser;
    vector<stScope> res;

    res = cgiParser.Parse(config);
    if(res.empty())
        return Log("Error: Cgi config is empty");

    for(int i = 0, size = res.size(); i < size; i++)
    {
        servers.push_back(initServer(res[i]));
        for (int j = 0; j < res[i].scopes.size(); ++j) {
            servers[i].addLocation(res[i].scopes[j]);
        }
    }

    for (auto item: servers[0].locations) {
        cout << item.endPoint << " " << item.index << " " << item.cgiPath << endl;
    }

    //for(int i = 0; i < servers[0].ALLOW_METHODS.size(); i++)
        //cout << servers[0].ALLOW_METHODS[i] << endl;

    //std::cout << servers[0].ERRORPAGE << endl;
    return 1;

}

const string Malazgirt::readConfig()
{
    string res = FileReader::readFile(this->CONFIG_PATH);
    return res;
}

int Malazgirt::Log(const char *format, ...)
{
    char buffer[MAX_LOG_LEN];
    va_list args;

    va_start (args, format);
    vsprintf (buffer,format, args);
    cout << buffer << endl;
    va_end (args);
    return 0;
}
