#include <iostream>
#include <fstream>
#include <list>

#include "Dataset.h"
#include "Cluster.h"
#include "Transaction.h"

int main()
{
    /*std::ifstream inFile("Data/agaricus-lepiota.data");
    if (!inFile) {
        std::cout << "Can't open file." << std::endl;
        exit(1);
    }*/

    // Инициализация
    /*std::string transaction;
    while (inFile >> transaction) {
        std::cout << transaction << std::endl;
    }*/

    //inFile.close();

    float r = 2.6;      // repulsion.
    float profit = 0;   // Качество кластеризации Profit(C).

    Dataset data;
    Cluster emptyCluster;
    std::list<Cluster> clusters = { emptyCluster }; // В конце списка всегда будем держать пустой кластер для оценки Profit.
    Transaction t;

    Cluster dummy;

    // Инициализация.
    while (data.ReadNextTransaction(t)) {
        std::cout << "Transaction: " << t.name << std::endl;

        Cluster& max = dummy;   // Кластер, добавление в который текущей транзакции максимизирует Profit.
        float dQualityMax = 0;
        for (const auto& c : clusters) {
            float dQuality = c.EstimateAdd(t);
            std::cout << "delta: " << dQuality << std::endl;

            if (dQuality > dQualityMax) {
                dQualityMax = dQuality;
                max = c;
            }
        }

        // Если Profit максимизируется добавлением в пустой кластер,
        // то создаем новый кластер, а пустой оставляем нетронутым для дальнейших проверок.
        if (max.IsEmpty()) {
            Cluster newClaster;
            newClaster.Add(t);
            clusters.push_back(newClaster);
        }
        else {
            max.Add(t);
        }

        data.WriteTransaction(t);
    }

    // Максимизация Profit.
    bool moved = false;
    do {
        data.Rewind();
        while (data.ReadNextTransaction(t)) {
            // Удалить транзакцию из кластера

            Cluster& max = dummy;
            float dQualityMax = 0;
            for (const auto& c : clusters) {
                float dQuality = c.EstimateAdd(t);
                std::cout << "delta: " << dQuality << std::endl;

                if (dQuality > dQualityMax) {
                    dQualityMax = dQuality;
                    max = c;
                }
            }

            if (max.IsEmpty()) {
                Cluster newClaster;
                newClaster.Add(t);
                clusters.push_back(newClaster);
            }
            else
                max.Add(t);

            // Если транзакция не возвращается в свой прежний кластер, то
            // moved = true;

            data.WriteTransaction(t);
        }
    } while (moved);
}
