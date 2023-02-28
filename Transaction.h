#pragma once

#include <iostream> 
#include <list> 

class Transaction
{
public:
    int clusterId;

    std::list<std::string> items;

    bool RemoveFromCurrentCluster();
};
