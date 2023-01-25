#include "CgiParser.h"

vector<stScope> CgiParser::Parse(const string& str)
{
    int last_index = 0;

    for(int i = 0, len = str.length(); i < len; i++)
    {
        if(str[i] == '\n')
        {
            if(str_getlast(str,i) == '{')
            {
                size_t end_index = i + getScopeEnd(&str[i]);
                if(end_index == str.npos)
                    return vector<stScope>();
                vector<string> tmpstr = str_split(str.substr(last_index, i - last_index), ' ');
                if(tmpstr[0] == SERVER_KEY) {
                    addScope(tmpstr[0],vector<string>(tmpstr.begin() + 1,tmpstr.end()),0);
                    ParseScope(str.substr(i + 1, end_index - i),0);
                }
                i = end_index + 1;
                last_index = i + 1;
            }
        }
    }

    return servers;
}

void CgiParser::ParseScope(const string &str,int index)
{
    int last_index = 0;
    for(int i = 0,len = str.length(); i < len; i++)
    {
        if(str[i] == '\n')
        {
            if(str_getlast(str,i) == '{')
            {
                size_t end_index = i + getScopeEnd(&str[i + 1]);
                if(end_index == str.npos)
                    return;
                vector<string> tmpstr = str_split(str.substr(last_index, i - last_index), ' ');
                addScope(tmpstr[0],vector<string>(tmpstr.begin() + 1, tmpstr.end()), 1);
                ParseScope(str.substr(i, end_index - i), 1);
                i = end_index + 1;
                last_index = i;
            }
            else
            {
                addValues(ParseLine(str.substr(last_index, i - last_index)), index);
                last_index = i + 1;
            }
        }
    }
}

pair<string,vector<string> > CgiParser::ParseLine(const string &str)
{
    vector<string> tmp = str_split(str,' ');
    if(tmp.size() > 0)
        return pair<string,vector<string> >(tmp[0], vector<string>(tmp.begin() + 1, tmp.end()));
    else
        return pair<string,vector<string> >();
}

void CgiParser::addScope(string key, vector <string> values, int index)
{
    stScope tmp;
    tmp.name = key;
    tmp.args = values;
    if(index == 0)
        servers.push_back(tmp);
    else
        servers[servers.size() - 1].scopes.push_back(tmp);
}

void CgiParser::addValues(pair <string, vector<string> > values, int index)
{
    size_t len = servers.size();

    if(values.first.length() == 0)
        return;
    if(index == 0)
        servers[len - 1].values.insert(values);
    else
        servers[len - 1].scopes[servers[len - 1].scopes.size() - 1].values.insert(values);

}

char CgiParser::str_getlast(const string &str, int index)
{
    while(str[index])
    {
        if(isprint(str[index]))
            break;
        index--;
    }
    return str[index];
}

size_t CgiParser::getScopeEnd(const string &str)
{
    for(int i = 0; str[i]; i++)
    {
        if(str[i] == '{') {
            size_t index = str.find_first_of("}",i + 1);
            if(index == str.npos)
                return str.npos;
            i = index + 1;
        }
        else if(str[i] == '}')
            return i;
    }
    return str.npos;
}
