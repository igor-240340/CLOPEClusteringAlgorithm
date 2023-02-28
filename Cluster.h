#pragma once

#include <unordered_map>

#include "Transaction.h"

class Cluster
{
public:
    Cluster();

    float EstimateAdd(Transaction&, float repulsion);   // Оценивает изменение качества кластера при добавлении транзакции.
    void Add(Transaction&, float repulsion);
    bool IsEmpty() { return isEmpty; };

    unsigned int id;

private:
    static unsigned int NEXT_CLUSTER_ID;

    bool isEmpty = true;

    // Мера качества кластера: (S * |C|) / W^repulsion.
    float quality = 0;

    int transactionCount = 0;
    int size = 0;   // Количество всех item (сумма по каждой транзакции).
    int width = 0;  // Количество уникальных item.

    std::unordered_map<std::string, int> itemToOccurence;
};
