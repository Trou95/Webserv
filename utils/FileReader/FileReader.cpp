#include "FileReader.h"

std::string FileReader::readFile(std::string filepath)
{
    std::string res;
    std::ifstream file(filepath);

    if(file.is_open())
    {
        long long fileLen = getFileLen(filepath);
        char *buffer = new char[fileLen + 1];
        file.read(buffer,fileLen);
        res = buffer;
        delete[] buffer;
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

long long FileReader::getFileLen(const std::string filepath) {
    std::ifstream in(filepath, std::ifstream::ate | std::ifstream::binary);
    auto result = in.tellg();
    in.close();
    return result;
}