#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include <iostream>
#include <fstream>

class FileReader{
private:
    static long long getFileLen(const std::string filepath);
public:
    static std::string readFile(const std::string filepath);
    static std::string getFileType(std::string filepath);

};


#endif