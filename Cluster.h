#pragma once

#include <unordered_map>

#include "Transaction.h"

class Cluster
{
public:
    int clusterId;

    // Оценивает изменение качества кластера при добавлении транзакции.
    float EstimateAdd(Transaction&);
    void Add(Transaction&);

    bool IsEmpty() { return isEmpty; };

private:
    bool isEmpty = true;

    // Мера качества кластера. (S * |C|) / W^r.
    float quality = 0;
    int transactionCount = 0;
    int size = 0;
    int width = 0;

    std::unordered_map<std::string, int> itemToOccurence;
};
