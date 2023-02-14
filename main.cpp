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

    float r = 2.6;  // repulsion.
    float profit = 0;   // Качество кластеризации Profit(C).

    Dataset data;
    std::list<Cluster> clusters;
    Transaction t;

    Cluster dummy;

    // Инициализация.
    while (data.ReadNextTransaction(t)) {
        std::cout << "Transaction: " << t.name << std::endl;

        Cluster& max = dummy;
        for (const auto& c : clusters) {
            float dQuality = c.EstimateAdd(t);
            std::cout << "delta: " << dQuality << std::endl;
        }

        max.Add(t);

        data.WriteTransaction(t);
    }

    do {
        while (data.ReadNextTransaction(t)) {
        }
    } while (true);
}
