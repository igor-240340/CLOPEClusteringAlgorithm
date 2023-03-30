#pragma once

#include <list>

#include "Dataset.h"
#include "Cluster.h"
#include "Transaction.h"

class CLOPEClusteringAlgorithm
{
public:
    static unsigned short Perform(Dataset& dataset, const float repulsion, const unsigned short iterationsMax = 1000);

private:
    static Cluster* FindMaxCluster(std::list<Cluster>& clusters, Transaction& transaction, const float repulsion);
};
