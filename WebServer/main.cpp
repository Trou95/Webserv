#include <iostream>

#include "Malazgirt.h"

int main(int ac, char** av)
{
    Malazgirt malazgirt(av[1] ? av[1] : "Conf/default.conf");

    return 0;
}
