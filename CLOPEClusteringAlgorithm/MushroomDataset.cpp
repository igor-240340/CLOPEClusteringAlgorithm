#include <sstream>

#include "MushroomDataset.h"

//
//
//
MushroomDataset::MushroomDataset(std::string filePath) : Dataset(filePath) {
    PreProcess();
    Reopen();
}

//
// Проставляет уникальные номера значениям атрибутов транзакции.
//
void MushroomDataset::PreProcess() {
    do {
        std::string srcLine;
        fileIn >> srcLine;

        if (srcLine == "")
            break;

        std::stringstream ss(srcLine);
        int attributeIndex = 0;
        std::string reformattedLine;
        std::string itemValue;
        std::getline(ss, itemValue, ','); // Пропускает первый символ, который обозначет признак poisonos/edible, который не является значением атрибута в датасете грибов.
        while (ss.good()) {
            std::getline(ss, itemValue, ',');

            // Пропускаем неопределенное значение атрибута.
            if (itemValue != "?")
                reformattedLine += itemValue + std::to_string(attributeIndex) + ",";

            attributeIndex++;
        }
        reformattedLine.pop_back(); // Удаляет последнюю запятую.

        fileOut << reformattedLine << std::endl;
    } while (true);
}

//
//
//
MushroomDataset::~MushroomDataset() {
    Reopen();
    PostProcess();
}

//
// Берет строки исхдного файла и проставляет им номера кластеров.
//
void MushroomDataset::PostProcess() {
    std::ifstream sourceDatasetFile;
    sourceDatasetFile.open(sourceFilePath);

    if (sourceDatasetFile.fail())
        exit(1);

    Transaction t;
    while (ReadNextTransaction(t)) {
        std::string srcLine;
        sourceDatasetFile >> srcLine;
        fileOut << srcLine + "," + std::to_string(t.clusterId) << std::endl;
    }

    sourceDatasetFile.close();
}
