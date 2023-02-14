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

    float clusterizationProfit = 0;

    Dataset data;
    std::list<Cluster> clusters;
    Transaction t;

    // Инициализация.
    while (data.ReadNextTransaction(t)) {
        std::cout << "Transaction: " << t.name << std::endl;

        for (const auto& c : clusters) {
            float d = c.EstimateDeltaQuality(t);
            std::cout << "delta: " << d << std::endl;
        }

        data.WriteTransaction(t);
    }

    do {
        while (data.ReadNextTransaction(t)) {
        }
    } while (true);
}
