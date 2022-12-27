#ifndef _CGIPARSER_H_
#define _CGIPARSER_H_

#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include "../../Helper.h"

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::vector;
using std::map;

#define SERVER_KEY "server"

struct stScope
{
    string name;
    vector<string> args;
    map<string,vector<string> > values;
    vector<stScope> scopes;
};

class CgiParser {
private:
    vector<stScope> servers;

    void addScope(string key, vector<string> values,int index);
    void addValues(pair<string,vector<string> >, int index);
    char str_getlast(const string& str, int index);
    size_t getScopeEnd(const string& str);
public:
    vector<stScope> Parse(const string& str);
    void ParseScope(const string& str, int index);
    pair<string, vector<string> > ParseLine(const string& str);



};


#endif
