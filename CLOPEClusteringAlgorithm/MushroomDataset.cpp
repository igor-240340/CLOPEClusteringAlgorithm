#include <sstream>

#include "MushroomDataset.h"

//
//
//
MushroomDataset::MushroomDataset(std::string filePath) : Dataset(filePath) {
    Open();
    Reformat();
    Reopen();
}

//
// Проставляет уникальные номера значениям атрибутов транзакции.
//
void MushroomDataset::Reformat() {
    do {
        std::string srcLine;
        fileIn >> srcLine;

        if (srcLine == "")
            break;

        std::stringstream ss(srcLine);
        int attributeIndex = 0;
        std::string reformattedLine;
        std::string itemValue;
        std::getline(ss, itemValue, ','); // Пропускает первый символ, который обозначет признак poisonos/edible, который не является значением атрибута в дадасете грибов.
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
