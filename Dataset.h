#pragma once

#include <fstream>

#include "Transaction.h"

class Dataset
{
public:
    Dataset(std::string fileName);
    ~Dataset();

    bool ReadNextTransaction(Transaction&);
    void WriteTransaction(Transaction&);
    void Rewind();

private:
    std::fstream fileIn;
    std::ofstream fileOut;
};
