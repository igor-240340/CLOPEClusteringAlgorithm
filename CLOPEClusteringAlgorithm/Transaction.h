#pragma once

#include <string>
#include <list>

struct Transaction
{
    unsigned short clusterId;
    std::list<std::string> items;
};
