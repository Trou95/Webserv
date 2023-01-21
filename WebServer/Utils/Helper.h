#pragma once

#include <cstring>
#include <cstdarg>
#include <iostream>
#include <vector>


#define MAX_LOG_LEN 256


#define LOG_INFO "1"
#define LOG_ERROR "2"


size_t str_space_len(const char* str);
int str_get_chrindex(const char* str, char c);
std::string str_clear(const std::string& str);
char *str_join(const char* s1, const char* s2);
std::vector<std::string> str_split(const std::string& str, char c);
std::string str_trim(const std::string& str, char c);

int Log(const char *type, const char* format, ...);


