#pragma once

#include <iostream> 
#include <list> 

struct Transaction
{
    unsigned short clusterId;
    std::list<std::string> items;
};
