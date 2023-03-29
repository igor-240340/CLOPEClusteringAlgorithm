#pragma once

#include <list>

#include "Dataset.h"
#include "Cluster.h"
#include "Transaction.h"

class CLOPEClusteringAlgorithm
{
public:
    static unsigned short Apply(Dataset& dataset, const double repulsion);

private:
    static Cluster* FindMaxCluster(std::list<Cluster>& clusters, Transaction& transaction, const double repulsion);
};
