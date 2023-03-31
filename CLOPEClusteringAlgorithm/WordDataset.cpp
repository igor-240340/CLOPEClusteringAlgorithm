#include "WordDataset.h"

//
//
//
WordDataset::WordDataset(std::string filePath) : Dataset(filePath) {
    PreProcess();
    Reopen();
}

//
//
//
void WordDataset::PreProcess() {
    do {
        std::string word;
        std::getline(fileIn, word);

        if (word == "")
            break;

        // ��������� ����� ��������, ���������� �� ���������� ������.
        // �� ������� �� ����� ��� �� �����, � �� ������� �������� - ��� �� �������� ������, � �� ���� ������� � �������� - ��� �� �������� ���������.
        // ��� ���������� ������������� ����� ����� ���� ���������� �� ���������� �������� ��������,
        // ������� ���������� ������� � ���������� �������� ������ ���������.
        // � �� �� ���������� �������, �� � ������ �������� - �����������.
        unsigned short attributeIndex = 0;
        std::string wordCommaSeparated;
        for (char& ch : word) {
            wordCommaSeparated += ch;
            wordCommaSeparated += std::to_string(attributeIndex++);
            wordCommaSeparated += ',';
        }
        wordCommaSeparated.pop_back();

        fileOut << wordCommaSeparated << std::endl;
    } while (true);
}

//
//
//
WordDataset::~WordDataset() {
    Reopen();
    PostProcess();
}

//
// ����� ������ ��������� ����� � ����������� �� ������ ���������.
// ����� ��������� �� ���������, ������ ��� �� �������� ��� ����� � ������ ���� ��� ������������� � �������� BookDataset.
//
void WordDataset::PostProcess() {
    std::ifstream wordsFile;
    wordsFile.open(sourceFilePath);

    if (wordsFile.fail())
        exit(1);

    Transaction t;
    while (ReadNextTransaction(t)) {
        std::string word;
        std::getline(wordsFile, word);
        fileOut << word + "," + std::to_string(t.clusterId) << std::endl;
    }

    wordsFile.close();
}
