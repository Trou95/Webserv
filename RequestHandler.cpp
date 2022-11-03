#include "RequestHandler.h"

#define DEFAULT_PATH "index"

const char methods[][7] = {"GET","POST","PUT","HEAD","DELETE"};


RequestHandler::RequestHandler() : _default_path(DEFAULT_PATH) {

}

stResponse RequestHandler::Process(Request request)
{
    stRequestData data;
    stResponse res;
    stUrlData urlData;

    memset(&data, 0, sizeof(stRequestData));

    E_HTTP_METHOD method = getMethod(request);
    data.method = getMethodNameByIndex(method);
    urlData = parseURL(request);
    data.httpProtocol = getHttpProtocol(request);
    cout << urlData.filePath << endl;
    data.filePath = urlData.filePath;
    data.data = getData(request);

    string fileText = FileReader::readFile(data.filePath);


    res.responseHead = data.httpProtocol + "200 OK\n";
    res.responseHead += "Content-Type: text/html\n";
    res.responseHead += "Content-Length: " + std::to_string(fileText.length());
    res.responseHead += "\n\n";

    res.responseData = fileText;


    return res;

}

const stUrlData RequestHandler::parseURL(Request& request)
{
    string tmp = request;
    stUrlData data;
    
    string filePath = getFilePath(request);
    vector<UrlParams> urlParams = getParams(request);

    data.filePath = filePath;
    data.data = urlParams;
    return data;
}

E_HTTP_METHOD RequestHandler::getMethod(Request& request)
{
    Request tmp = request;
    size_t index = tmp.find(" ");
    if(index != tmp.npos) {
         tmp = tmp.substr(0,index);
         request = request.substr(index + 1);
    }
    for(int i = 0; i < sizeof(methods) / sizeof(methods[0]); i++)
        if(!tmp.compare(methods[i]))
            return static_cast<E_HTTP_METHOD>(i);
    return METHOD_NONE;
}

const string RequestHandler::getHttpProtocol(Request& request)
{
    string tmp = request;
    size_t index = tmp.find("\n");

    if(index != tmp.npos){
        request = request.substr(index + 1);
        tmp = tmp.substr(0,index);
    }
    return tmp;
}

const std::vector<std::pair<string,string>> RequestHandler::getData(Request& request)
{
    Request tmp = request;
    std::pair<string,string> res;
    std::vector<std::pair<string,string>> data;
    size_t index;
    

    index = tmp.find("\n");
    res = getDataVal(tmp);
    data.push_back(res);
    while(index != tmp.npos)
    {
        res = getDataVal(tmp.substr(index + 1));
        if(!res.first.empty() && !res.second.empty())
            data.push_back(res);
        index = tmp.find("\n",index + 1);
    }

    
    return data;
}

std::pair<string,string> RequestHandler::getDataVal(string text)
{
    size_t index = text.find(":");
    size_t len;
    string key;
    string value;

    if(index != text.npos)
    {
     len = text.find("\n");
     if(len == text.npos)
       value = text.substr(index + 1,text.length());
     else
       value = text.substr(index + 1, len - (index + 1));
     key = text.substr(0, index);
    }
    return std::pair<string,string>(key,value);
}

const string RequestHandler::getFilePath(Request& request)
{
    int i = 0;
    while(request[i] && request[i] != ' ' && request[i] != '?')
        i++;
    string filePath = request.substr(0,i);
    if(filePath == "/"){
        filePath = DEFAULT_PATH;
        filePath += ".html";
    }
    else if(filePath[0] == '/')
        filePath = filePath.substr(1);
    request = request.substr(i);
    return filePath;
}

const vector<UrlParams> RequestHandler::getParams(Request& request)
{
    vector<UrlParams> data;

    if(request[0] == '?')
    {
        int i = 1,last_index = 1,last_equal = 1;
        string key,value;
        for(; request[i]; i++)
        {
            if(request[i] == '&' || request[i] == ' ')
            {
                key = request.substr(last_index, last_equal - last_index);
                value = request.substr(last_equal + 1, i - (last_equal + 1));
                last_index = i + 1;
                data.push_back(std::pair<string,string>(key,value));
                if(request[i] == ' ')
                    break;
            }
            else if(request[i] == '=')
                last_equal = i;
        }
        request = request.substr(i + 1);
    }
    return data;
}

const string RequestHandler::getMethodNameByIndex(E_HTTP_METHOD method)
{
    if(method >= 0 && method < sizeof(methods) / sizeof(methods[0]))
        return string(methods[method]);
    return string("NULL");
}