#pragma once

#include "Transaction.h"

class Cluster
{
public:
    // Мера качества кластера. Определена как градиент на мощность кластера: G(C) * |C|.
    float quality;

    // Оценивает изменение качества кластера в случае добавления транзакции.
    float EstimateAdd(Transaction&) const;
    void Add(Transaction&);
};
