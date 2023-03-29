#include <iostream>
#include <sstream>

#include "Dataset.h"

//
// Создает копию оригинального файла датасета.
// Открывает копию на чтение, а временный файл - на запись.
//
Dataset::Dataset(std::string filePath) {
    std::string origFileName = std::filesystem::path(filePath).filename().string();
    fileInPath = std::filesystem::path(filePath).replace_filename("copy_" + origFileName);
    std::filesystem::copy(filePath, fileInPath);

    fileOutPath = std::filesystem::path(filePath).replace_filename("tmp_copy_" + origFileName);
}

//
//
//
Dataset::~Dataset() {
    Close();

    // После последней итерации, на которой не было зафиксировано ни одного перемещения между кластерами,
    // выходной файл окажется эквивалентен входному.
    // Эквивалентен он будет в смысле распределения транзакций по кластерам, но конкретные номера кластеров могут тем не менее измениться.
    // В любом случае смысла менять эти файлы местами нет и мы просто оставляем результат предыдущей кластеризации а новый выходной файл удаляем.
    std::filesystem::remove(fileOutPath);
}

//
// Читает следующую строку датасета и формирует список значений атрибутов (items).
// Если у транзакции есть номер кластера, то проставляет его.
// Ожидаемый формат датасета: каждая строка - транзакция, элементы разделены запятой, без пробелов, в конце, через запятую, указан номер кластера.
// Номер кластера может (и будет) отсутствовать при первом чтении на фазе инициализации.
//
bool Dataset::ReadNextTransaction(Transaction& t) {
    t.items.clear();

    std::string line;
    fileIn >> line;

    if (line == "")
        return false;

    std::stringstream ss(line);
    while (ss.good()) {
        std::string itemValue;
        std::getline(ss, itemValue, ',');

        // Если считанное значение начинается с цифры, значит перед нами номер кластера, а не значение атрибута.
        if (itemValue.c_str()[0] >= 48 && itemValue.c_str()[0] <= 57) {
            t.clusterId = std::stoi(itemValue.c_str(), nullptr, 10);
        }
        else {
            t.items.push_back(itemValue);
        }
    }

    return true;
}

//
// Записывает транзакцию в выходной файл с присвоенным номером кластера.
//
void Dataset::WriteTransaction(Transaction& t) {
    for (const auto& i : t.items) {
        fileOut << i << ",";
    }

    fileOut << std::to_string(t.clusterId) << std::endl;
}

//
// Удаляет входной файл, переименовывает временный выходной и открывает его как входной.
//
void Dataset::Rewind() {
    Close();

    std::filesystem::remove(fileInPath);
    std::filesystem::rename(fileOutPath, fileInPath);

    Open();
}

//
//
//
void Dataset::Open() {
    fileIn.open(fileInPath);
    fileOut.open(fileOutPath);
    if (fileIn.fail() || fileOut.fail())
        exit(1);
}

//
//
//
void Dataset::Close() {
    fileIn.close();
    fileOut.close();
}
