#pragma once

#include <iostream> 
#include <list> 

class Transaction
{
public:
    unsigned int clusterId;

    std::list<std::string> items;

    std::string sourceString;
};
