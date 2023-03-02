#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <unordered_map>

#include "Dataset.h"
#include "Cluster.h"
#include "Transaction.h"

static const float REPULSION = 2.6f;

int main()
{
    Dataset data;
    data.OpenFile("agaricus-lepiota.data", "agaricus-lepiota_1.data");

    std::unordered_map<int, Cluster*> idToCluster;
    Cluster emptyCluster;
    std::list<Cluster> clusters = { emptyCluster }; // В конце списка всегда будем держать пустой кластер для оценки Profit.
    Transaction t;

    // Инициализация.
    while (data.ReadNextTransaction(t)) {
        Cluster* max = nullptr; // Кластер, добавление в который текущей транзакции максимизирует Profit.
        double dQualityMax = 0;
        for (auto& c : clusters) {
            double dQuality = c.EstimateAdd(t, REPULSION);
            //std::cout << "quality change: " << dQuality << std::endl;

            if (dQuality > dQualityMax) {
                dQualityMax = dQuality;
                max = &c;
            }
        }

        // Если Profit максимизируется добавлением в пустой кластер,
        // то создаем новый кластер, а пустой оставляем нетронутым для дальнейших проверок.
        if ((*max).IsEmpty()) {
            Cluster newClaster;
            newClaster.Add(t, REPULSION);
            clusters.push_back(newClaster);
            idToCluster[newClaster.id] = &clusters.back();
        }
        else {
            (*max).Add(t, REPULSION);
        }

        data.WriteTransaction(t);
    }

    data.CloseFiles();

    // Максимизация Profit.
    bool moved = false;
    int sfix = 1;
    do {
        data.OpenFile("agaricus-lepiota_" + std::to_string(sfix) + ".data", "agaricus-lepiota_" + std::to_string(sfix + 1) + ".data");

        while (data.ReadNextTransaction(t)) {
            Cluster& prevCluster = *(idToCluster[t.clusterId]);
            prevCluster.Remove(t, REPULSION);
            int prevClusterId = prevCluster.id;
            bool prevClusterWasEmpty;
            if (prevCluster.IsEmpty()) {
                prevClusterWasEmpty = true;
                idToCluster.erase(prevClusterId);
                clusters.remove(prevCluster);
            }

            Cluster* max = nullptr;
            double dQualityMax = 0;
            for (auto& c : clusters) {
                double dQuality = c.EstimateAdd(t, REPULSION);
                //std::cout << "quality change: " << dQuality << std::endl;

                if (dQuality > dQualityMax) {
                    dQualityMax = dQuality;
                    max = &c;
                }
            }

            if ((*max).IsEmpty()) {
                Cluster newClaster;
                newClaster.Add(t, REPULSION);
                clusters.push_back(newClaster);

                if (!prevClusterWasEmpty) {
                    moved = true;
                }
            }
            else {
                (*max).Add(t, REPULSION);

                if ((*max).id != prevClusterId) {
                    moved = true;
                }
            }

            data.WriteTransaction(t);
        }

        data.CloseFiles();
        sfix++;
        moved = false;
    } while (moved);
}
