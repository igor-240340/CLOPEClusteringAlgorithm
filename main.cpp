#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <unordered_map>

#include "Dataset.h"
#include "Cluster.h"
#include "Transaction.h"

//static const double REPULSION = 2.6;
static const double REPULSION = 2;

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
            if (t.sourceString == "e0,x1,s2,c3,t4,n5,f6,c7,b8,w9,e10,b11,s12,s13,w14,w15,p16,w17,t18,p19,w20,v21,p22,18")
                std::cout << "h";

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

    data.OpenFile("agaricus-lepiota_" + std::to_string(sfix) + ".data", "agaricus-lepiota_" + std::to_string(sfix + 1) + ".data");
    std::unordered_map<int, int> clusterToTransactionCount;
    while (data.ReadNextTransaction(t)) {
        clusterToTransactionCount[t.clusterId]++;
    }
    
    for (auto entry : clusterToTransactionCount) {
        auto k = entry.first;
        auto x = entry.second;
        std::cout << k << ": " << x << "\n";
    }

    data.CloseFiles();
}
