#include <unordered_map>

#include "CLOPEClusteringAlgorithm.h"

//
// Применяет кластеризацию к датасету dataset с параметром repulsion.
//
unsigned short CLOPEClusteringAlgorithm::Apply(Dataset& dataset, const float repulsion) {
    std::unordered_map<unsigned int, Cluster*> idToCluster;
    std::list<Cluster> clusters = { Cluster() }; // Пустой кластер - для оценки прироста целевой функции при добавлении транзакции в новый кластер.
    unsigned short iterationCount = 0;
    bool thereWasMove; // Признак того, что хотя бы одна транзакция была перемещена между кластерами во время итерации.
    do {
        thereWasMove = false;
        bool initPhase = iterationCount == 0;   // На фазе инициализации добавление в любой кластер считается перемещением транзакции и thereWasMove устанавливается в true.

        // Переоткрываем выходной файл с предыдущей итерации как входной.
        if (!initPhase) {
            dataset.Reopen();
        }

        Transaction transac;
        while (dataset.ReadNextTransaction(transac)) {
            // Удаляем транзакцию из её текущего кластера.
            // Если кластер стал пустым, то удаляем и сам кластер.
            unsigned int prevClusterId;
            bool prevClusterWasEmpty;
            if (!initPhase) {
                Cluster& prevCluster = *(idToCluster[transac.clusterId]);
                prevClusterId = prevCluster.id;
                prevCluster.Remove(transac, repulsion);
                prevClusterWasEmpty = false;

                if (prevCluster.IsEmpty()) {
                    prevClusterWasEmpty = true;
                    idToCluster.erase(prevClusterId);
                    clusters.remove(prevCluster);
                }
            }

            Cluster* maxCluster = FindMaxCluster(clusters, transac, repulsion);

            // Если целевая функция максимизируется добавлением в пустой кластер,
            // то создаем новый кластер, а имеющийся пустой оставляем нетронутым для дальнейших проверок.
            if ((*maxCluster).IsEmpty()) {
                Cluster newCluster;
                newCluster.Add(transac, repulsion);
                clusters.push_back(newCluster);
                idToCluster[newCluster.id] = &clusters.back();

                // Если текущая транзакция была единственной в предыдущем кластере,
                // то добавление её в новый пустой кластер эквивалентно возвращению в прежний,
                // что в свою очередь эквивалентно отсутствию перемещения этой транзакции.
                if (initPhase || !prevClusterWasEmpty) {
                    // NOTE:
                    // Мы не попадаем в эту ветку на текущем тестовом датасете по условию !prevClusterWasEmpty на фазе максимизации.
                    // То есть не возникает ситуация, когда удаление транзакции из существующего кластера (который содержит транзакций больше чем 1)
                    // и помещение её в новый пустой максимизирует целевую функцию.
                    thereWasMove = true;
                }
            }
            else {
                (*maxCluster).Add(transac, repulsion);

                // Если произошел возврат в прежний кластер, то перемещения не было.
                if (initPhase || (*maxCluster).id != prevClusterId) {
                    thereWasMove = true;
                }
            }

            dataset.WriteTransaction(transac);
        }

        iterationCount++;
    } while (thereWasMove);

    return iterationCount;
}

//
// Ищет кластер, добавление в который даёт максимальный прирост качества кластера (а значит и максимальный прирост целевой функции).
//
Cluster* CLOPEClusteringAlgorithm::FindMaxCluster(std::list<Cluster>& clusters, Transaction& transaction, const float repulsion) {
    Cluster* maxCluster = nullptr;
    float deltaQualityMax = 0;
    for (Cluster& cluster : clusters) {
        float deltaQuality = cluster.CalcDeltaQuality(transaction, repulsion);

        if (deltaQuality > deltaQualityMax) {
            deltaQualityMax = deltaQuality;
            maxCluster = &cluster;
        }
    }

    return maxCluster;
}
