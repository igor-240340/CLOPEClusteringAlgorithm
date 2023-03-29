#include <sstream>

#include "WordDataset.h"

//
//
//
WordDataset::WordDataset(std::string filePath) : Dataset(filePath) {
    Open();
    Reformat();
    Reopen();
}

//
// ����������� ���������� ������ ��������� ��������� ����������.
//
void WordDataset::Reformat() {
    do {
        std::string srcLine;
        fileIn >> srcLine;

        if (srcLine == "")
            break;

        std::stringstream ss(srcLine);
        int attributeIndex = 0;
        std::string reformattedLine;
        std::string itemValue;
        while (ss.good()) {
            std::getline(ss, itemValue, ',');

            reformattedLine += itemValue + std::to_string(attributeIndex) + ",";

            attributeIndex++;
        }
        reformattedLine.pop_back(); // ������� ��������� �������.

        fileOut << reformattedLine << std::endl;
    } while (true);
}
