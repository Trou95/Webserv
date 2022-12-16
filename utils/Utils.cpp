#include "Utils.h"


size_t str_space_len(const char *str)
{
    int i = 0;

    while(str[i])
    {
        if(str[i] != ' ' && str[i] != '\t')
            break;
        i++;
    }
    return i;
}

std::vector<std::string> str_split(const std::string& str, char c)
{
    std::vector<std::string> res;
    std::string tmp;
    int index;
    int last_index = str_space_len(str.c_str());

    while((index = str.find(c, last_index)) != str.npos)
    {
        tmp = str.substr(last_index, index - last_index);
        res.push_back(tmp);
        last_index = index + str_space_len(&str[index]);
    }
    if(last_index < str.length() && res.size() > 0)
    {
        tmp = str.substr(last_index);
        res.push_back(tmp);
    }
    return res;
}