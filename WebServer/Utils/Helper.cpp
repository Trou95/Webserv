#include "Helper.h"

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

int str_get_chrindex(const char *str, char c)
{
    int	i;

    i = 0;
    while (str[i])
    {
        if(str[i] == c)
            return i;
        i++;
    }
    return (-1);
}

char	*str_join(char const *s1, char const *s2)
{
    char	*ptr = new char[strlen(s1) + strlen(s2) + 1];

    int i = 0;
    for(int n = 0; s1[n]; n++)
        ptr[i++] = s1[n];
    for(int n = 0; s2[n] >= 32; n++)
        ptr[i++] = s2[n];
    ptr[i] = '\0';
    return (ptr);
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
        size_t len = str_space_len(&str[index]);
        last_index = index + ((len > 0) ? len : 1);
    }
    if(last_index < str.length() && res.size() > 0)
    {
        tmp = str.substr(last_index);
        res.push_back(tmp);
    }
    return res;
}

std::string str_trim(const std::string& str, char c)
{
    int start = 0;
    int end = str.length() - 1;

    while(str[start] == ' ')
        start++;
    while(str[end] == ' ' || str[end] == '\r')
        end--;
    return str.substr(start,end- start + 1);
}

int Log(const char *type, const char* format, ...)
{
    char buffer[MAX_LOG_LEN];
    int index;
    va_list args;

    va_start (args, format);
    vsprintf (buffer,format, args);
    index = str_get_chrindex(buffer,':');
    if(index > -1)
    {
        std::string tmp(buffer);

        if(!strcmp(type,LOG_INFO))
            std::cout << "\x1B[32m";
        else if(!strcmp(type,LOG_ERROR))
            std::cout << "\x1B[31m";
        else
            std::cout << type;
        std::cout << tmp.substr(0,index + 1);
        std::cout << "\x1B[0m" << tmp.substr(index + 1);
    }
    else
        std::cout << buffer;
    std::cout << std::endl;
    va_end (args);

    return 0;
}