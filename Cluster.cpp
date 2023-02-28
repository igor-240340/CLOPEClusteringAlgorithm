#include "Cluster.h"

float Cluster::EstimateAdd(Transaction& t) {
    int newSize = size + t.items.size();
    int newWidth = width;
    for (const auto& i : t.items) {
        if (itemToOccurence.count(i) == 0)
            newWidth++;
    }

    float newQuality = newSize * (transactionCount + 1) / pow(newWidth, 2.6f);
    return newQuality - quality;
}

void Cluster::Add(Transaction& t) {
    std::cout << "Cluster::Add" << std::endl;

    isEmpty = false;
}
