#pragma once

#include <fstream>
#include <filesystem>

#include "Transaction.h"

class Dataset
{
public:
    Dataset(std::string filePath);
    ~Dataset();

    void Reopen();
    void Close();

    bool ReadNextTransaction(Transaction& t);
    void WriteTransaction(Transaction& t);

protected:
    void Open();

    std::fstream fileIn;    // Копия оригинального файла.
    std::ofstream fileOut;  // Временный выходной файл с результатом.

private:
    std::filesystem::path fileInPath;
    std::filesystem::path fileOutPath;

    virtual void Reformat() = 0; // Приводит конкретный файл датасета к унифицированному формату.
};
