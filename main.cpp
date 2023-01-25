#include <iostream>

#include "Malazgirt.h"

#include "Utils/FileReader/FileReader.h"
#include "Utils/Parser/Request/RequestParser.h"

#include <fstream>



int main(int ac, char** av)
{
    Malazgirt malazgirt(av[1] ? av[1] : "Conf/default.conf");

    
    return 0;
}
