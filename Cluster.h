#pragma once

#include <unordered_map>

#include "Transaction.h"

class Cluster
{
public:
    Cluster();

    double EstimateAdd(Transaction& t, double repulsion);   // Оценивает изменение качества кластера при добавлении транзакции.
    void Add(Transaction& t, double repulsion);
    void Remove(Transaction& t, double repulsion);
    bool IsEmpty() { return isEmpty; };

    // Для list.remove().
    bool operator == (const Cluster& c) const { return id == c.id; }
    bool operator != (const Cluster& c) const { return !operator==(c); }

    unsigned int id;

private:
    static unsigned int NEXT_CLUSTER_ID;

    bool isEmpty = true;

    // Мера качества кластера: (S * |C|) / W^repulsion.
    double quality = 0;

    int transactionCount = 0;
    int size = 0;   // Количество всех item (сумма по каждой транзакции).
    int width = 0;  // Количество уникальных item.

    std::unordered_map<std::string, int> itemToOccurence;
};
