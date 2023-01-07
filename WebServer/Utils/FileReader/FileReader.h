#ifndef _FILEREADER_H_
#define _FILEREADER_H_

#include <iostream>
#include <fstream>

using std::string;
using std::ifstream;

class FileReader {

    private:

        static long long getFileLen(const string& filepath);


    public:
        static string readFile(const string& filepath);
        static const string getFileType(const string& filepath);

};


#endif