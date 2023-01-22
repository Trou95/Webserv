#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <utility>
#include <map>


#include "../Utils/Parser/Cgi/CgiParser.h"
#include "../Utils/Parser/Request/RequestParser.h"
#include "../Utils/FileReader/FileReader.h"
#include "../Utils/Helper.h"

#define MAX_REQUEST 10

using std::string;
using std::vector;
using std::map;


enum E_HTTP_STATUS
{
    HTTP_200 = 200,
    HTTP_403 = 403,
    HTTP_404 = 404
};

struct stResponse
{
    string responseHead;
    string responseData;
};

struct stResponseInfo
{
    E_HTTP_STATUS status;
    int index;
    string info;
    string contentType;
    string filePath;
};


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
    private:

        int _serverFD;
        struct sockaddr_in data;

        int PORT;
        string NAME;
        string ROOT;
        string ERROR_PAGE_404;
        string ERROR_PAGE_403;
        unsigned int CLIENT_LIMIT;

        vector<Location> locations;
        RequestParser requestParser;

        static map<string,string> filetypes;

        string readRequest(int requestFD);
        int isValidEndPoint(string& filePath, const string& method);
        string runCGI(const Location& location, const stRequest& request, const stResponseInfo& responseInfo,const string& rawcontent);
        string initResponse(const stRequest& request, const string& rawcontent);
        stResponse parseResponse(string& response, const stResponseInfo& responseInfo);
        stResponseInfo getResponseInfo(const stRequest& request);
        inline string getResponseStatusType(const E_HTTP_STATUS& status);
        char **initEnv(const char* filePath, const stRequest& request, const string& contentType);

        void uploadFile(const stRequest& request, const string& requestcontent,const string& type);

        vector<string> getAllDirectoryies(const string& filePath, int hidden_files);
        string showDirectoryies(const string& endPoint, const string& filePath);
        bool isValidFile(const string& filePath);
        bool isDirectory(const string& f ilePath);

        string getContentType(const string& fileExtension);


    public:

        Server(int PORT,string NAME, string ROOT, vector<pair<int, string> > ERROR_PAGES, unsigned int CLIENT_LIMIT, map<string,string>& filetypes);

        bool connect();
        void addLocation(stScope location);
        string HandleRequest(int requestFD);
        void sendResponse(int requestFD, const string& response);

        const int getServerFD();
        const int getPort();
        const string& getName();

};