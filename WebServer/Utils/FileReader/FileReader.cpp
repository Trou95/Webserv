#include "FileReader.h"

string FileReader::readFile(const string& filepath)
{
    string res;
    ifstream file(filepath);

    if(file.is_open())
    {
        long long fileLen = getFileLen(filepath);
        char *buffer = new char[fileLen + 1];
        file.read(buffer,fileLen);
        buffer[fileLen] = '\0';
        res = buffer;
        delete[] buffer;
    }
    return res;
}

const string FileReader::getFileType(const string& filepath)
{
    size_t index = filepath.find_last_of(".");
    if(index != filepath.npos)
        return filepath.substr(index + 1);
    return "";
}

long long FileReader::getFileLen(const string& filepath) {
    std::ifstream in(filepath, std::ifstream::ate | std::ifstream::binary);
    long long result = in.tellg();
    in.close();
    return result;
}