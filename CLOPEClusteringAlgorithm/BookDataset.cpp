#include <sstream>

#include "BookDataset.h"
#include "WordDataset.h"
#include "CLOPEClusteringAlgorithm.h"

//
//
//
BookDataset::BookDataset(std::string filePath) : Dataset(filePath) {
    Open();
    Reformat();
    Reopen();
}

//
//
//
void BookDataset::Reformat() {
    std::ofstream wordDatasetFile;
    std::fstream rawWordsFile;
    std::ofstream clusteredWords;

    std::string wordDatasetFilePath = "D:/Temp/words.txt";
    std::string rawWordsFilePath = "D:/Temp/raw-words.txt";
    std::string clusteredWordsPath = "D:/Temp/clustered-words.txt";

    wordDatasetFile.open(wordDatasetFilePath);
    rawWordsFile.open(rawWordsFilePath, std::ios_base::out);
    clusteredWords.open(clusteredWordsPath);

    if (wordDatasetFile.fail() || rawWordsFile.fail() || clusteredWords.fail())
        exit(1);

    char c;
    std::string origLine;
    while (fileIn.get(c)) {
        if (c != '\n') {
            origLine += c;
        }
        else {
            // ������� ������ ������� ������ �� �������� ����� (������ ������� - ��������� ������ PowerShell-�������, ������� �������� ��������).
            // � ����� �� ������� ����� �� ���������� ���������� � PowerShell, ������� ���� ������� ��������� �����.
            std::string::iterator p;
            for (p = origLine.end(); p != origLine.begin() && *--p == ' ';) {
                if (*p != ' ')
                    p++;

                origLine.erase(p, origLine.end());
            }

            // ��������� �������� �� ��������� �����, ����������� ��������.
            std::stringstream ss(origLine);
            std::string reformattedLine;
            std::string item;
            while (ss.good()) {
                std::getline(ss, item, ' ');

                // ��������� ������� �� ��������� ����,
                // � �������� �� ����� �������� ������������� CLOPE,
                // ����� ������������� ������� ����� ����� �����,
                // ����� ����� � ������������ �������� �������� ������ �� ��������� �� ������� �� ������ �����
                // �� ����� ����� ������.
                // ����� �������, ���� ��� ����� �������� � ����� ��������� ������������ �����, � ������� ���������, ������, ���������,
                // ����� ��� ����� �������� �� ����� �������� � �� ������� � ����������� �������� ���������� item ������� ���������� ����� ����������.
                // TODO: ������� ����� �������������.

                // ��������� ����� � ����� ��������.
                // ���������� ������ Reformat � WordDataset ����� ��������� ������ ����� ���������� �����.
                // ���������� ������ ���� ����������, ���� ��� ���������� ������������� ����� ����� ���� ���������� �� ���������� �������� ��������,
                // �� ��� �� ������, ��� ����� ������� ���� ����������,
                // ������� ����� ��������� � ������� ������� � �����.
                if (item != "") {
                    std::string itemCommaSeparated;
                    for (char& ch : item) {
                        itemCommaSeparated += ch;
                        itemCommaSeparated += ',';
                    }
                    itemCommaSeparated.pop_back();
                    wordDatasetFile << itemCommaSeparated << std::endl;

                    // ��������� ���� � ��������� �������, ����� ����� ����������� ������������� � �����.
                    rawWordsFile << item << std::endl;
                }

                reformattedLine += item + ',';
            }
            reformattedLine.pop_back();

            fileOut << reformattedLine << std::endl;
            origLine = "";
        }
    }
    wordDatasetFile.close();
    rawWordsFile.close();

    // ���������� ������� �����.
    WordDataset words(wordDatasetFilePath);
    // ����� ���������, ��� ��� ������� ��������, ������������� ������ ����� ������� �����, ������ �� ����������.
    // ������� ��������� ���������� ������������ ����� ���������.
    unsigned short iterations = CLOPEClusteringAlgorithm::Apply(words, 3.0f);

    // ��������� ���-������� �������� ���� � ������� ���������.
    // ��������� ����� �������� ���������� ��������� ����������, ��������� ��� ���������� �������, ����� ��������� ����� �������� ��� � ����� ��������.
    words.Reopen();
    Transaction wordTransac;
    // � ����� ���������������� ���� ������� ���� ��������� ��������, � ��� ����� ������������ �� �������� �������������, ����� �������� ������������ �����->����� ��������.
    // ����� n-� ����������, �� �� ���� ������ n-� ������ �����.
    // ������� �� ����� ������� ������� ���� � ��������� ������� � ������ ��� ��������������� ������ � ����������������.
    rawWordsFile.open(rawWordsFilePath, std::ios_base::in);
    std::string origWord;
    std::unordered_map<std::string, std::string> wordToClusterId;
    while (words.ReadNextTransaction(wordTransac)) {
        unsigned short clusterId = wordTransac.clusterId;
        rawWordsFile >> origWord;

        wordToClusterId[origWord] = "c" + std::to_string(clusterId); // ��������� ������ "c", ����� ��������� ����� �������� � ������� ����������.

        clusteredWords << "\"" + origWord + "\"," + std::to_string(clusterId) << std::endl;
    }
    rawWordsFile.close();
    clusteredWords.close();
    words.Close();

    // �������� ����� �� �������� ���������,
    // ����� �������� ���������� ����� ���������� ���� � ������� �� ���������,
    // �� �������� �� ������ �������.
    Reopen(); // ������������� ������� �������.
    do {
        std::string srcLine;
        fileIn >> srcLine;

        if (srcLine == "")
            break;

        std::stringstream ss(srcLine);
        std::string reformattedLine;
        std::string itemValue;
        while (ss.good()) {
            std::getline(ss, itemValue, ',');

            // �������� ����� �� �������.
            std::string wordClusterName = wordToClusterId[itemValue];
            reformattedLine += wordClusterName + ",";
        }
        reformattedLine.pop_back(); // ������� ��������� �������.

        fileOut << reformattedLine << std::endl;
    } while (true);
}

//
//
//
void BookDataset::PostProcess() {
    Close();
    std::filesystem::remove(fileOutPath);
    Open();

    std::ifstream origDatasetFile;
    origDatasetFile.open(std::filesystem::path(fileInPath).replace_filename("my-books-clean.txt"));

    if (origDatasetFile.fail())
        exit(1);

    Transaction t;
    while (ReadNextTransaction(t)) {
        unsigned int clusterId = t.clusterId;

        // �������� � �������� �������� ������ ������� ����� PowerShell, ������� ������ ��� � � PreProcess.
        char c;
        std::string origBookName;
        while (origDatasetFile.get(c)) {
            if (c != '\n') {
                origBookName += c;
            }
            else { // �������� �� \n
                // ������� ������ ������� ������ �� �������� ����� (������ ������� - ��������� ������ PowerShell-�������, ������� �������� ��������).
                // � ����� �� ������� ����� �� ���������� ���������� � PowerShell, ������� ���� ������� ��������� �����.
                std::string::iterator p;
                for (p = origBookName.end(); p != origBookName.begin() && *--p == ' ';) {
                    if (*p != ' ')
                        p++;

                    origBookName.erase(p, origBookName.end());
                }

                break;
            }
        }

        fileOut << "\"" + origBookName + "\"" + ',' + std::to_string(clusterId) << std::endl;
    }
    origDatasetFile.close();

    Reopen();
}
