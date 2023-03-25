#pragma once

#include <fstream>
#include <filesystem>

#include "Transaction.h"

class Dataset
{
public:
    Dataset(std::string fileName);
    ~Dataset();

    void Reopen();
    void Close();

    bool ReadNextTransaction(Transaction& t);
    void WriteTransaction(Transaction& t);

private:
    std::fstream fileIn;
    std::ofstream fileOut;

    std::filesystem::path copyFilePath;
    std::filesystem::path tmpCopyFilePath;

    void Reformat(); // Присваивает уникальные номера значениям атрибутов.
};
