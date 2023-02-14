#include <iostream>

#include "Dataset.h"

//Dataset::Dataset() {
//    std::cout << "Dataset()" << std::endl;
//}

//Dataset::~Dataset() {
//    std::cout << "~Dataset()" << std::endl;
//}

bool Dataset::ReadNextTransaction(Transaction& t) {
    std::cout << "Dataset::ReadNextTransaction" << std::endl;

    t.name = "foo";
    return true;
}

void Dataset::WriteTransaction(Transaction& t) {
    std::cout << "Dataset::WriteTransaction. t.name = " << t.name << std::endl;
}
