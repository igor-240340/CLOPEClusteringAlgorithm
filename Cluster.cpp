#include "Cluster.h"

unsigned int Cluster::NEXT_CLUSTER_ID = 0;

Cluster::Cluster() {
    id = NEXT_CLUSTER_ID++;
}

double Cluster::EstimateAdd(Transaction& t, double repulsion) {
    int newSize = size + t.items.size();
    int newWidth = itemToOccurence.size();
    for (const auto& i : t.items) {
        if (itemToOccurence.count(i) == 0) {
            newWidth++;
        }
    }

    double newQuality = (double)newSize * (double)(transactionCount + 1) / pow(newWidth, repulsion);
    return newQuality - quality;
}

void Cluster::Add(Transaction& t, double repulsion) {
    for (const auto& i : t.items) {
        itemToOccurence[i]++;
        size++;
    }

    transactionCount++;

    int width = itemToOccurence.size();
    quality = (double)size * transactionCount / pow(width, repulsion);

    t.clusterId = id;

    isEmpty = false;
}

void Cluster::Remove(Transaction& t, double repulsion) {
    size = size - t.items.size();
    isEmpty = --transactionCount == 0;

    for (const auto& i : t.items) {
        if (--itemToOccurence[i] == 0) {
            itemToOccurence.erase(i);
        }
    }

    int width = itemToOccurence.size();
    quality = (double)size * transactionCount / pow(width, repulsion);
}
