#include <iostream>
#include <stdarg.h>

void Log(const char *format, ...)
{
    char buffer[256];
    va_list args;

    va_start (args, format);
    vsprintf(buffer,format, args);
    std::cout << buffer << std::endl;
    va_end (args);
}