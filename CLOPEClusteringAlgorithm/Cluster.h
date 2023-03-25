#pragma once

#include <unordered_map>

#include "Transaction.h"

class Cluster
{
public:
    Cluster();

    float CalcDeltaQuality(Transaction& t, float repulsion);   // ��������� ��������� �������� �������� ��� ���������� ����������.
    void Add(Transaction& t, float repulsion);
    void Remove(Transaction& t, float repulsion);

    bool IsEmpty() { return isEmpty; };

    // ��� list.remove().
    bool operator == (const Cluster& c) const { return id == c.id; }
    bool operator != (const Cluster& c) const { return !operator==(c); }

    unsigned int id;

private:
    static unsigned int nextClusterId;

    bool isEmpty = true;

    // ���������� �������� ��������: (size / width^repulsion) * transactionCount.
    int transactionCount = 0;
    int size = 0;                                           // ���������� ���� item (����� ������� �� ���� ����������� ��������).
    std::unordered_map<std::string, int> itemToOccurence;   // ���������� ��������� ���������� ������ ��������.
    float quality = 0.0f;
};
