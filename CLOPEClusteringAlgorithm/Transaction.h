#pragma once

#include <string>
#include <list>

struct Transaction
{
    unsigned int clusterId;
    std::list<std::string> items;
};
