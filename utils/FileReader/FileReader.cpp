#include "FileReader.h"

std::string FileReader::readFile(std::string filepath)
{
    std::string res;
    std::ifstream file(filepath);

    if(file.is_open())
    {
        std::string tmp;
        while(std::getline(file,tmp))
            res.append(tmp);
    }
    return res;
}

std::string FileReader::getFileType(std::string filepath)
{
    size_t index = filepath.find_last_of(".");
    if(index != filepath.npos)
        return filepath.substr(index + 1);
    return "";
}