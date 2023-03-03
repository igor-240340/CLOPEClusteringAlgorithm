#include "Cluster.h"

unsigned int Cluster::NEXT_CLUSTER_ID = 0;

Cluster::Cluster() {
    id = NEXT_CLUSTER_ID++;
}

double Cluster::EstimateAdd(Transaction& t, double repulsion) {
    int newSize = size + t.items.size() - 1;
    int newWidth = itemToOccurence.size();
    for (const auto& i : t.items) {
        if (i == "p0" || i == "e0")
            continue;

        if (itemToOccurence.count(i) == 0) {
            newWidth++;
        }
    }

    double newQuality = (double)newSize * ((double)transactionCount + 1) / pow(newWidth, repulsion);
    return newQuality - quality;
}

void Cluster::Add(Transaction& t, double repulsion) {
    size = size + t.items.size() - 1;

    for (const auto& i : t.items) {
        if (i != "p0" && i != "e0")
            itemToOccurence[i]++;
    }

    transactionCount++;

    int width = itemToOccurence.size();
    quality = (double)size * (double)transactionCount / pow(width, repulsion);

    t.clusterId = id;

    isEmpty = false;
}

void Cluster::Remove(Transaction& t, double repulsion) {
    size = size - (t.items.size() - 1);
    isEmpty = --transactionCount == 0;

    for (const auto& i : t.items) {
        if (i == "p0" || i == "e0")
            continue;

        if (--itemToOccurence[i] == 0) {
            itemToOccurence.erase(i);
        }
    }

    int width = itemToOccurence.size();
    quality = (double)size * (double)transactionCount / pow(width, repulsion);
}
