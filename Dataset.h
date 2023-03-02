#pragma once

#include <fstream>

#include "Transaction.h"

class Dataset
{
public:
    Dataset();
    ~Dataset();

    void OpenFile(std::string fileName, std::string postfix);
    void CloseFiles();

    bool ReadNextTransaction(Transaction& t);
    void WriteTransaction(Transaction& t);
    void Rewind();

private:
    std::fstream fileIn;
    std::ofstream fileOut;
};
