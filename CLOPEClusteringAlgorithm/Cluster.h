#pragma once

#include <unordered_map>

#include "Transaction.h"

class Cluster
{
public:
    Cluster();

    double CalcDeltaQuality(Transaction& t, double repulsion);   // Оценивает изменение качества кластера при добавлении транзакции.
    void Add(Transaction& t, double repulsion);
    void Remove(Transaction& t, double repulsion);

    bool IsEmpty() { return isEmpty; };

    // Для list.remove().
    bool operator == (const Cluster& c) const { return id == c.id; }
    bool operator != (const Cluster& c) const { return !operator==(c); }

    unsigned int id;

private:
    static unsigned int nextClusterId;

    bool isEmpty = true;

    // Определяют качество кластера: (size / width^repulsion) * transactionCount.
    int transactionCount = 0;
    int size = 0;                                           // Количество всех item (сумма айтемов по всем транзакциям кластера).
    std::unordered_map<std::string, int> itemToOccurence;   // Количество элементов определяет ширину кластера.
    double quality = 0.0f;
};
