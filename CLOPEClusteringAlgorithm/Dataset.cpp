#include <iostream>
#include <sstream>

#include "Dataset.h"

Dataset::Dataset() {
}

//
// ������� ����� ������������� �������� � ����������� ���������� ��������� ���������� ������.
// ��������� ����� �������� �� ������, � ��������� ���� - �� ������.
//
Dataset::Dataset(std::string filePath) {
    // ������� ����� ������������� �����.
    copyFilePath = std::filesystem::path(filePath).replace_filename("dataset_copy.txt");
    std::filesystem::copy(filePath, copyFilePath);

    tmpCopyFilePath = std::filesystem::path(copyFilePath).replace_filename("dataset_copy_tmp.txt");

    Reformat();

    fileIn.open(copyFilePath);
    fileOut.open(tmpCopyFilePath);
    if (fileIn.fail() || fileOut.fail())
        exit(1);
}

//
//
//
Dataset::~Dataset() {
    fileIn.close();
    fileOut.close();

    std::filesystem::remove(tmpCopyFilePath);
}

//
// ����������� ���������� ������ ��������� ��������� ����������.
//
void Dataset::Reformat() {
    fileIn.open(copyFilePath);
    fileOut.open(tmpCopyFilePath);
    if (fileIn.fail() || fileOut.fail())
        exit(1);

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

    fileIn.close();
    fileOut.close();

    // ��������� ������������ ����� �������������������.
    std::filesystem::remove(copyFilePath);
    std::filesystem::rename(tmpCopyFilePath, copyFilePath);
}

//
//
//
void Dataset::Close() {
    fileIn.close();
    fileOut.close();
}

//
// ������ ��������� ������ �������� � ��������� ������ �������� ��������� (items).
// ���� � ���������� ���� ����� ��������, �� ����������� ���.
//
bool Dataset::ReadNextTransaction(Transaction& t) {
    t.items.clear();

    std::string line;
    fileIn >> line;

    t.sourceString = line;

    if (line == "")
        return false;

    std::stringstream ss(line);
    while (ss.good()) {
        std::string itemValue;
        std::getline(ss, itemValue, ',');

        // ���� ��������� �������� ���������� � �����, ������ ����� ���� ����� ��������, � �� �������� ��������.
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
// ���������� ���������� � �������� ���� � ����������� ������� ��������.
//
void Dataset::WriteTransaction(Transaction& t) {
    for (const auto& i : t.items) {
        fileOut << i << ",";
    }

    fileOut << std::to_string(t.clusterId) << std::endl;
}

//
// ������������� �������� ���� � ����������� ����������� � ������������� ��� �������
// ��� ��������� �������� ������������� ���������� �� ���������.
//
void Dataset::Reopen() {
    Close();

    std::filesystem::remove(copyFilePath);
    std::filesystem::rename(tmpCopyFilePath, copyFilePath);

    fileIn.open(copyFilePath);
    fileOut.open(tmpCopyFilePath);
    if (fileIn.fail() || fileOut.fail())
        exit(1);
}

void Dataset::OpenFile(std::string fileName, std::string postfix) {
    fileIn.open(fileName);
    fileOut.open(postfix);

    if (fileIn.fail() || fileOut.fail())
        exit(1);
}
