#include "Cluster.h"

unsigned int Cluster::NEXT_CLUSTER_ID = 0;

Cluster::Cluster() {
    id = NEXT_CLUSTER_ID++;
}

float Cluster::EstimateAdd(Transaction& t, float repulsion) {
    int newSize = size + t.items.size();
    int newWidth = itemToOccurence.size();
    for (const auto& i : t.items) {
        if (itemToOccurence.count(i) == 0) {
            newWidth++;
        }
    }

    float newQuality = newSize * (transactionCount + 1) / pow(newWidth, repulsion);
    return newQuality - quality;
}

void Cluster::Add(Transaction& t, float repulsion) {
    for (const auto& i : t.items) {
        itemToOccurence[i]++;
        size++;
    }

    transactionCount++;

    int width = itemToOccurence.size();
    quality = size * transactionCount / pow(width, repulsion);

    t.clusterId = id;

    isEmpty = false;
}
