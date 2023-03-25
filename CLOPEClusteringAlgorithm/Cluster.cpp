#include "Cluster.h"

unsigned int Cluster::nextClusterId = 0;

//
//
//
Cluster::Cluster() {
    id = nextClusterId++;
}

//
// Оценивает новое состояние кластера в случае добавления транзакции и возвращает дельту.
//
float Cluster::CalcDeltaQuality(Transaction& t, float repulsion) {
    int newSize = size + t.items.size();
    int newWidth = itemToOccurence.size();

    // Если данного айтема еще не было в этом кластере, то он увеличит ширину кластера на единицу.
    for (const auto& i : t.items) {
        if (itemToOccurence.count(i) == 0) {
            newWidth++;
        }
    }

    float newQuality = newSize * (transactionCount + 1) / pow(newWidth, repulsion);
    return newQuality - quality;
}

//
// Проставляет транзакции идентификатор кластера и пересчитывает параметры кластера.
//
void Cluster::Add(Transaction& t, float repulsion) {
    t.clusterId = id;

    isEmpty = false;
    transactionCount++;

    size = size + t.items.size();

    for (const auto& i : t.items) {
        itemToOccurence[i]++;
    }

    int width = itemToOccurence.size();
    quality = size * transactionCount / pow(width, repulsion);
}

//
// Пересчитывает параметры кластера с учетом, что транзакция в нём больше не находится.
//
void Cluster::Remove(Transaction& t, float repulsion) {
    size = size - t.items.size();
    isEmpty = --transactionCount == 0;

    for (const auto& i : t.items) {
        if (--itemToOccurence[i] == 0) {
            itemToOccurence.erase(i);
        }
    }

    if (isEmpty) {
        quality = 0.0f;
    }
    else {
        int width = itemToOccurence.size();
        quality = size * transactionCount / pow(width, repulsion);
    }
}
