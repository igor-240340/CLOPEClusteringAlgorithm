#include <iostream>
#include <sstream>

#include "Dataset.h"

Dataset::Dataset(std::string fileName) {
    fileIn.open(fileName);
    fileOut.open(fileName + "_after_init");

    if (fileIn.fail() || fileOut.fail())
        exit(1);
}

Dataset::~Dataset() {
    fileIn.close();
    fileOut.close();
}

bool Dataset::ReadNextTransaction(Transaction& t) {
    t.items.clear();

    std::string line;
    fileIn >> line;

    if (line == "")
        return false;

    std::stringstream ss(line);
    int attributeIndex = 0;
    while (ss.good()) {
        std::string itemValue;
        std::getline(ss, itemValue, ',');

        // Пропускаем, если для текущего атрибута значение не определено.
        if (itemValue != "?") {
            t.items.push_back(itemValue + std::to_string(attributeIndex));
        }

        attributeIndex++;
    }

    return true;
}

void Dataset::WriteTransaction(Transaction& t) {
    for (const auto& i : t.items) {
        fileOut << i << ",";
    }

    fileOut << ": " << std::to_string(t.clusterId) << std::endl;
}

void Dataset::Rewind() {
    std::cout << "Dataset::Rewind" << std::endl;
}
