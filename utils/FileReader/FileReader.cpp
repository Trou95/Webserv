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

string FileReader::readFileBinary(const string &filePath)
{
    string res;
    ifstream file(filePath, std::ios::ate | std::ios::binary);

    if(file.is_open())
    {
        fpos_t size = file.tellg();
        char *buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer,size);
        file.close();
        for(fpos_t i = 0; i < size; i++)
            res += buffer[i];
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