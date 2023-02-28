#include <iostream>
#include <fstream>
#include <list>

#include "Dataset.h"
#include "Cluster.h"
#include "Transaction.h"

static const float REPULSION = 2.6f;

int main()
{
    Dataset data("agaricus-lepiota.data");

    Cluster emptyCluster;
    std::list<Cluster> clusters = { emptyCluster }; // В конце списка всегда будем держать пустой кластер для оценки Profit.
    Transaction t;

    // Инициализация.
    while (data.ReadNextTransaction(t)) {
        Cluster* max = nullptr; // Кластер, добавление в который текущей транзакции максимизирует Profit.
        float dQualityMax = 0;
        for (auto& c : clusters) {
            float dQuality = c.EstimateAdd(t, REPULSION);
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
        }
        else {
            (*max).Add(t, REPULSION);
        }

        data.WriteTransaction(t);
    }

    // Максимизация Profit.
    //bool moved = false;
    //do {
    //    data.rewind();
    //    while (data.readnexttransaction(t)) {
    //        int prevclusterid = t.clusterid;
    //        bool prevclusterempty = t.removefromcurrentcluster();

    //        cluster* max = nullptr;
    //        float dqualitymax = 0;
    //        for (auto& c : clusters) {
    //            float dquality = c.estimateadd(t);
    //            std::cout << "delta: " << dquality << std::endl;

    //            if (dquality > dqualitymax) {
    //                dqualitymax = dquality;
    //                max = &c;
    //            }
    //        }

    //        if ((*max).isempty()) {
    //            cluster newclaster;
    //            newclaster.add(t);
    //            clusters.push_back(newclaster);

    //            // если транзакция до этого была единственной в своем кластере, 
    //            // то добавление её в пустой кластер эквивалентно возврату в прежний: то есть, перемещения нет.
    //            if (!prevclusterempty)
    //                moved = true;
    //        }
    //        else {
    //            (*max).add(t);
    //            if ((*max).clusterid != prevclusterid)
    //                moved = true;
    //        }

    //        data.writetransaction(t);
    //    }
    //} while (moved);
}
