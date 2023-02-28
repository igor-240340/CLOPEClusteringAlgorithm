#include <iostream>
#include <sstream>

#include "Dataset.h"

Dataset::Dataset(std::string fileName) {
    file.open(fileName);
    if (file.fail())
        exit(1);
}

Dataset::~Dataset() {
    file.close();
}

bool Dataset::ReadNextTransaction(Transaction& t) {
    std::string line;
    file >> line;

    std::stringstream ss(line);
    while (ss.good()) {
        std::string itemValue;
        std::getline(ss, itemValue, ',');
        t.items.push_back(itemValue);
    }

    return true;
}

void Dataset::WriteTransaction(Transaction& t) {
}

void Dataset::Rewind() {
    std::cout << "Dataset::Rewind" << std::endl;
}
