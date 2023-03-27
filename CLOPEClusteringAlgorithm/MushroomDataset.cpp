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
// ����������� ���������� ������ ��������� ��������� ����������.
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
        std::getline(ss, itemValue, ','); // ���������� ������ ������, ������� ��������� ������� poisonos/edible, ������� �� �������� ��������� �������� � �������� ������.
        while (ss.good()) {
            std::getline(ss, itemValue, ',');

            // ���������� �������������� �������� ��������.
            if (itemValue != "?")
                reformattedLine += itemValue + std::to_string(attributeIndex) + ",";

            attributeIndex++;
        }
        reformattedLine.pop_back(); // ������� ��������� �������.

        fileOut << reformattedLine << std::endl;
    } while (true);
}
