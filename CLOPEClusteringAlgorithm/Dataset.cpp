#include <iostream>
#include <sstream>

#include "Dataset.h"

//
// Создает копию оригинального файла датасета.
// Открывает копию на чтение, а временный файл - на запись.
//
Dataset::Dataset(std::string filePath) {
    sourceFilePath = filePath;

    std::string srcFileName = std::filesystem::path(filePath).filename().string();

    fileInPath = std::filesystem::path(filePath).replace_filename("copy_" + srcFileName);
    fileOutPath = std::filesystem::path(filePath).replace_filename("tmp_copy_" + srcFileName);

    std::filesystem::copy(filePath, fileInPath);

    Open();
}

//
// Результат обработки датасета (в т.ч. и постобработки в дочернем классе) на данном этапе находится во временном выходном файле,
// поэтому чтобы его не потерять, мы перед закрытием переоткрываем выходной файл как входной, а новый временный выходной удаляем.
//
Dataset::~Dataset() {
    Reopen();
    Close();

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
// Переоткрывает выходной файл как входной.
//
void Dataset::Reopen() {
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
