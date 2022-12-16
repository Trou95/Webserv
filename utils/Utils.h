#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstddef>
#include <iostream>
#include <vector>

size_t str_space_len(const char* str);
std::vector<std::string> str_split(const std::string& str, char c);

void Log(const char *format, ...);

#endif
