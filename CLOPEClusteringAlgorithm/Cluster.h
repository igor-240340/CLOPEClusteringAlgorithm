#pragma once

#include <unordered_map>

#include "Transaction.h"

class Cluster
{
public:
    Cluster();

    float CalcDeltaQuality(Transaction& t, float repulsion);   // Оценивает изменение качества кластера при добавлении транзакции.
    void Add(Transaction& t, float repulsion);
    void Remove(Transaction& t, float repulsion);

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
    float quality = 0.0f;
};
