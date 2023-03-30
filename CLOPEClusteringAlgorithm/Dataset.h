#pragma once

#include <fstream>
#include <filesystem>

#include "Transaction.h"

//
// Абстрактный датасет.
// 
// Ожидаемый формат входного файла:
// <item_1>,<item_2>,...,<item_n>,[<cluster_id>]
// ...
// <item_1>,<item_2>,...,<item_n>,[<cluster_id>]
// 
// Каждый дочерний датасет должен реализовать методы PreProcess и PostProcess для приведения датасета к требуемому входному и желаемому выходному формату.
// Если дополнительной обработки датасета не требуется, то реализации можно оставить пустыми.
//
class Dataset
{
public:
    Dataset(std::string filePath);
    ~Dataset();

    bool ReadNextTransaction(Transaction& t);
    void WriteTransaction(Transaction& t);

    void Reopen();

protected:
    void Open();

    virtual void PreProcess() = 0;  // Предобработка входного файла в конструкторе конкретного датасета.
    virtual void PostProcess() = 0; // Постобработка выходного файла в деструкторе конкретного датасета.

    std::fstream fileIn;    // Копия оригинального файла.
    std::ofstream fileOut;  // Временный выходной файл с результатом.

    // Путь к исходному файлу, с копией которого работает датасет.
    // Может быть использован в PostProcess для сопоставления кластеров с исходными данными.
    std::string sourceFilePath;

private:
    std::filesystem::path fileInPath;
    std::filesystem::path fileOutPath;

    void Close();
};
