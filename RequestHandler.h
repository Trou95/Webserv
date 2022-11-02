#ifndef _REQUESTHANDLER_H_
#define _REQUESTHANDLER_H_


#include <iostream>
#include <string.h>
#include <vector>

#include "utils/FileReader/FileReader.h"

using std::cout;
using std::endl;
using std::string;

typedef string Request;


enum E_HTTP_METHOD
{
    METHOD_GET = 0,
    METHOD_POST,
    METHOD_PUT,
    METHOD_HEAD,
    METHOD_DELETE,
    METHOD_NONE
};


struct stRequestData
{
    string method;
    string filePath;
    string httpProtocol;
    std::vector<std::pair<string,string>> data;
};

struct stResponse
{
    string responseHead;
    string responseData;
};


class RequestHandler
{
 public:
  RequestHandler();

  stResponse Process(Request request);

 private:
   E_HTTP_METHOD getMethod(Request& request);
   const string getFilePath(Request& request);
   const string getHttpProtocol(Request& request);
   const std::vector<std::pair<string,string>> getData(Request& request);

   const string getMethodNameByIndex(E_HTTP_METHOD method);

   std::pair<string,string> getDataVal(string text);

   const char *_default_path;

};














#endif