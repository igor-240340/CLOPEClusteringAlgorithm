#pragma once

#include "Transaction.h"

class Dataset
{
    /*Dataset();
    ~Dataset();*/

public:
    bool ReadNextTransaction(Transaction&);
    void WriteTransaction(Transaction&);
    void Rewind();
};
