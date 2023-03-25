#include <unordered_map>

#include "CLOPEClusteringAlgorithm.h"

//
// ��������� ������������� � �������� dataset � ���������� repulsion.
//
unsigned short CLOPEClusteringAlgorithm::Apply(Dataset& dataset, const float repulsion) {
    unsigned short iterationCount = 0;
    std::unordered_map<int, Cluster*> idToCluster;
    Cluster emptyCluster;
    std::list<Cluster> clusters = { emptyCluster };

    // ��������� ������������� �� ��������� - ���� �������������.
    Transaction transac;
    while (dataset.ReadNextTransaction(transac)) {
        Cluster* maxCluster = FindMaxCluster(clusters, transac, repulsion);

        // ���� ������� ������� ��������������� ����������� � ������ �������,
        // �� ������� ����� �������, � ��������� ������ ��������� ���������� ��� ���������� ��������.
        if ((*maxCluster).IsEmpty()) {
            Cluster newCluster;
            newCluster.Add(transac, repulsion);
            clusters.push_back(newCluster);
            idToCluster[newCluster.id] = &clusters.back();
        }
        else {
            (*maxCluster).Add(transac, repulsion);
        }

        dataset.WriteTransaction(transac);
    }
    iterationCount++;

    // ���������� ������������ ������� �������.
    bool thereWasMove;  // ������� ����, ��� ���� �� ���� ���������� ���� ���������� ����� ���������� �� ����� ��������.
    do {
        thereWasMove = false;
        dataset.Reopen(); // ������������� �������� ���� � ���������� �������� ��� �������.

        while (dataset.ReadNextTransaction(transac)) {
            // ������� ���������� �� � �������� ��������.
            // ���� ������� ���� ������, �� ������� � ��� �������.
            Cluster& prevCluster = *(idToCluster[transac.clusterId]);
            int prevClusterId = prevCluster.id;
            prevCluster.Remove(transac, repulsion);
            bool prevClusterWasEmpty = false;
            if (prevCluster.IsEmpty()) {
                prevClusterWasEmpty = true;
                idToCluster.erase(prevClusterId);
                clusters.remove(prevCluster);
            }

            Cluster* maxCluster = FindMaxCluster(clusters, transac, repulsion);

            if ((*maxCluster).IsEmpty()) {
                Cluster newCluster;
                newCluster.Add(transac, repulsion);
                clusters.push_back(newCluster);
                idToCluster[newCluster.id] = &clusters.back();

                // ���� ������� ���������� ���� ������������ � ���������� ��������,
                // �� ���������� � � ����� ������ ������� ������������ ����������� � �������,
                // ��� � ���� ������� ������������ ���������� ����������� ���� ����������.
                if (!prevClusterWasEmpty) {
                    // NOTE:
                    // �� �� �������� � ��� ����� �� ������� �������� ��������.
                    // �� ���� �� ��������� ��������, ����� �������� ���������� �� ������������� �������� (������� �������� ���������� ������ ��� 1)
                    // � ��������� � � ����� ������ ������������� ������� �������.
                    thereWasMove = true;
                }
            }
            else {
                (*maxCluster).Add(transac, repulsion);

                // ���� ��������� ������� � ������� �������, �� ����������� �� ����.
                if ((*maxCluster).id != prevClusterId) {
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
// ���� �������, ���������� � ������� ��� ������������ ������� �������� �������� (� ������ � ������������ ������� ������� �������).
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
