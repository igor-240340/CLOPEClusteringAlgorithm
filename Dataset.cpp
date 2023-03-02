#include <iostream>
#include <sstream>

#include "Dataset.h"

Dataset::Dataset() {
}
void Dataset::OpenFile(std::string fileName, std::string postfix) {
    fileIn.open(fileName);
    fileOut.open(postfix);

    if (fileIn.fail() || fileOut.fail())
        exit(1);
}

void Dataset::CloseFiles() {
    fileIn.close();
    fileOut.close();
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

        // Если у транзакции есть номер кластера.
        int clusterId = -1;
        if (attributeIndex > 20) {
            try {
                clusterId = std::stoi(itemValue.c_str(), nullptr, 10);
                t.clusterId = clusterId;
            }
            catch (std::invalid_argument const& ex)
            {
                //std::cout << "std::invalid_argument::what(): " << ex.what() << std::endl;
            }
            catch (std::out_of_range const& ex)
            {
                //std::cout << "std::out_of_range::what(): " << ex.what() << std::endl;
            }
        }

        // Пропускаем, если для текущего атрибута значение не определено.
        if (itemValue != "?" && clusterId == -1) {
            t.items.push_back(itemValue.length() > 1 ? itemValue : itemValue + std::to_string(attributeIndex));
        }

        attributeIndex++;
    }

    return true;
}

void Dataset::WriteTransaction(Transaction& t) {
    for (const auto& i : t.items) {
        fileOut << i << ",";
    }

    fileOut << std::to_string(t.clusterId) << std::endl;
}

void Dataset::Rewind() {
    std::cout << "Dataset::Rewind" << std::endl;
}
