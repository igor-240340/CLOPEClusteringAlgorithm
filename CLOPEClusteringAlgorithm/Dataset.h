#pragma once

#include <fstream>
#include <filesystem>

#include "Transaction.h"

class Dataset
{
public:
    Dataset();
    Dataset(std::string fileName);
    ~Dataset();

    void OpenFile(std::string fileName, std::string postfix);
    void Close();
    void Reopen();

    bool ReadNextTransaction(Transaction& t);
    void WriteTransaction(Transaction& t);

private:
    std::fstream fileIn;
    std::ofstream fileOut;

    std::filesystem::path copyFilePath;
    std::filesystem::path tmpCopyFilePath;

    void Reformat(); // Присваивает уникальные номера значениям атрибутов.
};
