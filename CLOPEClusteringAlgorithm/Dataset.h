#pragma once

#include <fstream>
#include <filesystem>

#include "Transaction.h"

//
// Абстрактный датасет.
// 
// Ожидаемый формат входного файла:
// каждая строка файла - транзакция,
// элементы транзакции разделены запятыми,
// каждый элемент уникален (имеет уникальный номер, например).
// 
// Каждый дочерний датасет должен реализовать PreProcess/PostProcess для приведения к указанному формату и для постобработки (если требуется
//
class Dataset
{
public:
    Dataset(std::string filePath);
    ~Dataset();

    bool ReadNextTransaction(Transaction& t);
    void WriteTransaction(Transaction& t);

    void Rewind();

protected:
    void Open();

    virtual void PreProcess() = 0;  // Предобработка входного файла в конструкторе конкретного датасета.
    virtual void PostProcess() = 0; // Постобработка выходного файла в деструкторе конкретного датасета.

    std::fstream fileIn;    // Копия оригинального файла.
    std::ofstream fileOut;  // Временный выходной файл с результатом.

private:
    std::filesystem::path fileInPath;
    std::filesystem::path fileOutPath;

    void Close();
};
